/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 17:57:09 by scarpent         ###   ########.fr       */
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
			else if (command == "MSG")
				msg(client, lineStream, client_socket, _clients);
			else if (command == "PART")
				part(client, lineStream, client_socket);
			else if (command == "TOPIC")
				topic(client, lineStream, client_socket);
			else if (valread == 0)
			{
				close(client_socket);
				_clients.erase(client_socket);
				std::cout << "Client disconnected" << std::endl;
				return;
			}
			//else
			//{
				// //permet de specifier le nickname de la personne envoyant le message
				// std::string new_buf = ":" + client.get_Nickname() + " " + buffer; 
				// std::cout << "Received message: " << buffer << std::endl;
				// // Envoyer le message à tous les autres clients dans le même canal
				// std::string clientChannel = currIt;
				// for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
				// {
				// 	if (it->second.get_Channel() == clientChannel && it->first != client_socket)
				// 		send(it->first, new_buf.c_str(), new_buf.length(), 0);
				// }
			//}
		}
	}
}


void Server::topic(Clients &client, std::istringstream &lineStream, int client_socket)
{
    std::string channelName;
    std::string newTopic;

    // Read the channel name
    if (lineStream >> channelName)
    {
        // Read the new topic
        if (std::getline(lineStream, newTopic))
        {
            if (!newTopic.empty() && newTopic[0] == ' ' && newTopic[1] == ':')
                newTopic.erase(0, 2);
        }

        // Check if the client is in the channel
        std::vector<Channel *> &connectedChannels = client.getCurrConnected();
        std::vector<Channel *>::iterator it = connectedChannels.end();
        for (std::vector<Channel *>::iterator iter = connectedChannels.begin(); iter != connectedChannels.end(); ++iter)
        {
            if ((*iter)->getChanName() == channelName)
            {
                it = iter;
                break;
            }
        }

        if (it != connectedChannels.end())
        {
            // Notify all members of the channel about the topic change
            std::string fullTopicMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU TOPIC " + channelName + " " + newTopic + "\n";
            for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin();
                 connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
            {
                send(connIt->second->get_Socket(), fullTopicMessage.c_str(), fullTopicMessage.length(), 0);
            }
        }
        else
        {
            std::string errMsg = "You're not on that channel\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        }
    }
    else
    {
        std::string errMsg = "TOPIC command error. Use : TOPIC <channelName> [new_topic]\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
    }
}

void	Server::part(Clients &client, std::istringstream &lineStream, int client_socket)
{
	std::string channelName;
	std::string partMessage;
	
	if (lineStream >> channelName)
	{
		// Read optional part message
		if (std::getline(lineStream, partMessage))
		{
			if (!partMessage.empty() && partMessage[0] == ' ' && partMessage[1] == ':')
				partMessage.erase(0, 2);
		}

		// Check if the client is in the channel
		std::vector<Channel *> &connectedChannels = client.getCurrConnected();
		std::vector<Channel *>::iterator it = connectedChannels.end();
		for (std::vector<Channel *>::iterator iter = connectedChannels.begin(); iter != connectedChannels.end(); ++iter)
		{
			if ((*iter)->getChanName() == channelName)
			{
				it = iter;
				break;
			}
		}

		if (it != connectedChannels.end())
		{
			// Inform other members of the channel
			std::string fullPartMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU PART " + channelName;
			if (!partMessage.empty())
				fullPartMessage += " :" + partMessage;
			fullPartMessage += "\n";
			
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
			{
				send(connIt->second->get_Socket(), fullPartMessage.c_str(), fullPartMessage.length(), 0);
			}

			// Remove the client from the channel
			_Channel[channelName].getConnUsers().erase(client.get_Nickname());
			connectedChannels.erase(it);
		}
		else
		{
			std::string errMsg = "You're not in that channel\n";
			send(client_socket, errMsg.c_str(), errMsg.length(), 0);
		}
	}
	else
	{
		std::string errMsg = "PART command error. Use : PART <channelName> [message]\n";
		send(client_socket, errMsg.c_str(), errMsg.length(), 0);
	}
}

void Server::msg(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> _clients)
{
	// Récupérer le nom du destinataire et le contenu du message
	std::string dest;
	std::string msg;
	if (lineStream >> dest && std::getline(lineStream, msg))
	{
		if (dest.find("#") < dest.size())
		{
			std::cout << "msg: " << msg << std::endl;
			std::string sent_msg;
			for (std::vector<Channel *>::iterator currIt = client.getCurrConnected().begin(); currIt != client.getCurrConnected().end(); ++currIt)
			{
				if (dest == (*currIt)->getChanName())
				{
					sent_msg = ":" + client.get_Nickname() + " " + msg;
				}
				std::string clientChannel = (*currIt)->getChanName();
				for (std::map<std::string, Clients *>::iterator it = (*currIt)->getConnUsers().begin(); it != (*currIt)->getConnUsers().end(); ++it)
				{
					send(it->second->get_Socket(), sent_msg.c_str(), sent_msg.length(), 0);
				}
			}
		}
		// Supprimer les espaces en trop
		if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
			msg.erase(0, 2);
		// Recherche du destinataire dans la liste des clients
		bool found = false;
		int rSocket = -1;
		for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (it->second.get_Nickname() == dest)
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
			std::string errMsg = "User '" + dest + "' doesn't exist.\n";
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

void	Server::join(Clients &client, std::istringstream &lineStream, int client_socket)
{
	std::string channelName;
	if (lineStream >> channelName)
	{
		// Find or create the channel
		if (_Channel.find(channelName) == _Channel.end())
		{
			Channel new_channel(channelName);
			_Channel[channelName] = new_channel;
			std::cout << "New Channel created: " << channelName << std::endl;
		}

		// Add the client to the channel's connected users
		_Channel[channelName].getConnUsers()[client.get_Nickname()] = &client;
		client.getCurrConnected().push_back(&_Channel[channelName]);

		// Notify all clients in the channel
		std::string joinMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU JOIN " + channelName + "\n";
		for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
		{
			send(it->second->get_Socket(), joinMessage.c_str(), joinMessage.length(), 0);
		}

		// Send the list of users in the channel to the new client
		std::string namesMessage = ":I.R.SIUSIU 353 " + client.get_Nickname() + " = " + channelName + " :";
		for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
		{
			namesMessage += it->first + " ";
		}
		namesMessage += "\n";
		send(client_socket, namesMessage.c_str(), namesMessage.length(), 0);

		// End of the list
		std::string endNamesMessage = ":I.R.SIUSIU 366 " + client.get_Nickname() + " " + channelName + " :End of /NAMES list.\n";
		send(client_socket, endNamesMessage.c_str(), endNamesMessage.length(), 0);
	}
	else
	{
		std::string errorMessage = "Usage: JOIN <channelName>\n";
		send(client_socket, errorMessage.c_str(), errorMessage.length(), 0);
	}
}

// void Server::join(Clients &client, std::istringstream &lineStream, int client_socket)
// {
//     std::string channelName;
//     if (lineStream >> channelName)
//     {
//         client.set_Channel(channelName);
//         // Vérifie si le channelName existe sinon le crée
//         if (_Channel.find(channelName) == _Channel.end())
//         {
//             Channel new_channel(channelName);
//             _Channel[channelName] = new_channel;
//             std::cout << "New Channel created: " << channelName << std::endl;
//         }
//         // Ajoute le client au canal
//         std::map<std::string, Clients*>& connUsers = _Channel[channelName].getConnUsers();
//         connUsers[client.get_Nickname()] = &client;
//         // Informer tous les clients du canal qu'un nouveau client a rejoint
//         std::string joinMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU JOIN " + channelName + "\n";
//         std::cout << "CLIENTS: " << client.get_Nickname() << std::endl;
//         for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
//         {
//             send(it->second->get_Socket(), joinMessage.c_str(), joinMessage.length(), 0);
//         }
//         // Envoyer la liste des utilisateurs du canal au nouveau client
//         std::string namesMessage = ":I.R.SIUSIU 353 " + client.get_Nickname() + " = " + channelName + " :";
//         for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
//         {
//             namesMessage += it->second->get_Nickname() + " ";
//         }
//         namesMessage += "\n";
//         send(client_socket, namesMessage.c_str(), namesMessage.length(), 0);
//         // Fin de la liste des noms
//         std::string endNamesMessage = ":I.R.SIUSIU 366 " + client.get_Nickname() + " " + channelName + " :End of /NAMES list.\n";
//         send(client_socket, endNamesMessage.c_str(), endNamesMessage.length(), 0);
//     }
//     else
//     {
//         std::string errorMessage = "Usage: JOIN <channelName>\n";
//         send(client_socket, errorMessage.c_str(), errorMessage.length(), 0);
//     }
// }

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
