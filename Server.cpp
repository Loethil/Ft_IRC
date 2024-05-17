/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/17 18:38:50 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>

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
    // fcntl(_server_fd, F_SETFL, O_NONBLOCK);
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

void Server::acceptNewConnection()
{
    socklen_t addrlen = sizeof(_cli_adr);
    int new_socket = accept(_server_fd, (struct sockaddr *)&_cli_adr, &addrlen);
    if (new_socket < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    Clients newClient;
    newClient.set_Socket(new_socket);
    newClient.set_Status(Clients::PASSWORD);
    _clients[new_socket] = newClient;

    std::cout << "New connection accepted: " << new_socket << std::endl;
    send(new_socket, "Please enter the password: ", 28, 0);
}



void Server::handleClientMessage(int client_socket, Clients::status status)
{
    char buffer[BUFFER_SIZE + 1]; // +1 for null terminator
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        perror("read");
        close(client_socket);
        _clients.erase(client_socket);
        return;
    }
    buffer[valread] = '\0'; // Properly terminate the read string

    std::string pwd = _pwd + "\n";
    Clients &client = _clients[client_socket];
    std::string welcomeMsg;
    
    switch (status)
    {
        case Clients::PASSWORD:
            if (pwd.compare(buffer) == 0)
            {
                client.set_Status(Clients::USERNAME);
                send(client_socket, "Set a Username: ", 17, 0);
            }
            else
            {
                send(client_socket, "Invalid password, please try again...", 38, 0);
            }
            break;
        case Clients::USERNAME:
            buffer[valread - 1] = '\0';
            client.set_Username(buffer);
            send(client_socket, "Set a Nickname: ", 17, 0);
            client.set_Status(Clients::NICKNAME);
            break;
        case Clients::NICKNAME:
            buffer[valread - 1] = '\0';
            client.set_Nickname(buffer);
            client.set_Status(Clients::COMPLETED);
            welcomeMsg = "Welcome " + client.get_Username() + " " + client.get_Nickname() + '\n';
            send(client_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);
            break;
        case Clients::COMPLETED:
            if (valread == 0)
            {
                close(client_socket);
                _clients.erase(client_socket);
                std::cout << "Client disconnected" << std::endl;
                return;
            }
            std::cout << "Received message: " << buffer << std::endl;
            send(client_socket, buffer, valread, 0);
            break;
    }
}



void Server::run()
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
                    int new_socket = _clients.rbegin()->first; // Get the last added client's socket
                    struct pollfd client_pollfd;
                    client_pollfd.fd = new_socket;
                    client_pollfd.events = POLLIN;
                    pollfds.push_back(client_pollfd);
                }
                else
                {
                    int client_socket = pollfds[i].fd;
                    handleClientMessage(client_socket, _clients[client_socket].get_Status());
                }
            }
        }
    }
}


