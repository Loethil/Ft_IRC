/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/28 18:58:12 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _server_fd(-1), _server_name("I.R.SIUSIU") {}

Server::~Server()
{
	if (_server_fd != -1)
		close(_server_fd);
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i]->get_Socket());
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
		close(_server_fd);
		throw std::runtime_error("Bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(_server_fd, 3) < 0)
	{
		close(_server_fd);
		throw std::runtime_error("Listen failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "The server started with success !" << std::endl;
}

//fonction permettant de lancer le serveur
void	Server::run()
{
	if (_server_fd == -1)
	{
		throw std::runtime_error("Server not initialized");
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
			throw std::runtime_error("Poll error");
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
					handleClientMessage(client_socket, _clients[client_socket]->get_Status());
				}
			}
		}
	}
}

//fonction permettant d'accepter de nouveau clients et de lui donner ses bons parametres
void	Server::acceptNewConnection()
{
	socklen_t addrlen = sizeof(_cli_adr);
	int new_socket = accept(_server_fd, (struct sockaddr *)&_cli_adr, &addrlen);
	if (new_socket < 0)
	{
		throw std::runtime_error("Accept failed");
		exit(EXIT_FAILURE);
	}
	fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	Clients *newClient = new Clients();
	newClient->set_Socket(new_socket);
	newClient->set_Status(Clients::USERNAME); //modif
	_clients[new_socket] = newClient;
	std::cout << "New connection accepted: " << new_socket << std::endl;
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
				join(client, lineStream, client_socket);
			else if (command == "PRIVMSG")
				msg(client, lineStream, buffer);
			else if (command == "TOPIC")
				topic(client, lineStream, client_socket);
			else if (command == "NICK")
				nick(client, lineStream);
			else if (command == "PART")
				part(client, lineStream);
			else if (command == "MODE")
				mode(client, lineStream);
			else if (valread == 0)
			{
				close(client_socket);
				std::map<int, Clients*>::iterator it = _clients.find(client_socket);
				if (it != _clients.end())
				{
					delete it->second;
					_clients.erase(it->first);
				}
				std::cout << "Client disconnected" << std::endl;
				return;
			}
			else
				regularChat(client, lineStream, buffer);
		}
	}
}

//fonction permettant d'afficher les messages de bienvenu a l'utilisateur
void Server::sendWelcomeMessages(int client_socket, Clients *client)
{
	std::string welcomeMsg = ":I.R.SIUSIU 001 " + client->get_Nickname() + " :" GREEN "Welcome to the IRC server, " + client->get_Realname() + RESET "\n";
	send(client_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);
	std::cout << "Sent welcome message: " << welcomeMsg;

	std::string yourHost = ":I.R.SIUSIU 002 " + client->get_Nickname() + " :" GREEN "Your host is I.R.SIUSIU, running version 1.0" RESET "\n";
	send(client_socket, yourHost.c_str(), yourHost.size(), 0);
	std::cout << "Sent yourHost message: " << yourHost;

	std::string created = ":I.R.SIUSIU 003 " + client->get_Nickname() + "\n";
	send(client_socket, created.c_str(), created.size(), 0);
	std::cout << "Sent created message: " << created;

	std::string myInfo = ":I.R.SIUSIU 004 " + client->get_Nickname() + "\n";
	send(client_socket, myInfo.c_str(), myInfo.size(), 0);
	std::cout << "Sent myInfo message: " << myInfo;

	std::string motdStart = ":I.R.SIUSIU 375 " + client->get_Nickname() + " :" YELLOW "------ I.R.SIUSIU Message of the Day ------" RESET "\n";
	send(client_socket, motdStart.c_str(), motdStart.size(), 0);
	std::cout << "Sent MOTD start: " << motdStart;

	std::string motdSpaces = ":I.R.SIUSIU 372 " + client->get_Nickname() + " :" YELLOW "|                                         |" RESET "\n";
	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd = ":I.R.SIUSIU 372 " + client->get_Nickname() + " :" YELLOW "| Enjoy our Internet Relay chat " + client->get_Nickname() + " ! |" RESET "\n";
	send(client_socket, motd.c_str(), motd.size(), 0);

	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd2 = ":I.R.SIUSIU 372 " + client->get_Nickname() + " :" YELLOW "-------------------------------------------" RESET "\n";
	send(client_socket, motd2.c_str(), motd2.size(), 0);

	std::string motdEnd = ":I.R.SIUSIU 376 " + client->get_Nickname() + " :End of Message Of The Day\n";
	send(client_socket, motdEnd.c_str(), motdEnd.size(), 0);
	std::cout << "Sent MOTD end: " << motdEnd;
}
