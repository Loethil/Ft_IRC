#include "Server.hpp"

void Server::pong(Clients *client, const std::string &token)
{
	std::string response = "PONG :" + token + "\n";
	send(client->getSocket(), response.c_str(), response.size(), 0);
}
