/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 13:01:26 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/21 13:37:32 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Clients.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Input must follow: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    try
    {
        Server server;
        server.setPwd(argv[2]);
        server.start(atoi(argv[1]));
        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
