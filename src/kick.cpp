
#include "Server.hpp"

void Server::kick(Clients *client, std::istringstream &lineStream)
{
    std::string channelName, nickname, reason;

    if (lineStream >> channelName >> nickname)
    {
        if (std::getline(lineStream, reason))
        {
            if (!reason.empty() && reason[0] == ' ' && reason[1] == ':')
                reason.erase(0, 2);
        }

        // Vérifier si le canal existe
        std::map<std::string, Channel*>::iterator chanIt = _Channel.find(channelName);
        if (chanIt == _Channel.end())
        {
            std::string errMsg = "No such channel: " + channelName + "\n";
            send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
            return;
        }

        Channel* channel = chanIt->second;

        // Vérifier si l'utilisateur qui effectue le kick est un opérateur
        if (!channel->getOpStatus(client->getNickname()))
        {
            std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + channelName + " :You're not an operator\n";
            send(client->getSocket(), msg.c_str(), msg.size(), 0);
            return;
        }

        // Vérifier si l'utilisateur à expulser est dans le canal
        Clients* kickedClient = NULL;
        for (std::map<int, Clients*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (it->second->getNickname() == nickname)
            {
                kickedClient = it->second;
                break;
            }
        }
        if (kickedClient == NULL)
        {
            // L'utilisateur à expulser n'existe pas
            std::string errMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + channelName + " :User " + nickname + " does not exist\n";
            send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
            return;
        }

        // Vérifier si l'utilisateur à expulser est dans le canal
        if (channel->getConnUsers().find(kickedClient->getNickname()) == channel->getConnUsers().end())
        {
            // L'utilisateur à expulser n'est pas dans le canal
            std::string errMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + channelName + " :User " + nickname + " is not in channel " + channelName + "\n";
            send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
            return;
        }

        // Construire et envoyer le message KICK à tous les utilisateurs du canal
        std::string kickMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU KICK " + channelName + " " + kickedClient->getNickname();
        if (!reason.empty())
            kickMessage += " :" + reason + "\n";
        else
            kickMessage += " :\n";
        part(kickedClient, channelName);
        // Envoyer le message de kick à tous les utilisateurs du canal
        for (std::map<std::string, Clients*>::iterator it = channel->getConnUsers().begin(); it != channel->getConnUsers().end(); ++it)
        {
            send(it->second->getSocket(), kickMessage.c_str(), kickMessage.length(), 0);
        }
    }
    else
    {
        std::string errMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + channelName + " :Usage: KICK <channel> <user> [<reason>]\n";
        send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
    }
}
