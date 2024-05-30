#include "Server.hpp"

//fonction permettant de verifier le mot de passe
bool	Server::pass(Clients *client, std::istringstream &lineStream)
{
	std::string pass;
	lineStream >> pass;
	if (pass == _pwd)
	{
		client->setStatus(Clients::USERNAME);
		return (true);
	}
	else
	{
		std::string errormsg = ":I.R.SIUSIU 300 " + client->getNickname() + " :Invalid password, try again...\n";
		send(client->getSocket(), errormsg.c_str(), errormsg.size(), 0);
		close(client->getSocket());
		delete _clients[client->getSocket()];
		_clients.erase(client->getSocket());
		return (false);
	}
}
