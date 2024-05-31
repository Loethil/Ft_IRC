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
