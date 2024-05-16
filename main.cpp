#include "Clients.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

std::vector<Clients> clients;

int initServer(int port) {
    int server_fd;
    struct sockaddr_in address;

	std::cout << "Starting the server..." << std::endl;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) 
	{
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
	{
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) 
	{
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
	std::cout << "The server is started with success !" << std::endl;
    return server_fd;
}

void acceptNewConnection(int server_fd) 
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) 
	{
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    Clients newClient;
    newClient.set_Socket(new_socket);
    clients.push_back(newClient);
    std::cout << "New connection accepted" << std::endl;
	if (clients[new_socket - 4].get_Username().compare("") == 0)
	{
		char buf[BUFFER_SIZE];

		send(new_socket, "Set an Username : ", 19, 0);
		int valread = recv(new_socket, buf, BUFFER_SIZE, 0);
		buf[valread - 1] = '\0';
		clients[new_socket - 4].set_Username(buf);		
		std::string name = "Welcome " + clients[new_socket - 4].get_Username() + '\n';
		send(new_socket, name.c_str(), name.size(), 0);
	}
}

void handleClientMessage(int client_socket) 
{
    char buffer[BUFFER_SIZE];
    int valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread == 0) 
	{
        close(client_socket);
        std::cout << "Client disconnected" << std::endl;
        return;
    }
    buffer[valread] = '\0';

	std::string buf = buffer;
    std::cout << "Received message from " << clients[client_socket - 4].get_Username() << ": " << buffer;
	// -4 car fd[0] == entree standard fd[1] == sortie standar fd[2] == sortie d'erreur fd[3] == fd server

    // Echo message back to client
    send(client_socket, buffer, valread, 0);
}

int main(int argc, char **argv) 
{
	if (argc != 3)
		return (0);

    int server_fd = initServer(atoi(argv[1]));
    std::vector<struct pollfd> pollfds;
    struct pollfd server_pollfd;

    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    pollfds.push_back(server_pollfd);

    while (true) 
	{
        int poll_count = poll(pollfds.data(), pollfds.size(), -1);
        if (poll_count < 0) 
		{
            std::cerr << "Poll error" << std::endl;
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < pollfds.size(); ++i) 
		{
            if (pollfds[i].revents & POLLIN) 
			{
                if (pollfds[i].fd == server_fd) 
				{
                    acceptNewConnection(server_fd);
                    struct pollfd client_pollfd;
                    client_pollfd.fd = clients.back().get_Socket();
                    client_pollfd.events = POLLIN;
                    pollfds.push_back(client_pollfd);
                } 
				else
                	handleClientMessage(pollfds[i].fd);
            }
        }
    }
    close(server_fd);
    return 0;
}
