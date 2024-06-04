#include "Server.hpp"

void	Server::botStart(int port)
{
	_botFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_botFd == -1)
	{
		throw std::runtime_error("Socket failed\n");
		exit(EXIT_FAILURE);
	}
	_serverAdr.sin_family = AF_INET;
	_serverAdr.sin_addr.s_addr = INADDR_ANY;
	_serverAdr.sin_port = htons(port);
	if (bind(_botFd, (struct sockaddr *)&_serverAdr, sizeof(_serverAdr)) < 0)
	{
		close(_botFd);
		throw std::runtime_error("Bind failed\n");
		exit(EXIT_FAILURE);
	}
	if (listen(_botFd, 3) < 0)
	{
		close(_botFd);
		throw std::runtime_error("Listen failed\n");
		exit(EXIT_FAILURE);
	}
	std::cout << "The server started with success !" << std::endl;
}

void    Clients::setIsBot(bool key)
{
    _isBot = key;
}

int	Server::acceptBotConnection()
{
	socklen_t addrlen = sizeof(_botAdr);
	int new_socket = accept(_serverFd, (struct sockaddr *)&_botAdr, &addrlen);
	if (new_socket == -1)
	{
		throw std::runtime_error("Accept failed");
		exit(EXIT_FAILURE);
	}
	Clients *newClient = new Clients();
	newClient->setSocket(new_socket);
	newClient->setStatus(Clients::USERNAME);
    newClient->setIsBot(true);
    newClient->setNickname("Bob1d1");
    newClient->setUsername("B4bidi");
    newClient->setRealname("Bo0");
	_clients[new_socket] = newClient;
    return (new_socket);
}