#include "Server.hpp"

void	Server::topic(Clients *client, std::istringstream &lineStream)
{
	std::string channelName;
	std::string newTopic;

	// Read the channel name
	if (lineStream >> channelName)
	{
		// Read the new topic
		if (lineStream >> newTopic)
		{
			if (!newTopic.empty() && newTopic[0] == ' ' && newTopic[1] == ':')
				newTopic.erase(0, 2);
		}
		else
		{
			if (_Channel[channelName])
			{
				if (_Channel[channelName]->getConnUsers().find(client->getNickname()) != _Channel[channelName]->getConnUsers().end() && _Channel[channelName]->getTopic().size() > 0)
				{
					std::string fullTopicMessage = ":I.R.SIUSIU 332 " + client->getNickname() + " " + channelName + " :" + _Channel[channelName]->getTopic() + "\n";
					send(client->getSocket(), fullTopicMessage.c_str(), _Channel[channelName]->getTopic().length(), 0);
				}
				else
				{
					std::string send_msg = ":I.R.SIUSIU 442 " + client->getNickname() + channelName + " :" RED "You are not connected to that channel.\n" RESET;
					send(client->getSocket(), send_msg.c_str(), send_msg.size(), 0);
				}
			}
			else
			{
				std::string errMsg = ":I.R.SIUSIU 403 " + client->getNickname() + " " + channelName + " :" RED "No such channel" RESET "\n";
				send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
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
			for (std::map<std::string, Clients*>::iterator connIt = _Channel[channelName]->getConnUsers().begin(); connIt != _Channel[channelName]->getConnUsers().end(); ++connIt)
				send(connIt->second->getSocket(), fullTopicMessage.c_str(), fullTopicMessage.length(), 0);
		}
		else
		{
			std::string send_msg = ":I.R.SIUSIU 442 " + client->getNickname() + channelName + " :" RED "You are not connected to that channel.\n" RESET;
			send(client->getSocket(), send_msg.c_str(), send_msg.size(), 0);
		}
	}
}
