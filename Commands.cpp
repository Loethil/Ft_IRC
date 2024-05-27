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
				//std::string fullTopicMessage = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU TOPIC " + channelName + " " + newTopic + "\n";
				std::string fullTopicMessage = ":I.R.SIUSIU 332 " + client->get_Nickname() + " " + channelName + " :" + _Channel[channelName].getTopic() + "\n";
				send(client->get_Socket(), fullTopicMessage.c_str(), _Channel[channelName].getTopic().length(), 0);
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
			std::string fullTopicMessage = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU TOPIC " + channelName + " " + newTopic + "\n";
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
				send(connIt->second->get_Socket(), fullTopicMessage.c_str(), fullTopicMessage.length(), 0);
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

void Server::msg(Clients *client, std::istringstream &lineStream, int client_socket, std::map<int, Clients *> &_clients)
{
	std::string dest;
	std::string msg;
	if (lineStream >> dest && std::getline(lineStream, msg))
	{
		// Supprimer les espaces en trop
		if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
			msg.erase(0, 2);
		// Recherche du destinataire dans la liste des clients
		bool found = false;
		int rSocket = -1;
		for (std::map<int, Clients *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (it->second->get_Nickname() == dest)
			{
				found = true;
				rSocket = it->first;
				break;
			}
		}
		if (found)
		{
			// Envoyer le message au destinataire
			std::string fullMsg = "MSG " + client->get_Nickname() + ": " + msg + "\n";
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

		// Add the client to the channel's connected users
		_Channel[channelName].getConnUsers()[client->get_Nickname()] = client;
		client->getCurrConnected().push_back(&_Channel[channelName]);

		// Notify all clients in the channel
		std::string joinMessage = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU JOIN " + channelName + "\n";
		for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
			send(it->second->get_Socket(), joinMessage.c_str(), joinMessage.length(), 0);

		// Send the current topic to the new client
		if (!_Channel[channelName].getTopic().empty())
		{
			std::string topicMessage = ":I.R.SIUSIU 332 " + client->get_Nickname() + " " + channelName + " :" + _Channel[channelName].getTopic() + "\n";
			send(client_socket, topicMessage.c_str(), topicMessage.length(), 0);
		}

		// Send the list of users in the channel to the new client
		std::string namesMessage = ":I.R.SIUSIU 353 " + client->get_Nickname() + " = " + channelName + " :";
		for (std::map<std::string, Clients*>::iterator it = _Channel[channelName].getConnUsers().begin(); it != _Channel[channelName].getConnUsers().end(); ++it)
			namesMessage += it->first + " ";
		namesMessage += "\n";
		send(client_socket, namesMessage.c_str(), namesMessage.length(), 0);

		// End of the list
		std::string endNamesMessage = ":I.R.SIUSIU 366 " + client->get_Nickname() + " " + channelName + " :End of /NAMES list.\n";
		send(client_socket, endNamesMessage.c_str(), endNamesMessage.length(), 0);
	}
	else
	{
		std::string errorMessage = "Usage: JOIN <channelName>\n";
		send(client_socket, errorMessage.c_str(), errorMessage.length(), 0);
	}
}

void	Server::part(Clients *client, std::istringstream &lineStream)
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
			std::string partMessageFull;
			// Remove the client from the channel's connected users
			_Channel[channelName].getConnUsers().erase(client->get_Nickname());
			connectedChannels.erase(it);

			// Notify all clients in the channel about the part
			if (!partMessage.empty())
				partMessageFull = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU PART " + channelName + " :" + partMessage + "\n";
			else
				partMessageFull = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU PART " + channelName + " :\n";
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
			{
				send(connIt->second->get_Socket(), partMessageFull.c_str(), partMessageFull.length(), 0);
			}

			// Optionally: Remove the channel if no users left
			if (_Channel[channelName].getConnUsers().empty())
				_Channel.erase(channelName);

			// notifier irssi de la deconnection du client
			send(client->get_Socket(), partMessageFull.c_str(), partMessageFull.length(), 0);
		}
		else
		{
			std::string errMsg = "You're not on that channel\n";
			send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
		}
	}
	else
	{
		std::string errMsg = "Usage: PART <channelName> [message]\n";
		send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
	}
}

void	Server::mode(Clients *client, std::istringstream &lineStream)
{
	std::string chan;
	std::string mode;
	lineStream >> chan;
	lineStream >> mode;

	std::cout << "chan : " << chan << std::endl;
	std::cout << "mode : " << mode << std::endl;
	if (chan.find("#") < chan.size())
	{

	}
	else
	{

	}
	(void)client;
}

//fonction permettant de verifier le mot de passe
bool	Server::pass(Clients *client, std::istringstream &lineStream, int client_socket)
{
	std::string pass;
	lineStream >> pass;
	if (pass == _pwd)
	{
		client->set_Status(Clients::USERNAME);
		return (true);
	}
	else
	{
		std::string errormsg = ":I.R.SIUSIU 300 " + client->get_Nickname() + " :Invalid password, try again...\n";
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
	client->set_Nickname(nick);
	std::cout << "Nickname set to: " << client->get_Nickname() << std::endl;
}

//fonction permettant de set l'Username, le Realname et le status de l'utilisateur
void	Server::user(Clients *client, std::istringstream &lineStream, int client_socket)
{
	std::string user, mode, unused, realname;
	lineStream >> user >> mode >> unused;
	std::getline(lineStream, realname);
	if (!realname.empty() && realname[0] == ' ' && realname[1] == ':')
		realname.erase(0, 2);
	client->set_Username(user);
	client->set_Realname(realname);
	client->set_Status(Clients::COMPLETED);
	std::cout << "Username set to: " << client->get_Username() << std::endl;
	std::cout << "Realname set to: " << client->get_Realname() << std::endl;
	sendWelcomeMessages(client_socket, client);
}

//fonction permettant de gerer les messages classiques dans un channel
void Server::regularChat(Clients *client, std::istringstream &lineStream, char *buffer)
{
	// Récupérer le nom du destinataire et le contenu du message
	std::string dest;
	std::string msg = buffer;

	if (lineStream >> dest)
	{
		if (dest.find("#") >= dest.size())
			return ;
		if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
			msg.erase(0, 2);
		std::cout << "msg: " << msg << std::endl;
		std::string sent_msg;
		for (std::vector<Channel *>::iterator currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
		{
			if (dest != (*currIt)->getChanName())
				continue ;
			sent_msg = ":" + client->get_Nickname() + " " + msg;
			for (std::map<std::string, Clients *>::iterator it = (*currIt)->getConnUsers().begin(); it != (*currIt)->getConnUsers().end(); ++it)
			{
				std::cout << "\e[0;33m" << it->first << " on port " << it->second->get_Socket() << "\e[0;0m" << std::endl;
				if (it->second->get_Socket() != client->get_Socket())
					send(it->second->get_Socket(), sent_msg.c_str(), sent_msg.length(), 0);
			}
			return ;
		}
	}
}