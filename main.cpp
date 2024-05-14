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


int main(int argc, char **argv)
{
    struct sockaddr_in    server_addr;
    int                 socket_server    = socket(AF_INET, SOCK_STREAM, 0);

	if (argc != 3)
		return (0);
    server_addr.sin_addr.s_addr = INADDR_ANY; // IP -> 10.11.7.10
    server_addr.sin_family         = AF_INET;
    server_addr.sin_port         = htons(atoi(argv[2]));
    if (bind(socket_server, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    listen(socket_server, 5);
   	unsigned long numfds = 5;
    struct    pollfd  pollfds[5];
    pollfds->fd = socket_server;
    pollfds->events        = POLLIN;
    pollfds->revents        = 0;
    while(true)
    {
        if (poll(pollfds, numfds, -1) == -1)                /*--Watch-the-registered-fd--*/
            perror("poll");
        for (nfds_t fd = 0; fd <= numfds; fd++)
        {
            if (pollfds[fd].revents & POLLIN)                        /*--If-an-event-is-catch--*/
            {
                if (pollfds[fd].fd == pollfds->fd)		/*--New-connection-event--*/
                {
                    int new_socket = accept(socket_server, NULL, NULL);
                    if (new_socket >= 0)
                    {
                        std::cout << "New connection accepted" << std::endl;
                        for (unsigned long i = 1; i < numfds; i++)
                        {
                            if (pollfds[i].fd == -1)
                            {
                                pollfds[i].fd = new_socket;
                                pollfds[i].events = POLLIN;
								pollfds[i].revents = 0;
                                break;
                            }
                        }
                    }
                }
                else                                                 /*--communication-event--*/
                {
				    std::cout << "message send" << std::endl;
				}
            }
        }
    }
}