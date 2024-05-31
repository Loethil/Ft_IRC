#include "Server.hpp"

void    Channel::notifyChannel(const std::string &message)
{
    for (std::map<std::string, Clients *>::iterator it = _connUsers.begin(); it != _connUsers.end(); ++it)
    {
        send(it->second->getSocket(), message.c_str(), message.length(), 0);
    }
}