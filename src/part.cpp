#include "Server.hpp"

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
void Server::part(Clients *client)
{
    std::cerr << "Part command received from client: " << client->getNickname() << std::endl;

    // Utiliser un conteneur temporaire pour stocker les canaux à vérifier pour suppression
    std::vector<std::string> channelsToRemove;

    // Parcourir tous les canaux pour trouver et supprimer le client
    for (std::map<std::string, Channel>::iterator it = _Channel.begin(); it != _Channel.end(); ++it)
    {
        if (it->second.getConnUsers().find(client->getNickname()) != it->second.getConnUsers().end())
        {
            std::cerr << "Client found in channel: " << it->first << std::endl;
            it->second.getConnUsers().erase(client->getNickname());
            
            // Vérifier si le canal est vide après la suppression du client
            if (it->second.getConnUsers().empty())
            {
                channelsToRemove.push_back(it->first); // Ajouter le canal à la liste de suppression
                it->second.getConnUsers().clear();
            }
        }
        else
        {
            std::cerr << "Client not found in channel: " << it->first << std::endl;
        }
    }

    // Supprimer les canaux vides
    for (size_t i = 0; i < channelsToRemove.size(); ++i)
    {
        _Channel.erase(channelsToRemove[i]);
    }
}
