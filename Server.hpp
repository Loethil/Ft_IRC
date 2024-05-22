/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:24:31 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/22 14:31:11 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "libs.hpp"
# include "Clients.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

class Server
{
    private:
        int                                 _server_fd;
        int                                 _port;
        std::string                         _pwd;
        std::string                         _server_name;
        std::map<int , Clients>             _clients;
        struct sockaddr_in                  _cli_adr;
        struct sockaddr_in                  _serv_adr;

    public:

        Server();
        ~Server();
        void    start(int port);
        void    acceptNewConnection();
        void	handleClientMessage(int client_socket, Clients::status status);
        void    sendWelcomeMessages(int client_socket, Clients &client);
		void	user(Clients &client, std::istringstream &lineStream, int client_socket);
		void	nick(Clients &client, std::istringstream &lineStream);
		bool	pass(Clients &client, std::istringstream &lineStream, int client_socket);
		void	join(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> & _clients);
		void	msg(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients>  _clients);
        void    run();
        void    part(Clients &client, std::istringstream &lineStream, int client_socket, std::map<int, Clients> & _clients);


        // class bindException : public std::exception
        // {
        // public:
        //     virtual const char *what() const throw();
        // };

        std::string getPwd();

        void    setPwd(std::string pwd);
        void    setPort(int port);
};