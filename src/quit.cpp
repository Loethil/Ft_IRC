#include "Server.hpp"

// void Server::quit(Clients *client, std::istringstream &lineStream)
// {
//     std::string quitMsg;

//     // Lire le message de déconnexion s'il est fourni
//     if (std::getline(lineStream, quitMsg))
//     {
//         if (!quitMsg.empty() && quitMsg[0] == ' ' && quitMsg[1] == ':')
//             quitMsg.erase(0, 2);
//     }

//     // Construire le message complet de déconnexion
//     std::string fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :";
//     fullQuitMsg += quitMsg.empty() ? "" : quitMsg;
//     fullQuitMsg += "\n";

//     // Utiliser un vector pour suivre les sockets déjà notifiées
//     std::vector<int> notifiedSockets;

//     // Envoyer le message de déconnexion à tous les clients connectés aux mêmes canaux
//     std::vector<Channel*>::iterator chIt;
//     for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
//     {
//         Channel* channel = *chIt;
//         std::map<std::string, Clients*>::iterator chanClientIt;
//         for (chanClientIt = channel->getConnUsers().begin(); chanClientIt != channel->getConnUsers().end(); ++chanClientIt)
//         {
//             Clients* chanClient = chanClientIt->second;
//             if (std::find(notifiedSockets.begin(), notifiedSockets.end(), chanClient->getSocket()) == notifiedSockets.end())
//             {
//                 send(chanClient->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
//                 notifiedSockets.push_back(chanClient->getSocket());
//             }
//         }
//     }

//     // Supprimer le client de tous les canaux auxquels il est connecté
//     for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
//     {
//         part(client); // Assurez-vous que cette fonction retire le client du canal et met à jour les listes de membres
//     }
//     client->getCurrConnected().clear();

//     // Supprimer le client de la liste des clients du serveur et fermer la socket du client
//     std::map<int, Clients*>::iterator it = _clients.find(client->getSocket());
//     if (it != _clients.end())
//     {
//         close(client->getSocket());
//         delete it->second;
//         _clients.erase(it);
//     }
//     std::cout << "Client " << client->getNickname() << " has disconnected." << std::endl;
// }

void Server::quit(Clients *client, std::istringstream &lineStream)
{
    std::string quitMsg;

    // Lire le message de déconnexion s'il est fourni
    if (std::getline(lineStream, quitMsg))
    {
        if (!quitMsg.empty() && quitMsg[0] == ' ')
            quitMsg.erase(0, 1);
    }

    // Construire le message complet de déconnexion
    std::string fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :";
    fullQuitMsg += quitMsg.empty() ? "" : quitMsg;
    fullQuitMsg += "\n";

    // Utiliser un vector pour suivre les sockets déjà notifiées
    std::vector<int> notifiedSockets;

    // Parcourir tous les canaux pour trouver et supprimer le client
    for (std::map<std::string, Channel>::iterator it = _Channel.begin(); it != _Channel.end();)
    {
        if (it->second.getConnUsers().find(client->getNickname()) != it->second.getConnUsers().end())
        {
            std::cerr << "Client found in channel: " << it->first << std::endl;
            it->second.getConnUsers().erase(client->getNickname());
            
            // Envoyer le message de déconnexion à tous les clients connectés aux mêmes canaux
            for (std::map<std::string, Clients*>::iterator chanClientIt = it->second.getConnUsers().begin();
                 chanClientIt != it->second.getConnUsers().end(); ++chanClientIt)
            {
                Clients* chanClient = chanClientIt->second;
                if (std::find(notifiedSockets.begin(), notifiedSockets.end(), chanClient->getSocket()) == notifiedSockets.end())
                {
                    send(chanClient->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
                    notifiedSockets.push_back(chanClient->getSocket());
                }
            }

            // Vérifier si le canal est vide après la suppression du client
            if (it->second.getConnUsers().empty())
            {
                it = _Channel.erase(it); // retourne un nouvel itérateur valide
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }

    // Supprimer le client de la liste des clients du serveur
    std::map<int, Clients*>::iterator clientIt = _clients.find(client->getSocket());
    if (clientIt != _clients.end())
    {
        delete clientIt->second;  // Le destructeur fermera la socket
        _clients.erase(clientIt);
    }

    std::cout << "Client " << client->getNickname() << " has disconnected." << std::endl;
}