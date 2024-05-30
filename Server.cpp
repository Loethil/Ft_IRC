/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/30 12:47:01 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _serverFd(-1), _serverName("I.R.SIUSIU") {}

Server::~Server()
{
	if (_serverFd != -1)
		close(_serverFd);
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i]->getSocket());
	if (!_clients.empty())
	{
		for (std::map<int, Clients *>::iterator delIt = _clients.begin(); delIt != _clients.end(); ++delIt)
		{
			delete delIt->second;
		}
		_clients.clear();
	}
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

//fonction permettant d'initialiser tout ce dont le serveur a besoin(ex: socket, bind, listen)
void	Server::start(int port)
{
	std::cout << "Starting the server on port " << port << "..." << std::endl;
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << "Socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	_serverAdr.sin_family = AF_INET;
	_serverAdr.sin_addr.s_addr = INADDR_ANY;
	_serverAdr.sin_port = htons(port);
	if (bind(_serverFd, (struct sockaddr *)&_serverAdr, sizeof(_serverAdr)) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(_serverFd, 3) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Listen failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "The server started with success !" << std::endl;
}

//fonction permettant de lancer le serveur
void	Server::run()
{
	if (_serverFd == -1)
	{
		throw std::runtime_error("Server not initialized");
		return;
	}

	std::vector<struct pollfd>	pollfds;
	struct pollfd				server_pollfd;

	server_pollfd.fd = _serverFd;
	server_pollfd.events = POLLIN;
	pollfds.push_back(server_pollfd);

	while (true)
	{
		int poll_count = poll(pollfds.data(), pollfds.size(), -1);
		if (poll_count < 0)
		{
			throw std::runtime_error("Poll error");
			exit(EXIT_FAILURE);
		}
		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == _serverFd)
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
					handleClientMessage(client_socket, _clients[client_socket]->getStatus());
				}
			}
		}
	}
}

//fonction permettant d'accepter de nouveau clients et de lui donner ses bons parametres
void	Server::acceptNewConnection()
{
	socklen_t addrlen = sizeof(_clientAdr);
	int new_socket = accept(_serverFd, (struct sockaddr *)&_clientAdr, &addrlen);
	if (new_socket < 0)
	{
		throw std::runtime_error("Accept failed");
		exit(EXIT_FAILURE);
	}
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	Clients *newClient = new Clients();
	newClient->setSocket(new_socket);
	newClient->setStatus(Clients::USERNAME); //modif
	_clients[new_socket] = newClient;
}

//fonction qui gere toutes les entrees de l'utilisateur 
void Server::handleClientMessage(int client_socket, Clients::status status)
{
	char buffer[BUFFER_SIZE + 1]; // +1 for null terminator
	ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
	if (valread < 0)
	{
		throw std::runtime_error("Read failed");
		close(client_socket);
		_clients.erase(client_socket);
		return;
	}
	buffer[valread] = '\0';
	Clients *client = _clients[client_socket];
	client->partialData.append(buffer, valread);
	size_t pos;

	while ((pos = client->partialData.find('\n')) != std::string::npos)
	{
		std::string line = client->partialData.substr(0, pos);
		client->partialData.erase(0, pos + 1);
		if (!line.empty() && line[line.size() - 1] == '\r')  // Remove trailing '\r' if present
			line.erase(line.size() - 1);
		std::istringstream lineStream(line);

		std::string command;
		lineStream >> command;
		std::cout << "read buffer: " << buffer << std::endl;

		if (status == Clients::USERNAME)
		{
			if (command == "PASS")
			{
				if (pass(client, lineStream, client_socket) == false)
					return ;
			}
			else if (command == "NICK")
				nick(client, lineStream);
			else if (command == "USER")
				user(client, lineStream, client_socket);
		}
		else if (status == Clients::COMPLETED)
		{
			if (command == "JOIN")
				join(client, lineStream);
			else if (command == "PRIVMSG")
				msg(client, lineStream, buffer);
			else if (command == "TOPIC")
				topic(client, lineStream);
			else if (command == "NICK")
				nick(client, lineStream);
			else if (command == "PART")
				part(client, lineStream);
			else if (command == "MODE")
				mode(client, lineStream);
			else if (command == "INVITE")
				invite(client, lineStream, client_socket);
			else if (command == "QUIT")
				quit(client, lineStream);
			else if (valread == 0)
			{
				close(client_socket);
				std::map<int, Clients*>::iterator it = _clients.find(client_socket);
				if (it != _clients.end())
				{
					delete it->second;
					_clients.erase(it->first);
				}
				return;
			}
			else
				std::string errmsg = ":I.R.SIUSIU 421 " + client->getNickname() + " " + command + " :" RED "The command " + command + "doesn't exist\n" RESET;
		}
	}
}

//fonction permettant d'afficher les messages de bienvenu a l'utilisateur
void Server::sendWelcomeMessages(int client_socket, Clients *client)
{
	std::string truncatedNick = client->getNickname().substr(0, 9); // Tronque le pseudo à 9 caractères
	std::string paddedNick = truncatedNick;
	if (truncatedNick.size() < 9)
		paddedNick += std::string(9 - truncatedNick.size(), ' ');

	std::string welcomeMsg = ":I.R.SIUSIU 001 " + client->getNickname() + " :" GREEN "Welcome to the IRC server, " + client->getRealname() + RESET "\n";
	send(client_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);

	std::string yourHost = ":I.R.SIUSIU 002 " + client->getNickname() + " :" GREEN "Your host is I.R.SIUSIU, running version 1.0" RESET "\n";
	send(client_socket, yourHost.c_str(), yourHost.size(), 0);

	std::string created = ":I.R.SIUSIU 003 " + client->getNickname() + "\n";
	send(client_socket, created.c_str(), created.size(), 0);

	std::string myInfo = ":I.R.SIUSIU 004 " + client->getNickname() + "\n";
	send(client_socket, myInfo.c_str(), myInfo.size(), 0);

	std::string motdStart = ":I.R.SIUSIU 375 " + client->getNickname() + " :" YELLOW "------ I.R.SIUSIU Message of the Day ------" RESET "\n";
	send(client_socket, motdStart.c_str(), motdStart.size(), 0);

	std::string motdSpaces = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "|                                         |" RESET "\n";
	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "| Enjoy our Internet Relay chat " + paddedNick + " |" RESET "\n";
	send(client_socket, motd.c_str(), motd.size(), 0);

	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd2 = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "-------------------------------------------" RESET "\n";
	send(client_socket, motd2.c_str(), motd2.size(), 0);

	std::string motdEnd = ":I.R.SIUSIU 376 " + client->getNickname() + " :End of Message Of The Day\n";
	send(client_socket, motdEnd.c_str(), motdEnd.size(), 0);
}
