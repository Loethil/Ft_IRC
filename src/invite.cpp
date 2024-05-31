#include "Server.hpp"

void Server::invite(Clients *client, std::istringstream &lineStream)
{
    std::string nickname;
    std::string channelName;

	// Check de l'operator status
    // Lire le nom de l'utilisateur à inviter et le canal
    if (lineStream >> nickname >> channelName)
    {
		if (_Channel[channelName]->getOpStatus(client->getNickname()) == false)
		{
			std::string msg = ":I.R.SIUSIU PRIVMSG " + channelName + " :" RED "You're not an operator" RESET "\n";
			send(client->getSocket(), msg.c_str(), msg.size(), 0);
			return ;
		}
        std::map<std::string, Channel *>::iterator chanIt = _Channel.find(channelName);
        if (chanIt != _Channel.end())
        {
            Channel *channel = chanIt->second;
            
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
                channel->addInvite(nickname);

                // Envoyer un message à l'utilisateur invité
                std::string inviteMessage = ":I.R.SIUSIU 341 " + client->getNickname() + " " + nickname + " " + channelName + "\n";
                send(invitedClient->getSocket(), inviteMessage.c_str(), inviteMessage.length(), 0);

                // Informer l'utilisateur qui envoie l'invitation du succès
                std::string successMsg = "You invited " + nickname + " to " + channelName + "\n";
                send(client->getSocket(), successMsg.c_str(), successMsg.length(), 0);
            }
            else
            {
                // L'utilisateur à inviter n'existe pas
                std::string errMsg = "User " + nickname + " does not exist\n";
                send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
            }
        }
        else
        {
            // Le canal n'existe pas
            std::string errMsg = "No such channel: " + channelName + "\n";
            send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
        }
    }
}
