#include "Server.hpp"

void Server::quit(Clients *client, std::istringstream &lineStream)
{
    std::string quitMsg;

    // Lire le message de déconnexion s'il est fourni
    if (std::getline(lineStream, quitMsg))
    {
        if (!quitMsg.empty() && quitMsg[0] == ' ' && quitMsg[1] == ':')
            quitMsg.erase(0, 2);
    }

    // Construire le message complet de déconnexion
    std::string fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :";
    fullQuitMsg += quitMsg.empty() ? "" : quitMsg;
    fullQuitMsg += "\n";

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
            Clients* chanClient = chanClientIt->second;
            if (std::find(notifiedSockets.begin(), notifiedSockets.end(), chanClient->getSocket()) == notifiedSockets.end())
            {
                send(chanClient->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
                notifiedSockets.push_back(chanClient->getSocket());
            }
        }
    }

    // Supprimer le client de tous les canaux auxquels il est connecté
    for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
    {
        part(client); // Assurez-vous que cette fonction retire le client du canal et met à jour les listes de membres
    }
    client->getCurrConnected().clear();

    // Supprimer le client de la liste des clients du serveur et fermer la socket du client
    std::map<int, Clients*>::iterator it = _clients.find(client->getSocket());
    if (it != _clients.end())
    {
        close(client->getSocket());
        delete it->second;
        _clients.erase(it);
    }

    std::cout << "Client " << client->getNickname() << " has disconnected." << std::endl;
}

