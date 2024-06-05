#include "Server.hpp"

Server::Server() : _serverFd(-1), _serverName("I.R.SIUSIU") {}

Server::~Server() {}

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

//fonction permettant d'accepter de nouveau clients et de lui donner ses bons parametres
int	Server::acceptNewConnection()
{
	socklen_t addrlen = sizeof(_clientAdr);
	int new_socket = accept(_serverFd, (struct sockaddr *)&_clientAdr, &addrlen);
	if (new_socket == -1)
	{
		throw std::runtime_error("Accept failed");
		exit(EXIT_FAILURE);
	}
	Clients *newClient = new Clients();
	newClient->setSocket(new_socket);
	newClient->setStatus(Clients::USERNAME); //modif
	_clients[new_socket] = newClient;
	return new_socket;
}

//fonction qui gere toutes les entrees de l'utilisateur 
int Server::handleClientMessage(int client_socket, Clients::status status)
{
	char buffer[BUFFER_SIZE];
	ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
	if (valread <= 0)
	{
		std::istringstream line;
		quit(_clients[client_socket], line);
		return (1);
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
				if (pass(client, lineStream) == false)
				{
					std::istringstream line;
					quit(client, line);
					return (1);
				}
			}
			else if (command == "NICK" && client->getClientPwd() == _pwd)
			{
				if (nick(client, lineStream) == 1)
				{
					std::istringstream line;
					quit(client, line);
					return (1);
				}
			}
			else if (command == "USER" && client->getClientPwd() == _pwd)
				user(client, lineStream);
		}
		else if (status == Clients::COMPLETED)
		{
			if (command == "JOIN")
				join(client, lineStream);
			else if (command == "PRIVMSG")
				msg(client, lineStream, buffer);
			else if (command == "TOPIC")
				topic(client, lineStream);
			else if (command == "PART")
				part(client, lineStream);
			else if (command == "MODE")
				mode(client, lineStream);
			else if (command == "INVITE")
				invite(client, lineStream);
			else if (command == "QUIT")
			{
				quit(client, lineStream);
				return (1);
			}
			else if (command == "KICK")
				kick(client, lineStream);
			else if (command == "PING")
			{
				std::string token;
				lineStream >> token;
				pong(client, token);
			}
			return (0);
		}
	}
	return (0);
}
