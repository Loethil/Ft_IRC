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
			std::string send_msg = ":I.R.SIUSIU 442 " + client->get_Nickname() + channelName + " :" RED "You are not connected to that channel.\n" RESET;
			send(client->get_Socket(), send_msg.c_str(), send_msg.size(), 0);
		}
	}
	else
	{
		std::string errMsg = ":I.R.SIUSIU " + client->get_Nickname() + "TOPIC " + channelName + " :" RED "Command error. Use : TOPIC <channelName> [new_topic]\n" RESET;
		send(client_socket, errMsg.c_str(), errMsg.length(), 0);
	}
}

void Server::msg(Clients *client, std::istringstream &lineStream, char *buffer)
{
    std::string dest;
    std::string msg = buffer;

    std::cerr << "Msg command received from client: " << client->get_Nickname() << std::endl;

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
				sent_msg = ":" + client->get_Nickname() + " " + msg;
				for (std::map<std::string, Clients *>::iterator it = (*currIt)->getConnUsers().begin(); it != (*currIt)->getConnUsers().end(); ++it)
				{
					if (it->second->get_Socket() != client->get_Socket())
						send(it->second->get_Socket(), sent_msg.c_str(), sent_msg.length(), 0);
				}
				return ;
            }
			if (currIt == client->getCurrConnected().end())
			{
				std::string send_msg = ":I.R.SIUSIU 442 " + client->get_Nickname() + dest + " :" RED "You are not connected to that channel.\n" RESET;
				send(client->get_Socket(), send_msg.c_str(), send_msg.size(), 0);
			}
        }
        else
        {
            if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
                msg.erase(0, 2);
            std::string send_msg;
            for (std::map<int, Clients*>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter)
            {
                if (iter->second->get_Nickname() == dest)
                {
                    send_msg = ":" + client->get_Nickname() + " " + msg;
                    send(iter->second->get_Socket(), send_msg.c_str(), send_msg.size(), 0);
                    return ;
                }
            }
            std::string errMsg = ":I.R.SIUSIU 401 " + client->get_Nickname() + " " + dest + " :" RED "No such user\n" RESET;
            send(client->get_Socket(), errMsg.c_str(), errMsg.size(), 0);
        }
    }
    else
    {
        std::string errMsg = "MSG command error. Use : MSG <destinataire> <message>\n";
        send(client->get_Socket(), errMsg.c_str(), errMsg.size(), 0);
    }
}

void Server::invite(Clients *client, std::istringstream &lineStream, int client_socket)
{
    std::string nickname;
    std::string channelName;

    // Lire le nom de l'utilisateur à inviter et le canal
    if (lineStream >> nickname >> channelName)
    {
        std::map<std::string, Channel>::iterator chanIt = _Channel.find(channelName);
        if (chanIt != _Channel.end())
        {
            Channel &channel = chanIt->second;
            
            // Vérifier si l'utilisateur qui envoie l'invitation est dans le canal
            // Vérifier si l'utilisateur à inviter existe
            Clients* invitedClient = NULL;
            for(std::map<int, Clients*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (it->second->get_Nickname() == nickname)
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
                std::string inviteMessage = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU INVITE " + nickname + " :" + channelName + "\n";
                send(invitedClient->get_Socket(), inviteMessage.c_str(), inviteMessage.length(), 0);

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
    else
    {
        // La commande INVITE est mal formée
        std::string errMsg = "INVITE command error. Use: INVITE <nickname> <channel>\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
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
			// Make the first client to connect the operator of the channel
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

void Server::part(Clients *client, std::istringstream &lineStream)
{
    std::string channelName;
    std::string partMessage;
    
    std::cerr << "Part command received from client: " << client->get_Nickname() << std::endl;

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
            _Channel[channelName].getConnUsers().erase(client->get_Nickname());
            connectedChannels.erase(it);

            if (!partMessage.empty())
                partMessageFull = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU PART " + channelName + " :" + partMessage + "\n";
            else
                partMessageFull = ":" + client->get_Nickname() + "!" + client->get_Username() + "@I.R.SIUSIU PART " + channelName + " :\n";
            for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName].getConnUsers().begin(); connIt != _Channel[channelName].getConnUsers().end(); ++connIt)
            {
                send(connIt->second->get_Socket(), partMessageFull.c_str(), partMessageFull.length(), 0);
            }
			if (_Channel[channelName].getConnUsers().empty())
                _Channel.erase(channelName);

            send(client->get_Socket(), partMessageFull.c_str(), partMessageFull.length(), 0);
        }
        else
        {
            std::cerr << "Client not found in channel: " << channelName << std::endl;
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
	std::string name;
	std::vector<Channel *>::iterator currIt;

	lineStream >> chan;
	lineStream >> mode;
	lineStream >> name;
	std::cout << "chan : " << chan << std::endl;
	std::cout << "mode : " << mode << std::endl;
	std::cout << "name : " << name << std::endl;
	for (currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
	{
		if (chan == (*currIt)->getChanName())
		{
			if ((*currIt)->getConnUsers().find(name) == (*currIt)->getConnUsers().end() && !name.empty())
			{
				std::string errMsg = ":I.R.SIUSIU 401 " + client->get_Nickname() + " " + chan + " :" RED "No such user\n" RESET;
				send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
				return ;
			}
			break ;
		}
	}
	if (currIt == client->getCurrConnected().end() && mode != "+o")
	{
		if (chan == client->get_Nickname())
			return ;
		std::string errMsg = "This channel does not exist\n";
        send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
		return ;
	}
	// if (mode.length() > 2)
	// {
	// 	std::string errMsg = "Only one option mode at a time\n";
    //     send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
	// 	return ;
	// }
	if (mode[0] == '+' || mode[0] == '-')
	{
		bool type = (mode[0] == '+') ? true : false;
		if (mode.find('i') < mode.length())
		{
			if (type == true)
				(*currIt)->set_invit(true);
			else
				(*currIt)->set_invit(false);
		}
		if (mode.find('t') < mode.length())
		{
			if (type == true)
				(*currIt)->set_topic_mode(true);
			else
				(*currIt)->set_topic_mode(false);
		}
		if (mode.find('k') < mode.length())
		{
			if (type == true)
			{

				return ;
			}

		}
		if (mode.find('o') < mode.length())
		{
			if (type == true)
				(*currIt)->set_operator(name, true);
			else
				(*currIt)->set_operator(name, false);
		}
		if (mode.find('l') < mode.length())
		{
			if (type == true)
			{

				return ;
			}

		}
		else
		{
			std::string errMsg = "This option is not handle, see IRC subject\n";
			send(client->get_Socket(), errMsg.c_str(), errMsg.length(), 0);
		}
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
