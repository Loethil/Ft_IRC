#include "Server.hpp"

void Server::quit(Clients *client, std::istringstream &lineStream)
{
	std::string quitMsg;

	// Lire le message de déconnexion s'il est fourni
	if (lineStream >> quitMsg)
	{
		if (std::getline(lineStream, quitMsg))
		{
			if (!quitMsg.empty() && quitMsg[0] == ' ' && quitMsg[1] == ':')
				quitMsg.erase(0, 2);
		}
	}

	// Construire le message complet de déconnexion
	std::string fullQuitMsg;
	if (!quitMsg.empty())
		fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :" + quitMsg + "\n";
	else
		fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :\n";
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
			//Clients* chanClientIt = *chanClientIt;
			if (std::find(notifiedSockets.begin(), notifiedSockets.end(), chanClientIt->second->getSocket()) == notifiedSockets.end())
			{
				send(chanClientIt->second->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
				notifiedSockets.push_back(chanClientIt->second->getSocket());
			}
		}
	}
	//a REGLER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Supprimer le client de tous les canaux
	for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
	{
		if (client->getCurrConnected().empty())
			part(client, (*chIt)->getChanName());
		else
			break ;
	}
	// Supprimer le client de la liste des clients du serveur
	int clientSocket = client->getSocket();
	_clients.erase(clientSocket);
	// Fermer la socket du client
	close(clientSocket);
}