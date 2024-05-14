#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>

struct Client {
  int socket;
  std::string nickname;
  std::string username;
  std::string hostname;
  std::string realname;
  std::vector<std::string> channels;
};

int	main(int argc, char **argv)
{
	(void)argc;
	struct sockaddr_in  	addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}
	int	serverSockfd = sockfd;

	// Configurer l'adresse
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(atoi(argv[2]));

	// Lier le socket à l'adresse et au port
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	// Définir le mode non bloquant
	int flags = fcntl(sockfd, F_GETFL, 0);
		if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("fcntl");
		exit(1);
	}

	// Écouter les connexions entrantes
	if (listen(sockfd, SOMAXCONN) < 0) {
		perror("listen");
		exit(1);
	}

	std::vector<Client> clients;
	while (true) 
	{
		// Configurer la structure pollfd
		struct pollfd pfd[clients.size() + 1];
		pfd[0].fd = serverSockfd;
		pfd[0].events = POLLIN | POLLERR;

		for (unsigned long i = 0; i < clients.size(); ++i) {
			pfd[i + 1].fd = clients[i].socket;
			pfd[i + 1].events = POLLIN | POLLERR | POLLHUP;
		}

		// Surveiller les événements
		int n = poll(pfd, clients.size() + 1, -1);
		if (n < 0) {
			perror("poll");
			break;
		}

		// Gérer les événements
		for (int i = 0; i < n; ++i) 
		{
			if (pfd[i].fd == serverSockfd)
			{
				// Accepter une nouvelle connexion
				int new_socket = accept(serverSockfd, NULL, NULL);

				if (new_socket >= 0)
				{
					std::cout << "New connection accepted" << std::endl;
				} 
			}
			else if (pfd[i].revents & POLLIN) // Gérer les événements du client (lecture, écriture, déconnexion)
			{
				std::cout << "message send" << std::endl;
			}
			else
				std::cout << "error POLLIN not set" << std::endl;
		}
	}
}
