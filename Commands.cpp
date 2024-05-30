#include "Server.hpp"

void	Server::topic(Clients *client, std::istringstream &lineStream, int client_socket)
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
		else
		{
			if (_Channel[channelName].getTopic().size() > 0)
			{
				std::string fullTopicMessage = ":I.R.SIUSIU 332 " + client->getNickname() + " " + channelName + " :" + _Channel[channelName].getTopic() + "\n";
				send(client->getSocket(), fullTopicMessage.c_str(), _Channel[channelName].getTopic().length(), 0);
			}
			return ;
		}
		// Check if the client is in the channel
		std::vector<Channel *> &connectedChannels = client->getCurrConnected();
		std::vector<Channel *>::iterator it = connectedChannels.end();
		for (std::vector<Channel *>::iterator iter = connectedChannels.begin(); iter != connectedChannels.end(); ++iter)
		{
			if ((*iter)->getChanName() == channelName)
			{
				it = iter;
				break ;
			}
		}
		if (it != connectedChannels.end())
		{
			// Stocks the topic in the channel instance
			(*it)->setTopic(newTopic);
			// Notify all members of the channel about the topic change
			std::string fullTopicMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU TOPIC " + channelName + " " + newTopic + "\n";
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
				send(connIt->second->getSocket(), fullTopicMessage.c_str(), fullTopicMessage.length(), 0);
		}
		else
		{
			std::string send_msg = ":I.R.SIUSIU 442 " + client->getNickname() + channelName + " :" RED "You are not connected to that channel.\n" RESET;
			send(client->getSocket(), send_msg.c_str(), send_msg.size(), 0);
		}
	}
}

void Server::msg(Clients *client, std::istringstream &lineStream, char *buffer)
{
    std::string dest;
    std::string msg = buffer;

    std::cerr << "Msg command received from client: " << client->getNickname() << std::endl;

    if (lineStream >> dest)
    {
        if (dest.find("#") < dest.size())
        {
            if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
                msg.erase(0, 2);
            std::string sent_msg;
			std::vector<Channel *>::iterator currIt;
            for (currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
            {
                if (dest != (*currIt)->getChanName())
					continue ;
				sent_msg = ":" + client->getNickname() + " " + msg;
				for (std::map<std::string, Clients *>::iterator it = (*currIt)->getConnUsers().begin(); it != (*currIt)->getConnUsers().end(); ++it)
				{
					if (it->second->getSocket() != client->getSocket())
						send(it->second->getSocket(), sent_msg.c_str(), sent_msg.length(), 0);
				}
				return ;
            }
			if (currIt == client->getCurrConnected().end())
			{
				std::string send_msg = ":I.R.SIUSIU 442 " + client->getNickname() + dest + " :" RED "You aren't connected to that channel.\n" RESET;
				send(client->getSocket(), send_msg.c_str(), send_msg.size(), 0);
			}
        }
        else
        {
            if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
                msg.erase(0, 2);
            std::string send_msg;
            for (std::map<int, Clients*>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter)
            {
                if (iter->second->getNickname() == dest)
                {
                    send_msg = ":" + client->getNickname() + " " + msg;
                    send(iter->second->getSocket(), send_msg.c_str(), send_msg.size(), 0);
                    return ;
                }
            }
            std::string errMsg = ":I.R.SIUSIU 401 " + client->getNickname() + " " + dest + " :" RED "No such user\n" RESET;
            send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
        }
    }
}

void Server::invite(Clients *client, std::istringstream &lineStream, int client_socket)
{
    std::string nickname;
    std::string channelName;

	// Check de l'operator status
    // Lire le nom de l'utilisateur à inviter et le canal
    if (lineStream >> nickname >> channelName)
    {
		if (_Channel[channelName].getOpStatus(client->getNickname()) == false)
		{
			std::string msg = ":I.R.SIUSIU PRIVMSG " + channelName + " :" RED "You're not an operator" RESET "\n";
			send(client->getSocket(), msg.c_str(), msg.size(), 0);
			return ;
		}
        std::map<std::string, Channel>::iterator chanIt = _Channel.find(channelName);
        if (chanIt != _Channel.end())
        {
            Channel &channel = chanIt->second;
            
            // Vérifier si l'utilisateur qui envoie l'invitation est dans le canal
            Clients* invitedClient = NULL;
            for(std::map<int, Clients*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (it->second->getNickname() == nickname)
                {
                    invitedClient = it->second;
                    break;
                }
            }
            if (invitedClient != NULL)
            {
                // Ajouter l'utilisateur à la liste des invitations du canal
                channel.addInvite(nickname);

                // Envoyer un message à l'utilisateur invité
                std::string inviteMessage = ":I.R.SIUSIU 341 " + client->getNickname() + " " + nickname + " " + channelName + "\n";
                send(invitedClient->getSocket(), inviteMessage.c_str(), inviteMessage.length(), 0);

                // Informer l'utilisateur qui envoie l'invitation du succès
                std::string successMsg = "You invited " + nickname + " to " + channelName + "\n";
                send(client_socket, successMsg.c_str(), successMsg.length(), 0);
            }
            else
            {
                // L'utilisateur à inviter n'existe pas
                std::string errMsg = "User " + nickname + " does not exist\n";
                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            }
        }
        else
        {
            // Le canal n'existe pas
            std::string errMsg = "No such channel: " + channelName + "\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        }
    }
}

void	Server::joinChannel(Clients *client, std::string channelName)
{
	// Add the client to the channel's connected users
	_Channel[channelName].getConnUsers()[client->getNickname()] = client;
	client->getCurrConnected().push_back(&_Channel[channelName]);
	if (_Channel[channelName].getConnUsers().size() == 1)
		_Channel[channelName].setOperator(client, true, client->getNickname());

	// Notify all clients in the channel
	std::string joinMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU JOIN " + channelName + "\n";
	for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
		send(it->second->getSocket(), joinMessage.c_str(), joinMessage.length(), 0);

	// Send the current topic to the new client
	if (!_Channel[channelName].getTopic().empty())
	{
		std::string topicMessage = ":I.R.SIUSIU 332 " + client->getNickname() + " " + channelName + " :" + _Channel[channelName].getTopic() + "\n";
		send(client->getSocket(), topicMessage.c_str(), topicMessage.length(), 0);
	}

	// Send the list of users in the channel to the new client
	std::string namesMessage = ":I.R.SIUSIU 353 " + client->getNickname() + " = " + channelName + " :";
	for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
		namesMessage += it->first + " ";
	namesMessage += "\n";
	send(client->getSocket(), namesMessage.c_str(), namesMessage.length(), 0);

	// End of the list
	std::string endNamesMessage = ":I.R.SIUSIU 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list.\n";
	send(client->getSocket(), endNamesMessage.c_str(), endNamesMessage.length(), 0);
}

void	Server::join(Clients *client, std::istringstream &lineStream, int client_socket)
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

		Channel& channel = _Channel[channelName];

		if (channel.getInvite() == false)
		{
			if (channel.getPwd().empty())
			{
				joinChannel(client, channelName);
			}
			else
			{
				std::string pwd;
				if (lineStream >> pwd)
				{
					if (pwd.compare(channel.getPwd()) == 0)
						joinChannel(client, channelName);
					else
					{
						std::string errMsg = ":I.R.SIUSIU 475 " + client->getNickname() + " " + channelName + " :Incorrect channel key\n";
						send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
					}
				}
				else
				{
					std::string errMsg = ":I.R.SIUSIU 475 " + client->getNickname() + " " + channelName + " :No channel key provided\n";
					send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
				}
			}
		}
		else 
		{
			std::vector<std::string>::iterator invIt;
			for(invIt = channel.getInvitedUsers().begin(); invIt != channel.getInvitedUsers().end(); ++invIt)
			{
				if ((*invIt).compare(client->getNickname()) == 0)
				{
					if (channel.getPwd().empty())
						joinChannel(client, channelName);
					else
					{
						std::string pwd;
						if (lineStream >> pwd)
						{
							if (pwd.compare(channel.getPwd()) == 0)
								joinChannel(client, channelName);
							else
							{
								std::string errMsg = ":I.R.SIUSIU 475 " + client->getNickname() + " " + channelName + " :Incorrect channel key (+k)\n";
								send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
							}
						}
						else
						{
							std::string errMsg = ":I.R.SIUSIU 475 " + client->getNickname() + " " + channelName + " :No channel key provided (+k)\n";
							send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
						}
					}
					return;
				}
			}
			if (invIt == channel.getInvitedUsers().end())
			{
				std::string errMsg = ":I.R.SIUSIU 473 " + client->getNickname() + " " + channelName + " :Cannot join channel (+i)\n";
				send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
			}
		}
	}
}

void Server::part(Clients *client, std::istringstream &lineStream)
{
    std::string channelName;
    std::string partMessage;
    
    std::cerr << "Part command received from client: " << client->getNickname() << std::endl;

    if (lineStream >> channelName)
    {
        std::cerr << "Channel to part: " << channelName << std::endl;
        if (std::getline(lineStream, partMessage))
        {
            if (!partMessage.empty() && partMessage[0] == ' ' && partMessage[1] == ':')
                partMessage.erase(0, 2);
        }

        std::vector<Channel *> &connectedChannels = client->getCurrConnected();
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
            std::cerr << "Client found in channel: " << channelName << std::endl;
            std::string partMessageFull;
            _Channel[channelName].getConnUsers().erase(client->getNickname());
            connectedChannels.erase(it);

            if (!partMessage.empty())
                partMessageFull = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :" + partMessage + "\n";
            else
                partMessageFull = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :\n";
            for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
            {
                send(connIt->second->getSocket(), partMessageFull.c_str(), partMessageFull.length(), 0);
            }
			if (_Channel[channelName].getConnUsers().empty())
                _Channel.erase(channelName);

            send(client->getSocket(), partMessageFull.c_str(), partMessageFull.length(), 0);
        }
        else
        {
            std::cerr << "Client not found in channel: " << channelName << std::endl;
            std::string errMsg = "You're not on that channel\n";
            send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
        }
    }
}

//c'est le part pour le /quit celui la
void Server::part(Clients *client, std::string channelName)
{
    std::string partMessage;
    
    std::cerr << "Part command received from client: " << client->getNickname() << std::endl;
	std::cerr << "Channel to part: " << channelName << std::endl;

	std::vector<Channel *> &connectedChannels = client->getCurrConnected();
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
		std::cerr << "Client found in channel: " << channelName << std::endl;
		_Channel[channelName].getConnUsers().erase(client->getNickname());
		connectedChannels.erase(it);

		partMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :\n";
		if (_Channel[channelName].getConnUsers().empty())
			_Channel.erase(channelName);

		send(client->getSocket(), partMessage.c_str(), partMessage.length(), 0);
	}
	else
	{
		std::cerr << "Client not found in channel: " << channelName << std::endl;
		std::string errMsg = "You're not on that channel\n";
		send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
	}
}

void Server::quit(Clients *client, std::istringstream &lineStream)
{
    std::string quitMsg;

    // Lire le message de déconnexion s'il est fourni
    if (lineStream >> quitMsg)
	{
        if (std::getline(lineStream, quitMsg))
		{
            if (!quitMsg.empty() && quitMsg[0] == ' ' && quitMsg[1] == ':')
                quitMsg.erase(0, 2);
        }
    }

    // Construire le message complet de déconnexion
    std::string fullQuitMsg;
    if (!quitMsg.empty())
        fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :" + quitMsg + "\n";
	else
        fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :\n";
    // Utiliser un vector pour suivre les sockets déjà notifiées
    std::vector<int> notifiedSockets;

    // Envoyer le message de déconnexion à tous les clients connectés aux mêmes canaux
    std::vector<Channel*>::iterator chIt;
    for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
	{
        Channel* channel = *chIt;
        std::map<std::string, Clients*>::iterator chanClientIt;
        for (chanClientIt = channel->getConnUsers().begin(); chanClientIt != channel->getConnUsers().end(); ++chanClientIt)
		{
            //Clients* chanClientIt = *chanClientIt;
            if (std::find(notifiedSockets.begin(), notifiedSockets.end(), chanClientIt->second->getSocket()) == notifiedSockets.end())
			{
                send(chanClientIt->second->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
                notifiedSockets.push_back(chanClientIt->second->getSocket());
            }
        }
    }

    // Supprimer le client de tous les canaux
    for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
        part(client, (*chIt)->getChanName());
    // Supprimer le client de la liste des clients du serveur
    int clientSocket = client->getSocket();
    _clients.erase(clientSocket);
    // Fermer la socket du client
    close(clientSocket);
}


void	Server::mode(Clients *client, std::istringstream &lineStream)
{
	std::string chan;
	std::string mode;
	std::vector<Channel *>::iterator currIt;

	lineStream >> chan >> mode;
	std::cout << "chan : " << chan << std::endl;
	std::cout << "mode : " << mode << std::endl;
	// std::cout << "name : " << name << std::endl;
	for (currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
	{
		if (chan == (*currIt)->getChanName())
		{
			if ((*currIt)->getConnUsers().find(client->getNickname()) == (*currIt)->getConnUsers().end())
			{
				std::string errMsg = ":I.R.SIUSIU 401 " + client->getNickname() + " " + chan + " :" RED "No such user\n" RESET;
				send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
				return ;
			}
			break ;
		}
	}
	if (currIt == client->getCurrConnected().end() && mode != "+o")
	{
		if (chan == client->getNickname())
			return ;
		std::string errMsg = "This channel does not exist\n";
		send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
		return ;
	}
	if (mode[0] == '+' || mode[0] == '-')
	{
		bool type = (mode[0] == '+') ? true : false;
		for (size_t i = 1; i < mode.size(); ++i)
		{
			char modeChar = mode[i];
			switch (modeChar)
			{
			case 'i':
				(*currIt)->setInvite(type);
				break;
			case 't':
				(*currIt)->setTopicMode(type);
				break;
			case 'k':
				if (type)
				{
					std::string pwd;
					if (lineStream >> pwd)
					{
						if (!pwd.empty())
						{
							(*currIt)->setPwd(pwd);
							std::cout << BLUE << (*currIt)->getPwd() << RESET << std::endl;
						}
					}
					else
					{
						std::string errMsg = "No password specified for +k mode\n";
						send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
					}
				}
				else
					(*currIt)->setPwd("");
				break;
			case 'o':
				{
					std::string newOp;
					if (lineStream >> newOp)
					{
						for (std::vector<std::string>::iterator opIt = _Channel[chan].getOperator().begin(); opIt != _Channel[chan].getOperator().end(); ++opIt)
						{
							if ((*opIt).compare(client->getNickname()) == 0)
							{
								(*currIt)->setOperator(client, type, newOp);
								return ;
							}
						}
						std::string errMsg = ":I.R.SIUSIU PRIVMSG " + chan + " :You don't have the privilege to give operator rights\n";;
						send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
					}
					else
					{
						std::string errMsg = (type ? "No operator specified for +o mode\n" : "No operator specified for -o mode\n");
						send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
					}
				}
				break;
			// case 'l':
			// 	if (type)
			// 	{
			// 		int userLimit;
			// 		lineStream >> userLimit;
			// 		if (lineStream.fail())
			// 		{
			// 			std::string errMsg = "No user limit specified for +l mode\n";
			// 			send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
			// 			lineStream.clear();
			// 		}
			// 		else
			// 			(*currIt)->setUserLimit(userLimit);
			// 	}
			// 	else
			// 		(*currIt)->setUserLimit(0);
			// 	break;
			default:
				{
					std::string errMsg = "This option is not handled, see IRC subject\n";
					send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
				}
				break;
			}
		}
	}
}

//fonction permettant de verifier le mot de passe
bool	Server::pass(Clients *client, std::istringstream &lineStream, int client_socket)
{
	std::string pass;
	lineStream >> pass;
	if (pass == _pwd)
	{
		client->setStatus(Clients::USERNAME);
		return (true);
	}
	else
	{
		std::string errormsg = ":I.R.SIUSIU 300 " + client->getNickname() + " :Invalid password, try again...\n";
		send(client_socket, errormsg.c_str(), errormsg.size(), 0);
		close(client_socket);
		delete _clients[client_socket];
		_clients.erase(client_socket);
		return (false);
	}
}

//fonction permettant de set le nickname de l'utilisateur
void	Server::nick(Clients *client, std::istringstream &lineStream)
{
	std::string nick;
	lineStream >> nick;
	client->setNickname(nick);
	std::cout << "Nickname set to: " << client->getNickname() << std::endl;
}

//fonction permettant de set l'Username, le Realname et le status de l'utilisateur
void	Server::user(Clients *client, std::istringstream &lineStream, int client_socket)
{
	std::string user, mode, unused, realname;
	lineStream >> user >> mode >> unused;
	std::getline(lineStream, realname);
	if (!realname.empty() && realname[0] == ' ' && realname[1] == ':')
		realname.erase(0, 2);
	client->setUsername(user);
	client->setRealname(realname);
	client->setStatus(Clients::COMPLETED);
	std::cout << "Username set to: " << client->getUsername() << std::endl;
	std::cout << "Realname set to: " << client->getRealname() << std::endl;
	sendWelcomeMessages(client_socket, client);
}
