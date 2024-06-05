#include "Server.hpp"

void Server::quit(Clients *client, std::istringstream &lineStream)
{
	std::string quitMsg;

	// Lire le message de déconnexion s'il est fourni
	if (lineStream >> quitMsg)
	{
		if (!quitMsg.empty() && quitMsg[0] == ' ' && quitMsg[1] == ':')
			quitMsg.erase(0, 2);
	}

	// Construire le message complet de déconnexion
	std::string fullQuitMsg;
	if (!quitMsg.empty())
		fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :" + quitMsg + "\n";
	else
		fullQuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU QUIT :\n";
	
	// Envoyer le message de déconnexion à tous les clients connectés aux mêmes canaux
	std::vector<Channel*>::iterator chIt;
	send(client->getSocket(), fullQuitMsg.c_str(), fullQuitMsg.size(), 0);
	for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
	{
		Channel *chan = *chIt;
		chan->notifyChannel(client, fullQuitMsg);
	}
	for (chIt = client->getCurrConnected().begin(); chIt != client->getCurrConnected().end(); ++chIt)
		part(client);

	// Supprimer le client de la liste des clients du serveur
	// Fermer la socket du client
	std::map<int, Clients*>::iterator it = _clients.find(client->getSocket());
	if (it != _clients.end())
	{
		delete it->second;
		_clients.erase(it->first);
	}
}

void Server::quit(Clients *client)
{
	// Supprimer le client de la liste des clients du serveur
	// Fermer la socket du client
	std::map<int, Clients*>::iterator it = _clients.find(client->getSocket());
	if (it != _clients.end())
	{
		delete it->second;
		_clients.erase(it->first);
	}
}
