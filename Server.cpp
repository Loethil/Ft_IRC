/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/17 14:39:52 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _server_fd(-1) {}

Server::~Server()
{
    if (_server_fd != -1)
        close(_server_fd);
    for (size_t i = 0; i < _clients.size(); ++i)
        close(_clients[i].get_Socket());
}

std::string Server::getPwd()
{
    return _pwd;
}

void    Server::setPwd(std::string pwd)
{
    _pwd = pwd;
}

void    Server::setPort(int port)
{
    _port = port;
}

void    Server::start(int port)
{
    std::cout << "Starting the server on port " << port << "..." << std::endl;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
    {
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    fcntl(_server_fd, F_SETFL, O_NONBLOCK | fcntl(_server_fd, F_GETFL, 0));
    _serv_adr.sin_family = AF_INET;
    _serv_adr.sin_addr.s_addr = INADDR_ANY;
    _serv_adr.sin_port = htons(port);
    if (bind(_server_fd, (struct sockaddr *)&_serv_adr, sizeof(_serv_adr)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        close(_server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(_server_fd, 3) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        close(_server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "The server started with success !" << std::endl;
}

void    Server::acceptNewConnection()
{
    socklen_t addrlen = sizeof(_cli_adr);
    int new_socket = accept(_server_fd, (struct sockaddr *)&_cli_adr, &addrlen);
    fcntl(new_socket, F_SETFL, O_NONBLOCK | fcntl(new_socket, F_GETFL, 0));
    if (new_socket < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    Clients newClient;
    newClient.set_Socket(new_socket);
    _clients.push_back(newClient);
    std::cout << "New connection accepted" << std::endl;
    // Username
    std::string username;
    char buffer[BUFFER_SIZE];
    while (true) {
        int userRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (userRead < 0) {
            std::cerr << "Error receiving username " << buffer << std::endl;
            close(new_socket);
            return;
        } else if (userRead == 0) {
            std::cerr << "Client disconnected while receiving username" << std::endl;
            close(new_socket);
            return;
        }
        username.append(buffer, userRead);
        if (username.find('\n') != std::string::npos) { // Check for newline (complete username)
            username.erase(username.find('\n')); // Remove newline character
            _clients[new_socket - 4].set_Username(username);
            send(new_socket, "Set a Nickname : ", 18, 0);
            break;
        }
    }

    // Nickname
    std::string nickname;
    while (true) {
        int nickRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (nickRead < 0) {
            std::cerr << "Error receiving nickname" << std::endl;
            close(new_socket);
            return;
        } else if (nickRead == 0) {
            std::cerr << "Client disconnected while receiving nickname" << std::endl;
            close(new_socket);
            return;
        }
        nickname.append(buffer, nickRead);
        if (nickname.find('\n') != std::string::npos) { // Check for newline (complete nickname)
            nickname.erase(nickname.find('\n')); // Remove newline character
            _clients[new_socket - 4].set_Nickname(nickname);
            std::string welcomeMsg = "Welcome " + _clients[new_socket - 4].get_Username() + " " + _clients[new_socket - 4].get_Nickname() + '\n';
            send(new_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);
            break;
        }
    }
}

ssize_t    Server::handleClientMessage(int client_socket)
{
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread == 0)
    {
        close(client_socket);
        std::cout << "Client disconnected" << std::endl;
        return valread;
    }
    else if (valread < 0)
    {
        if (errno == EWOULDBLOCK)
            return 0;
    }
    else
    {
        std::cerr << "Read error on client socket: " << client_socket << std::endl;
        close(client_socket);
        return -1;
    }
    buffer[valread] = '\0';
    std::cout << "Received message: " << std::endl << buffer << std::endl;

    // Echo message back to client
    send(client_socket, buffer, valread, 0);
    return valread;
}

void    Server::run()
{
    if (_server_fd == -1)
    {
        std::cerr << "Server not initialized" << std::endl;
        return;
    }

    std::vector<struct pollfd> pollfds;
    struct pollfd server_pollfd;

    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    pollfds.push_back(server_pollfd);

    while (true)
    {
        int poll_count = poll(pollfds.data(), pollfds.size(), -1);
        if (poll_count < 0)
        {
            std::cerr << "Poll error" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < pollfds.size(); ++i)
        {
            if (pollfds[i].revents & POLLIN)
            {
                if (pollfds[i].fd == _server_fd)
                {
                    acceptNewConnection();
                    struct pollfd client_pollfd;
                    client_pollfd.fd = _clients.back().get_Socket();
                    client_pollfd.events = POLLIN;
                    pollfds.push_back(client_pollfd);
                }
                else
                {
                    ssize_t valread = handleClientMessage(pollfds[i].fd);
                    if (!valread)
                        pollfds.erase(pollfds.begin() + i);
                }
            }
        }
    }
}
