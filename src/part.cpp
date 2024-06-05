#include "Server.hpp"

//commande part de base
void Server::part(Clients *client, std::istringstream &lineStream)
{
	std::string channelName;
	std::string partMessage;

	if (lineStream >> channelName)
	{
		if (lineStream >> partMessage)
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
			std::string partMessageFull;
			_Channel[channelName]->getConnUsers().erase(client->getNickname());
			connectedChannels.erase(it);
			_Channel[channelName]->setCurrentUser(false);

			if (!partMessage.empty())
				partMessageFull = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :" + partMessage + "\n";
			else
				partMessageFull = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :\n";
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName]->getConnUsers().begin(); connIt != _Channel[channelName]->getConnUsers().end(); ++connIt)
				send(connIt->second->getSocket(), partMessageFull.c_str(), partMessageFull.length(), 0);
			if (_Channel[channelName]->getConnUsers().empty())
			{
				delete _Channel[channelName];
				_Channel.erase(channelName);
			}
			send(client->getSocket(), partMessageFull.c_str(), partMessageFull.length(), 0);
		}
		else
		{
			std::string errMsg = RED "You're not on that channel" RESET "\n";
			send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
		}
	}
}

//c'est le part pour le /quit celui la
void Server::part(Clients *client)
{
	std::map<std::string, Channel*> channelMap = _Channel;
	std::map<std::string, Channel*>::iterator it;
	for (it = channelMap.begin(); it != channelMap.end(); ++it)
	{
		if (it->second->getConnUsers().find(client->getNickname()) != it->second->getConnUsers().end())
		{
			it->second->getConnUsers().erase(client->getNickname());
			it->second->setCurrentUser(false);
		}
	}
	for (std::map<std::string, Channel*>::iterator chIt =  channelMap.begin(); it != channelMap.end(); ++chIt)
		if (it->second->getConnUsers().empty())
		{
			delete _Channel[it->first];
			_Channel.erase(it->first);
		}
}

//part du kick
void Server::part(Clients *client, std::string channelName)
{
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
		std::string partMessageFull = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU PART " + channelName + " :\n";
		send(client->getSocket(), partMessageFull.c_str(), partMessageFull.length(), 0);
		// Supprimer l'utilisateur de la liste des utilisateurs connectés au canal
		_Channel[channelName]->setCurrentUser(false);
		_Channel[channelName]->getConnUsers().erase(client->getNickname());
		connectedChannels.erase(it);
	}
}
