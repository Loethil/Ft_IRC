/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/21 15:53:03 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

// const char *Server::bindException::what() const throw()
// {
//     return "Bind failed";
// }

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
		throw std::runtime_error("Listen failed");
		close(_server_fd);
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
					handleClientMessage(client_socket, _clients[client_socket].get_Status());
				}
			}
		}
	}
}

//fonction permettant d'accepter de nouveau clents et de lui donner ses bons parametres
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
	Clients newClient;
	newClient.set_Socket(new_socket);
	newClient.set_Status(Clients::USERNAME); //modif
	_clients[new_socket] = newClient;

	std::cout << "New connection accepted: " << new_socket << std::endl;
	//send(new_socket, "Please enter the password: ", 28, 0);
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

    // std::string msg(buffer); decommenter car inutile ?
    // std::istringstream iss(msg); decommenter car inutile ? 
    // std::string line; decommenter car inutile ?
    Clients &client = _clients[client_socket];
    client.partialData.append(buffer, valread);
    size_t pos;

    while ((pos = client.partialData.find('\n')) != std::string::npos)
    {
        std::string line = client.partialData.substr(0, pos);
        client.partialData.erase(0, pos + 1);
        if (!line.empty() && line[line.size() - 1] == '\r')  // Remove trailing '\r' if present
            line.erase(line.size() - 1);
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
    //-----------------------petit pb au niveau du blase de qui envoit le msg je crois----------------------------------------------
            else if (command == "MSG")
            {
                // Récupérer le nom du destinataire et le contenu du message
                std::string blase;
                std::string msg;
                if (lineStream >> blase && std::getline(lineStream, msg))
                {
                    // Supprimer les espaces en trop
                    if (!msg.empty() && msg[0] == ' ')
                        msg.erase(0, 1);
                    // Recherche du destinataire dans la liste des clients
                    bool found = false;
                    int rSocket = -1;
                    for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                    {
                        if (it->second.get_Nickname() == blase)
                        {
                            found = true;
                            rSocket = it->first;
                            break;
                        }
                    }
                    if (found)
                    {
                        // Envoyer le message au destinataire
                        std::string fullMsg = "MSG " + client.get_Nickname() + ": " + msg + "\n";
                        ssize_t msgSize = fullMsg.length();
                        send(rSocket, fullMsg.c_str(), msgSize, 0);
                    }
                    else
                    {
                        // Le destinataire n'a pas été trouvé, envoyer un message d'erreur au client
                        std::string errMsg = "User '" + blase + "' doesn't exist.\n";
                        ssize_t msgSize = errMsg.length();
                        send(client_socket, errMsg.c_str(), msgSize, 0);
                    }
                }
                else
                {
                    // La commande MSG est mal formée, envoyer un message d'erreur au client
                    std::string errMsg = "MSG command error. Use : MSG <destinataire> <message>\n";
                    ssize_t msgSize = errMsg.length();
                    send(client_socket, errMsg.c_str(), msgSize, 0);
                }
            }
            //-------------------------------------------------------------------------
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
                // Envoyer le message à tous les autres clients dans le même canal
                std::string clientChannel = client.get_Channel();
                for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                {
                    if (it->second.get_Channel() == clientChannel && it->first != client_socket)
                        send(it->first, buffer, valread, 0);
                }
                // Envoyer le message au client d'origine mais pas besoin en vrai
                //send(client_socket, buffer, valread, 0);
            }
        }
    }
}

 //fonction permettant de rejoindre un channel
void	Server::join(Clients &client, std::istringstream &lineStream, int client_socket)
{
        std::string channel;
		lineStream >> channel;

		client.set_Channel(channel);
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

//fonction permettant de verifier le mot de passe
bool	Server::pass(Clients &client, std::istringstream &lineStream, int client_socket)
{
    std::string pass;
	lineStream >> pass;
	if (pass == _pwd)
	{
		client.set_Status(Clients::USERNAME);
		return (true);
	}
	else
	{
		send(client_socket, "Invalid password, try again...\n", 32, 0);
		close(client_socket);
		_clients.erase(client_socket);
		return (false);
	}
}

//fonction permettant de set le nickname de l'utilisateur
void	Server::nick(Clients &client, std::istringstream &lineStream)
{
	std::string nick;
	lineStream >> nick;
	client.set_Nickname(nick);
	std::cout << "Nickname set to: " << client.get_Nickname() << std::endl;
}

//fonction permettant de set l'Username, le Realname et le status de l'utilisateur
void	Server::user(Clients &client, std::istringstream &lineStream, int client_socket)
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

//fonction permettant d'afficher les messages de bienvenu a l'utilisateur
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

