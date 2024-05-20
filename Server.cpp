/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/16 20:55:04 by scarpent         ###   ########.fr       */
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
    //fcntl(_server_fd, F_SETFL, O_NONBLOCK | fcntl(_server_fd, F_GETFL, 0));
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
    //fcntl(new_socket, F_SETFL, O_NONBLOCK | fcntl(new_socket, F_GETFL, 0));
    if (new_socket < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    Clients newClient;
    newClient.set_Socket(new_socket);
    _clients.push_back(newClient);
    std::cout << "New connection accepted" << std::endl;
    if (_clients[new_socket - 4].get_Username().compare("") == 0)
	{
		char buf[BUFFER_SIZE];

		send(new_socket, "Set a Username : ", 18, 0);
		int userRead = recv(new_socket, buf, BUFFER_SIZE, 0);
		buf[userRead - 1] = '\0';
		_clients[new_socket - 4].set_Username(buf);		
        send(new_socket, "Set a Nickname : ", 18, 0);
        int nickRead = recv(new_socket, buf, BUFFER_SIZE, 0);
        buf[nickRead - 1] = '\0';
        _clients[new_socket - 4].set_Nickname(buf);
		std::string Username = "Welcome " + _clients[new_socket - 4].get_Username() + " " + _clients[new_socket - 4].get_Nickname() + '\n';
		send(new_socket, Username.c_str(), Username.size(), 0);
	}
}

void    Server::handleClientMessage(int client_socket)
{
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread == 0)
    {
        close(client_socket);
        std::cout << "Client disconnected" << std::endl;
        return;
    }
    buffer[valread] = '\0';
    std::cout << "Received message: " << std::endl << buffer << std::endl;

    // Echo message back to client
    send(client_socket, buffer, valread, 0);
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
                    handleClientMessage(pollfds[i].fd);
            }
        }
    }
}
