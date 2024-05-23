/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:23:59 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 16:31:00 by llaigle          ###   ########.fr       */
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
				part(client, lineStream, client_socket, _clients);
			else if (command == "TOPIC")
				topic(client, lineStream, client_socket, _clients);
            else if (command == "INVITE")
                invite(client, lineStream, client_socket, _clients);
            else if (valread == 0)
            {
                close(client_socket);
                _clients.erase(client_socket);
                std::cout << "Client disconnected" << std::endl;
                return;
            }
            else
            {
				//permet de specifier le nickname de la personne envoyant le message
				std::string new_buf = ":" + client.get_Nickname() + " " + buffer; 
                std::cout << "Received message: " << buffer << std::endl;
                // Envoyer le message à tous les autres clients dans le même canal
                std::string clientChannel = client.get_Channel();
                for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                {
                    if (it->second.get_Channel() == clientChannel && it->first != client_socket)
                        send(it->first, new_buf.c_str(), new_buf.length(), 0);
                }
            }
        }
    }
}

///////////////////////////////a revoir celle la elle est bancale un peu
/// c'est giga long mais ca peut deja etre separe en deux avec channel et client split
void Server::mode(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> &_clients, std::map<std::string, Channel> &channels)
{
    std::string target;
    std::string modes;
    std::string modeParams;

    // Lire le nom de la cible (canal ou utilisateur)
    if (lineStream >> target)
    {
        // Vérifier si la cible est un canal ou un utilisateur
        if (target[0] == '#') // Cible est un canal
        {
            std::map<std::string, Channel>::iterator chanIt = channels.find(target);
            if (chanIt != channels.end())
            {
                Channel &channel = chanIt->second;
                // Lire les modes et les paramètres de mode
                if (lineStream >> modes)
                {
                    std::getline(lineStream, modeParams);
                    if (!modeParams.empty() && modeParams[0] == ' ')
                        modeParams.erase(0, 1);

                    // Appliquer les changements de mode
                    std::string result;
                    bool adding = true;
                    for (size_t i = 0; i < modes.size(); ++i)
                    {
                        char mode = modes[i];
                        if (mode == '+')
                            adding = true;
                        else if (mode == '-')
                            adding = false;
                        else
                        {
                            if (channel.changeMode(mode, adding))
                                result += (adding ? "+" : "-") + std::string(1, mode);
                            else
                            {
                                // Mode non reconnu ou non applicable, envoyer un message d'erreur au client
                                std::string errMsg = "Unknown mode: " + std::string(1, mode) + "\n";
                                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
                            }
                        }
                    }

                    // Informer les membres du canal des changements de mode
                    if (!result.empty())
                    {
                        std::string fullModeMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU MODE " + target + " " + result;
                        if (!modeParams.empty())
                            fullModeMessage += " " + modeParams;
                        fullModeMessage += "\n";

                        for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                        {
                            if (it->second.get_Channel() == target)
                                send(it->first, fullModeMessage.c_str(), fullModeMessage.length(), 0);
                        }
                    }
                }
                else
                {
                    // Afficher les modes actuels du canal
                    std::string currentModes = channel.getModes();
                    std::string modeMessage = "Current modes for " + target + ": " + currentModes + "\n";
                    send(client_socket, modeMessage.c_str(), modeMessage.length(), 0);
                }
            }
            else
            {
                // Le canal n'existe pas, envoyer un message d'erreur au client
                std::string errMsg = "No such channel: " + target + "\n";
                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            }
        }
        else // Cible est un utilisateur
        {
            // Implémentation des modes utilisateur (similaire à celle des canaux, mais appliquée à l'utilisateur)
            std::map<int, Clients>::iterator userIt;
            bool found = false;
            for (userIt = _clients.begin(); userIt != _clients.end(); ++userIt)
            {
                if (userIt->second.get_Nickname() == target)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                Clients &targetUser = userIt->second;
                if (lineStream >> modes)
                {
                    // Appliquer les changements de mode pour l'utilisateur
                    std::string result;
                    bool adding = true;
                    for (size_t i = 0; i < modes.size(); ++i)
                    {
                        char mode = modes[i];
                        if (mode == '+')
                            adding = true;
                        else if (mode == '-')
                            adding = false;
                        else
                        {
                            if (targetUser.changeMode(mode, adding))
                                result += (adding ? "+" : "-") + std::string(1, mode);
                            else
                            {
                                // Mode non reconnu ou non applicable, envoyer un message d'erreur au client
                                std::string errMsg = "Unknown mode: " + std::string(1, mode) + "\n";
                                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
                            }
                        }
                    }

                    // Informer l'utilisateur des changements de mode
                    if (!result.empty())
                    {
                        std::string fullModeMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU MODE " + target + " " + result + "\n";
                        send(userIt->first, fullModeMessage.c_str(), fullModeMessage.length(), 0);
                    }
                }
                else
                {
                    // Afficher les modes actuels de l'utilisateur
                    std::string currentModes = targetUser.getModes();
                    std::string modeMessage = "Current modes for " + target + ": " + currentModes + "\n";
                    send(client_socket, modeMessage.c_str(), modeMessage.length(), 0);
                }
            }
            else
            {
                // L'utilisateur n'existe pas, envoyer un message d'erreur au client
                std::string errMsg = "No such user: " + target + "\n";
                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            }
        }
    }
    else
    {
        // La commande MODE est mal formée, envoyer un message d'erreur au client
        std::string errMsg = "MODE command error. Use: MODE <target> [modes]\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
    }
}



/////////////////////////fonction qui gere les invitations
void Server::invite(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> &_clients)
{
    std::string inv;
    std::string channel;
    
    // Lire le nom de l'utilisateur invité et le nom du canal
    if (lineStream >> inv >> channel)
    {
        // Vérifier si le client est dans le canal
        if (client.get_Channel() == channel)
        {
            // Rechercher l'utilisateur invité dans la liste des clients
            std::map<int, Clients>::iterator inv_it;
            bool found = false;
            for (inv_it = _clients.begin(); inv_it != _clients.end(); ++inv_it)
            {
                if (inv_it->second.get_Nickname() == inv)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                // Informer l'utilisateur invité de l'invitation
                std::string inviteMsg = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU INVITE " + inv + " :" + channel + "\n";
                send(inv_it->first, inviteMsg.c_str(), inviteMsg.length(), 0);

                // Informer l'utilisateur qui invite que l'invitation a été envoyée
                std::string confirmMsg = "You invited " + inv + " to join " + channel + "\n";
                send(client_socket, confirmMsg.c_str(), confirmMsg.length(), 0);
            }
            else
            {
                // Envoyer un message d'erreur si l'utilisateur invité n'existe pas
                std::string errMsg = "User '" + inv + "' doesn't exist.\n";
                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            }
        }
        else
        {
            // Envoyer un message d'erreur si le client n'est pas dans le canal
            std::string errMsg = "You're not on that channel\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        }
    }
    else
    {
        // La commande INVITE est mal formée, envoyer un message d'erreur au client
        std::string errMsg = "INVITE command error. Use : INVITE <nickname> <channel>\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
    }
}

////////////stocker le topic dans une map ?
void Server::topic(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> &_clients)
{
    std::string channel;
    std::string newTopic;
    
    // Lire le nom du canal
    if (lineStream >> channel)
    {
        // Lire le nouveau sujet du canal
        if (std::getline(lineStream, newTopic))
        {
            // Supprimer les espaces en trop
            if (!newTopic.empty() && newTopic[0] == ' ')
                newTopic.erase(0, 2);
        }
        // Vérifier si le client est dans le canal
        if (client.get_Channel() == channel)
        {
            // Informer les autres membres du canal du changement de sujet
            std::string fullTopicMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU TOPIC " + channel + " " + newTopic + "\n";
            for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (it->second.get_Channel() == channel)
                {
                    send(it->first, fullTopicMessage.c_str(), fullTopicMessage.length(), 0);
                }
            }
        }
        else
        {
            // Envoyer un message d'erreur si le client n'est pas dans le canal
            std::string errMsg = "You're not on that channel\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        }
    }
    else
    {
        // La commande TOPIC est mal formée, envoyer un message d'erreur au client
        std::string errMsg = "TOPIC command error. Use : TOPIC <channel> [new_topic]\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
    }
}


void Server::part(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> &_clients)
{
    std::string channel;
    std::string partMessage;
    
    // Lire le nom du canal
    if (lineStream >> channel)
    {
        // Lire le message d'adieu optionnel
        if (std::getline(lineStream, partMessage))
        {
            if (!partMessage.empty() && partMessage[0] == ' ')
                partMessage.erase(0, 2);
        }
        // Vérifier si le client est dans le canal
        if (client.get_Channel() == channel)
        {
            // Informer les autres membres du canal que le client a quitté
            std::string fullPartMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU PART " + channel;
            if (!partMessage.empty())
                fullPartMessage += " :" + partMessage; // Ajoute ":" seulement si partMessage n'est pas vide
            fullPartMessage += "\n";
            
            for (std::map<int, Clients>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (it->second.get_Channel() == channel)
                {
                    send(it->first, fullPartMessage.c_str(), fullPartMessage.length(), 0);
                }
            }
            // Retirer le client du canal
            client.set_Channel("");
        }
        else
        {
            // Envoyer un message d'erreur si le client n'est pas dans le canal
            std::string errMsg = "You're not on that channel\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        }
    }
    else
    {
        // La commande PART est mal formée, envoyer un message d'erreur au client
        std::string errMsg = "PART command error. Use : PART <channel> [message]\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
    }
}

void Server::msg(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> _clients)
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

// void Server::join(Clients &client, std::istringstream &lineStream, int client_socket)
// {
//     std::string channelName;
//     if (lineStream >> channelName)
//     {
//         client.set_Channel(channelName);
// 		//verifie si le channelName existe sinon le cree
// 		std::map<std::string, Channel>::iterator it = _Channel.find(channelName);
// 		if (it == _Channel.end()) //le channelName n'existe pas
// 		{
// 			Channel		new_channel(channelName);

// 			_Channel[channelName] = new_channel;
// 			std::cout << "New Channel created :" << _Channel[channelName].getChanName() << std::endl;
// 		}
// 		_Channel[channelName].get_connUsers()[client.get_Nickname()] = &client;
//         // Informer tous les clients du canal qu'un nouveau client a rejoint
//         std::string joinMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU JOIN " + channelName + "\n";
// 		std::cout <<  "CLIENTS : " << _Channel[channelName].get_connUsers()[client.get_Nickname()]->get_Nickname() << std::endl;
//      	// Utiliser une référence pour éviter la destruction de l'objet temporaire
//         std::map<std::string, Clients*>& connUsers = _Channel[channelName].get_connUsers();
//         for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
//         {
//             send(it->second->get_Socket(), joinMessage.c_str(), joinMessage.length(), 0);
//         }
//         // Envoyer la liste des utilisateurs du canal au nouveau client
//         std::string namesMessage = ":I.R.SIUSIU 353 " + client.get_Nickname() + " = " + channelName + " :";
//          for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
//         {
//             if (it->first == channelName)
//             {
//                 namesMessage += it->second->get_Nickname() + " ";
//             }
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

void Server::join(Clients &client, std::istringstream &lineStream, int client_socket)
{
    std::string channelName;
    if (lineStream >> channelName)
    {
        client.set_Channel(channelName);
        // Vérifie si le channelName existe sinon le crée
        if (_Channel.find(channelName) == _Channel.end())
        {
            Channel new_channel(channelName);
            _Channel[channelName] = new_channel;
            std::cout << "New Channel created: " << channelName << std::endl;
        }
        // Ajoute le client au canal
        std::map<std::string, Clients*>& connUsers = _Channel[channelName].get_connUsers();
        connUsers[client.get_Nickname()] = &client;
        // Informer tous les clients du canal qu'un nouveau client a rejoint
        std::string joinMessage = ":" + client.get_Nickname() + "!" + client.get_Username() + "@I.R.SIUSIU JOIN " + channelName + "\n";
        std::cout << "CLIENTS: " << client.get_Nickname() << std::endl;
        for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
        {
            send(it->second->get_Socket(), joinMessage.c_str(), joinMessage.length(), 0);
        }
        // Envoyer la liste des utilisateurs du canal au nouveau client
        std::string namesMessage = ":I.R.SIUSIU 353 " + client.get_Nickname() + " = " + channelName + " :";
        for (std::map<std::string, Clients*>::iterator it = connUsers.begin(); it != connUsers.end(); ++it)
        {
            namesMessage += it->second->get_Nickname() + " ";
        }
        namesMessage += "\n";
        send(client_socket, namesMessage.c_str(), namesMessage.length(), 0);
        // Fin de la liste des noms
        std::string endNamesMessage = ":I.R.SIUSIU 366 " + client.get_Nickname() + " " + channelName + " :End of /NAMES list.\n";
        send(client_socket, endNamesMessage.c_str(), endNamesMessage.length(), 0);
    }
    else
    {
        std::string errorMessage = "Usage: JOIN <channelName>\n";
        send(client_socket, errorMessage.c_str(), errorMessage.length(), 0);
    }
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
