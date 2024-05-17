#pragma once
#include "libs.hpp"
#include "Clients.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

class Server
{
    private:
        int                                 _server_fd;
        int                                 _port;
        std::string                         _pwd;
        //std::vector<Clients>                _clients;
        std::map<int , Clients>             _clients;
        struct sockaddr_in                  _cli_adr;
        struct sockaddr_in                  _serv_adr;

    public:

        Server();
        ~Server();
        void    start(int port);
        void    acceptNewConnection();
        void	handleClientMessage(int client_socket, Clients::status status);
        void    run();

        std::string getPwd();

        void    setPwd(std::string pwd);
        void    setPort(int port);
};