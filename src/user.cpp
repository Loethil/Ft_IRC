#include "Server.hpp"

//fonction permettant de set l'Username, le Realname et le status de l'utilisateur
void	Server::user(Clients *client, std::istringstream &lineStream)
{
	if (client->getNickname().empty())
	{
		std::string msg = "A nickname is required on our server, type:\n\t[NICK <your_nick_name>]\nBefore setting your Username\n";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		return ;
	}
	std::string user, mode, unused, realname;
	lineStream >> user >> mode >> unused;
	std::getline(lineStream, realname);
	if (!realname.empty() && realname[0] == ' ' && realname[1] == ':')
		realname.erase(0, 2);
	client->setUsername(user);
	client->setRealname(realname);
	client->setStatus(Clients::COMPLETED);
	sendWelcomeMessages(client->getSocket(), client);
}