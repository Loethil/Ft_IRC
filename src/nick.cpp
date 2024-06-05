#include "Server.hpp"

//fonction permettant de set le nickname de l'utilisateur
int	Server::nick(Clients *client, std::istringstream &lineStream)
{
	std::string nick;
	lineStream >> nick;
	client->setNickname(nick);
	std::cout << "Nickname set to: " << client->getNickname() << std::endl;
	for (std::map<int , Clients *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		
		if (it->second->getNickname() == client->getNickname() && it->second->getSocket() != client->getSocket())
		{
			std::string errorMsg = RED "ERROR: A user has already this nickname. Please try another one" RESET "\n";
			send(client->getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
			return (1);
		}
	}
	return (0);
}

//fonction permettant d'afficher les messages de bienvenu a l'utilisateur
void Server::sendWelcomeMessages(int client_socket, Clients *client)
{
	std::string truncatedNick = client->getNickname().substr(0, 9); // Tronque le pseudo à 9 caractères
	std::string paddedNick = truncatedNick;
	if (truncatedNick.size() < 9)
		paddedNick += std::string(9 - truncatedNick.size(), ' ');

	std::string welcomeMsg = ":I.R.SIUSIU 001 " + client->getNickname() + " :" GREEN "Welcome to the IRC server, " + client->getRealname() + RESET "\n";
	send(client_socket, welcomeMsg.c_str(), welcomeMsg.size(), 0);

	std::string yourHost = ":I.R.SIUSIU 002 " + client->getNickname() + " :" GREEN "Your host is I.R.SIUSIU, running version 1.0" RESET "\n";
	send(client_socket, yourHost.c_str(), yourHost.size(), 0);

	std::string created = ":I.R.SIUSIU 003 " + client->getNickname() + "\n";
	send(client_socket, created.c_str(), created.size(), 0);

	std::string myInfo = ":I.R.SIUSIU 004 " + client->getNickname() + "\n";
	send(client_socket, myInfo.c_str(), myInfo.size(), 0);

	std::string motdStart = ":I.R.SIUSIU 375 " + client->getNickname() + " :" YELLOW "------ I.R.SIUSIU Message of the Day ------" RESET "\n";
	send(client_socket, motdStart.c_str(), motdStart.size(), 0);

	std::string motdSpaces = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "|                                         |" RESET "\n";
	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "| Enjoy our Internet Relay chat " + paddedNick + " |" RESET "\n";
	send(client_socket, motd.c_str(), motd.size(), 0);

	send(client_socket, motdSpaces.c_str(), motdSpaces.size(), 0);

	std::string motd2 = ":I.R.SIUSIU 372 " + client->getNickname() + " :" YELLOW "-------------------------------------------" RESET "\n";
	send(client_socket, motd2.c_str(), motd2.size(), 0);

	std::string motdEnd = ":I.R.SIUSIU 376 " + client->getNickname() + " :End of Message Of The Day\n";
	send(client_socket, motdEnd.c_str(), motdEnd.size(), 0);
}
