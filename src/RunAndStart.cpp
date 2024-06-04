#include "Server.hpp"

static bool sigStop = false;

//fonction permettant d'initialiser tout ce dont le serveur a besoin(ex: socket, bind, listen)
void	Server::start(int port)
{
	std::cout << "Starting the server on port " << port << "..." << std::endl;
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		throw std::runtime_error("Socket failed\n");
		exit(EXIT_FAILURE);
	}
	_serverAdr.sin_family = AF_INET;
	_serverAdr.sin_addr.s_addr = INADDR_ANY;
	_serverAdr.sin_port = htons(port);
	if (bind(_serverFd, (struct sockaddr *)&_serverAdr, sizeof(_serverAdr)) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Bind failed\n");
		exit(EXIT_FAILURE);
	}
	if (listen(_serverFd, 3) < 0)
	{
		close(_serverFd);
		throw std::runtime_error("Listen failed\n");
		exit(EXIT_FAILURE);
	}
	std::cout << "The server started with success !" << std::endl;
}

//fonction permettant de lancer le serveur
void	Server::run()
{
	std::vector<struct pollfd>	pollfds;
	struct pollfd				server_pollfd;
	server_pollfd.fd = _serverFd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	pollfds.push_back(server_pollfd);

	struct pollfd client_pollfd;
	client_pollfd.fd = -1;
	client_pollfd.events = 0;
	client_pollfd.revents = 0;

	signal(SIGINT, Server::sigInt_Hdl);
	
	while (sigStop == 0)
	{
		int poll_count = poll(pollfds.data(), pollfds.size(), -1);
		if (poll_count < 0)
			break ;
		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == _serverFd)
				{
					acceptNewConnection();
					int new_socket = _clients.rbegin()->first; // Get the last added client's socket
					client_pollfd.fd = new_socket;
					client_pollfd.events = POLLIN;
					pollfds.push_back(client_pollfd);
				}
				else
				{
					int client_socket = pollfds[i].fd;
					if (_clients[client_socket])
					{
						if (handleClientMessage(client_socket, _clients[client_socket]->getStatus()) == 1)
							pollfds.erase(pollfds.begin() + i);
					}
				}
			}
		}
	}
	if (!_clients.empty())
	{
		std::map<int, Clients*> clientMap = _clients;
		std::map<int, Clients*>::iterator it;
		std::istringstream line;
		for (it = clientMap.begin(); it != clientMap.end(); ++it)
			quit(it->second, line);
	}
	if (!_Channel.empty())
	{
		std::map<std::string, Channel*>::iterator it;
		for (it = _Channel.begin(); it != _Channel.end(); ++it)
			delete it->second;
	}
	if (client_pollfd.fd != -1)
		close(client_pollfd.fd);
	if (server_pollfd.fd != -1)
		close(server_pollfd.fd);
}

void    Server::sigInt_Hdl(int signo)
{
    if (signo == SIGINT)
	{
        sigStop = true;
		std::cerr << "\b\b  \b\b" << std::endl;
	}
}