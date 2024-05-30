#include "Server.hpp"

//fonction permettant de set le nickname de l'utilisateur
void	Server::nick(Clients *client, std::istringstream &lineStream)
{
	std::string nick;
	lineStream >> nick;
	client->setNickname(nick);
	std::cout << "Nickname set to: " << client->getNickname() << std::endl;
}
