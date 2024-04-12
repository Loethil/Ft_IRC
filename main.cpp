# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <sys/poll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <stdio.h>
# include <stdlib.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
    struct sockaddr_in    server_addr;
    int                 socket_server    = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_addr.s_addr = INADDR_ANY; // IP -> 10.11.7.10
    server_addr.sin_family         = AF_INET;
    server_addr.sin_port         = htons(atoi(argv[1]));
    if (bind(socket_server, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    listen(socket_server, 5);
    int fds = 0;
    int numfds = 1;
    struct    pollfd  pollfds[5];
    pollfds->fd = socket_server;
    pollfds->events        = POLLIN;
    pollfds->revents        = 0;
    while(true)
    {
        fds = numfds;
        if (poll(pollfds, fds, -1) == -1)                /*--Watch-the-registered-fd--*/
            perror("poll");
        for (nfds_t fd = 0; fd <= fds; fd++)
        {
            if (pollfds[fd].fd <= 0)                                                /*--Ignore-unusable-fd--*/
                continue;
            if ((pollfds[fd].revents & POLLIN) == POLLIN)                        /*--If-an-event-is-catch--*/
            {
                if (pollfds[fd].fd == pollfds[0].fd)                        /*--New-connection-event--*/
                    /**/; //accept la co et update pollfds
                else                                                                    /*--communication-event--*/
                    /**/; // recv msg et le parse / le renvoyer
            }
        }
    }
}