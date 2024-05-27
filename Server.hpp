/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:24:31 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/27 14:08:37 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "libs.hpp"
# include "Clients.hpp"
# include "Channel.hpp"

#define MAX_CLIENTS 10
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
        int                                 _server_fd;
        int                                 _port;
        std::string                         _pwd;
        std::string                         _server_name;
        std::map<int , Clients *>           _clients;
        struct sockaddr_in                  _cli_adr;
        struct sockaddr_in                  _serv_adr;
		std::map<std::string, Channel>		_Channel;

    public:

        Server();
        ~Server();
        void    start(int port);
        void    acceptNewConnection();
        void	handleClientMessage(int client_socket, Clients::status status);
        void    sendWelcomeMessages(int client_socket, Clients *client);
		void	user(Clients *client, std::istringstream &lineStream, int client_socket);
		void	nick(Clients *client, std::istringstream &lineStream);
		bool	pass(Clients *client, std::istringstream &lineStream, int client_socket);
		void	join(Clients *client, std::istringstream &lineStream, int client_socket);
		void 	msg(Clients *client, std::istringstream &lineStream, int client_socket, std::map<int, Clients *> &_clients);
        void    run();
        void    part(Clients *client, std::istringstream &lineStream);
        void    topic(Clients *client, std::istringstream &lineStream, int client_socket);

        std::string getPwd();
        

        void    setPwd(std::string pwd);
        void    setPort(int port);
};