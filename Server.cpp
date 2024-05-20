/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/20 16:21:42 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>
#include <sstream>

Server::Server() : _server_fd(-1), _server_name("I.R.SIUSIU") {}

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

void	Server::setPwd(std::string pwd)
{
	_pwd = pwd;
}

void	Server::setPort(int port)
{
	_port = port;
}

void	Server::start(int port)
{
	std::cout << "Starting the server on port " << port << "..." << std::endl;
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
	{
		std::cerr << "Socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	fcntl(_server_fd, F_SETFL, O_NONBLOCK);
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

void	Server::acceptNewConnection()
{
	socklen_t addrlen = sizeof(_cli_adr);
	int new_socket = accept(_server_fd, (struct sockaddr *)&_cli_adr, &addrlen);
	if (new_socket < 0)
	{
		std::cerr << "Accept failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	Clients newClient;
	newClient.set_Socket(new_socket);
	newClient.set_Status(Clients::USERNAME);
	_clients[new_socket] = newClient;

	std::cout << "New connection accepted: " << new_socket << std::endl;
	//send(new_socket, "Please enter the password: ", 28, 0);
}

void Server::handleClientMessage(int client_socket, Clients::status status)
{
    char buffer[BUFFER_SIZE + 1]; // +1 for null terminator
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        std::cerr << "Read failed" << std::endl;
        close(client_socket);
        _clients.erase(client_socket);
        return;
    }
    buffer[valread] = '\0';

    std::string msg(buffer);
    std::istringstream iss(msg);
    std::string line;

    Clients &client = _clients[client_socket];

    // Append received data to the client's partial data buffer
    client.partialData.append(buffer, valread);

    // Process complete lines from the client's partial data buffer
    size_t pos;
    while ((pos = client.partialData.find('\n')) != std::string::npos)
    {
        std::string line = client.partialData.substr(0, pos);
        client.partialData.erase(0, pos + 1);

        // Remove trailing '\r' if present
        if (!line.empty() && line[line.size() - 1] == '\r')
        {
            line.erase(line.size() - 1);
        }

        std::istringstream lineStream(line);
        std::string command;
        lineStream >> command;

        std::cout << "Command: " << command << std::endl;
		std::cout << "read buffer: " << buffer << std::endl;
        std::cout << "Client Status: " << client.get_Status() << std::endl;

        if (status == Clients::USERNAME)
        {
            if (command == "PASS")
            {
                std::string pass;
                lineStream >> pass;
                if (pass == _pwd)
                {
                    client.set_Status(Clients::USERNAME);
                }
                else
                {
                    send(client_socket, "Invalid password, try again...\n", 32, 0);
                    close(client_socket);
                    _clients.erase(client_socket);
                    return;
                }
            }
            else if (command == "NICK")
            {
                std::string nick;
                lineStream >> nick;
                client.set_Nickname(nick);
                std::cout << "Nickname set to: " << client.get_Nickname() << std::endl;
            }
            else if (command == "USER")
            {
                std::string user, mode, unused, realname;
                lineStream >> user >> mode >> unused;
                std::getline(lineStream, realname);
                if (!realname.empty() && realname[0] == ' ' && realname[1] == ':')
                    realname.erase(0, 2);
                client.set_Username(user);
                client.set_Realname(realname);
                client.set_Status(Clients::COMPLETED);
                std::cout << "Username set to: " << client.get_Username() << std::endl;
                std::cout << "Realname set to: " << client.get_Realname() << std::endl;
                sendWelcomeMessages(client_socket, client);
            }
        }
        else if (status == Clients::COMPLETED)
        {
            if (command == "JOIN")
            {
                std::string channel;
                lineStream >> channel;

                std::string joinMsg = ":" + client.get_Nickname() + "!" + client.get_Username() + "@localhost JOIN " + channel + "\r\n";
                std::cout << "Sent JOIN message: " << joinMsg << std::endl;
                send(client_socket, joinMsg.c_str(), joinMsg.size(), 0);

                std::string namesList = ":I.R.SIUSIU 353 " + client.get_Nickname() + " = " + channel + " :" + client.get_Nickname() + "\n";
                std::cout << "Sent NAMES list: " << namesList << std::endl;
                send(client_socket, namesList.c_str(), namesList.size(), 0);

                std::string endOfNames = ":I.R.SIUSIU 366 " + client.get_Nickname() + " " + channel + " :End of /NAMES list.\n";
                std::cout << "Sent end of NAMES list: " << endOfNames << std::endl;
                send(client_socket, endOfNames.c_str(), endOfNames.size(), 0);
            }
            else if (valread == 0)
            {
                close(client_socket);
                _clients.erase(client_socket);
                std::cout << "Client disconnected" << std::endl;
                return;
            }
            else
            {
                std::cout << "Received message: " << buffer << std::endl;
                send(client_socket, buffer, valread, 0);
            }
        }
    }
}

void Server::sendWelcomeMessages(int client_socket, Clients &client)
{
    std::string welcomeMsg = ":I.R.SIUSIU 001 " + client.get_Nickname() + " :Welcome to the IRC server, " + client.get_Realname() + "\n";
    send(client_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);
    std::cout << "Sent welcome message: " << welcomeMsg;

    std::string yourHost = ":I.R.SIUSIU 002 " + client.get_Nickname() + " :Your host is I.R.SIUSIU, running version 1.0\n";
    send(client_socket, yourHost.c_str(), yourHost.size(), 0);
    std::cout << "Sent yourHost message: " << yourHost;

    std::string created = ":I.R.SIUSIU 003 " + client.get_Nickname() + "\n";
    send(client_socket, created.c_str(), created.size(), 0);
    std::cout << "Sent created message: " << created;

    std::string myInfo = ":I.R.SIUSIU 004 " + client.get_Nickname() + "\n";
    send(client_socket, myInfo.c_str(), myInfo.size(), 0);
    std::cout << "Sent myInfo message: " << myInfo;

    std::string motdStart = ":I.R.SIUSIU 375 " + client.get_Nickname() + " :- I.R.SIUSIU Message of the Day -\n";
    send(client_socket, motdStart.c_str(), motdStart.size(), 0);
    std::cout << "Sent MOTD start: " << motdStart;

    std::string motdEnd = ":I.R.SIUSIU 376 " + client.get_Nickname() + " :End of /MOTD command.\n";
    send(client_socket, motdEnd.c_str(), motdEnd.size(), 0);
    std::cout << "Sent MOTD end: " << motdEnd;
}

void	Server::run()
{
	if (_server_fd == -1)
	{
		std::cerr << "Server not initialized" << std::endl;
		return;
	}

	std::vector<struct pollfd>	pollfds;
	struct pollfd				server_pollfd;

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
