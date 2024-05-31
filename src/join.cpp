#include "Server.hpp"

void	Server::joinChannel(Clients *client, std::string channelName)
{
	// Add the client to the channel's connected users
	_Channel[channelName]->getConnUsers()[client->getNickname()] = client;
	client->getCurrConnected().push_back(_Channel[channelName]);
	if (_Channel[channelName]->getConnUsers().size() == 1)
		_Channel[channelName]->setOperator(client, true, client->getNickname());

	// Notify all clients in the channel
	std::string joinMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU JOIN " + channelName + "\n";
	for (std::map<std::string, Clients*>::iterator it = _Channel[channelName]->getConnUsers().begin(); it != _Channel[channelName]->getConnUsers().end(); ++it)
		send(it->second->getSocket(), joinMessage.c_str(), joinMessage.length(), 0);

	// Send the current topic to the new client
	if (!_Channel[channelName]->getTopic().empty())
	{
		std::string topicMessage = ":I.R.SIUSIU 332 " + client->getNickname() + " " + channelName + " :" + _Channel[channelName]->getTopic() + "\n";
		send(client->getSocket(), topicMessage.c_str(), topicMessage.length(), 0);
	}

	// Send the list of users in the channel to the new client
	std::string namesMessage = ":I.R.SIUSIU 353 " + client->getNickname() + " = " + channelName + " :";
	for (std::map<std::string, Clients*>::iterator it = _Channel[channelName]->getConnUsers().begin(); it != _Channel[channelName]->getConnUsers().end(); ++it)
		namesMessage += it->first + " ";
	namesMessage += "\n";
	send(client->getSocket(), namesMessage.c_str(), namesMessage.length(), 0);

	// End of the list
	std::string endNamesMessage = ":I.R.SIUSIU 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list.\n";
	send(client->getSocket(), endNamesMessage.c_str(), endNamesMessage.length(), 0);
}

void	Server::join(Clients *client, std::istringstream &lineStream)
{
	std::string channelName;
	if (lineStream >> channelName)
	{
		// Find or create the channel
		if (_Channel.find(channelName) == _Channel.end())
		{
			Channel *new_channel = new Channel(channelName);
			_Channel[channelName] = new_channel;
			std::cout << "New Channel created: " << channelName << std::endl;
		}

		Channel *channel = _Channel[channelName];

		if (channel->getInvite() == false)
		{
			if (channel->getPwd().empty())
				joinChannel(client, channelName);
			else
			{
				std::string pwd;
				if (lineStream >> pwd)
				{
					if (pwd.compare(channel->getPwd()) == 0)
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
			for(invIt = channel->getInvitedUsers().begin(); invIt != channel->getInvitedUsers().end(); ++invIt)
			{
				if ((*invIt).compare(client->getNickname()) == 0)
				{
					if (channel->getPwd().empty())
						joinChannel(client, channelName);
					else
					{
						std::string pwd;
						if (lineStream >> pwd)
						{
							if (pwd.compare(channel->getPwd()) == 0)
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
			if (invIt == channel->getInvitedUsers().end())
			{
				std::string errMsg = ":I.R.SIUSIU 473 " + client->getNickname() + " " + channelName + " :Cannot join channel (+i)\n";
				send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
			}
		}
	}
}
