#include "Server.hpp"

//fonction permettant de set l'Username, le Realname et le status de l'utilisateur
void	Server::user(Clients *client, std::istringstream &lineStream)
{
	std::string user, mode, unused, realname;
	lineStream >> user >> mode >> unused >> realname;
	if (!realname.empty() && realname[0] == ' ' && realname[1] == ':')
		realname.erase(0, 2);
	client->setUsername(user);
	client->setRealname(realname);
	client->setStatus(Clients::COMPLETED);
	std::cout << "Username set to: " << client->getUsername() << std::endl;
	std::cout << "Realname set to: " << client->getRealname() << std::endl;
	sendWelcomeMessages(client->getSocket(), client);
}
