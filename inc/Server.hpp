#pragma once
# include "libs.hpp"
# include "Clients.hpp"
# include "Channel.hpp"

#define BUFFER_SIZE 1024

// ANSI escape codes for text colors
#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

class Server
{
    private:
        int                                 _serverFd;
        int                                 _port;
        std::string                         _pwd;
        std::string                         _serverName;
        std::map<int , Clients *>           _clients;
        struct sockaddr_in                  _clientAdr;
        struct sockaddr_in                  _serverAdr;
		std::map<std::string, Channel *>		_Channel;

    public:

        Server();
        ~Server();
        void    start(int port);
        void    acceptNewConnection();
        void	handleClientMessage(int client_socket, Clients::status status);
        void    sendWelcomeMessages(int client_socket, Clients *client);
		void	user(Clients *client, std::istringstream &lineStream);
		void	nick(Clients *client, std::istringstream &lineStream);
		bool	pass(Clients *client, std::istringstream &lineStream);
		void	join(Clients *client, std::istringstream &lineStream);
        void    msg(Clients *client, std::istringstream &lineStream, char *buffer);
        void    run();
        void    part(Clients *client, std::istringstream &lineStream);
        void    part(Clients *client, std::string channelName);
		void	part(Clients *client);
        void    topic(Clients *client, std::istringstream &lineStream);
        void	invite(Clients *client, std::istringstream &lineStream);
        void	joinChannel(Clients *client, std::string channelName);
        void	quit(Clients *client, std::istringstream &lineStream);
		void	pong(Clients *client);
        void	kick(Clients *client, std::istringstream &lineStream);
		
        void	mode(Clients *client, std::istringstream &lineStream);
        void	caseK(bool type, Clients *client, std::istringstream &lineStream, std::vector<Channel *>::iterator currIt);
        void	caseO(bool type, Clients *client, std::istringstream &lineStream, std::vector<Channel *>::iterator currIt, std::string chan);

        std::string getPwd();

        void    setPwd(std::string pwd);
        void    setPort(int port);
};