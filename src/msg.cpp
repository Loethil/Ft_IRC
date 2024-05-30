#include "Server.hpp"

void Server::msg(Clients *client, std::istringstream &lineStream, char *buffer)
{
    std::string dest;
    std::string msg = buffer;

    std::cerr << "Msg command received from client: " << client->getNickname() << std::endl;

    if (lineStream >> dest)
    {
        if (dest.find("#") < dest.size())
        {
            if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
                msg.erase(0, 2);
            std::string sent_msg;
			std::vector<Channel *>::iterator currIt;
            for (currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
            {
                if (dest != (*currIt)->getChanName())
					continue ;
				sent_msg = ":" + client->getNickname() + " " + msg;
				for (std::map<std::string, Clients *>::iterator it = (*currIt)->getConnUsers().begin(); it != (*currIt)->getConnUsers().end(); ++it)
				{
					if (it->second->getSocket() != client->getSocket())
						send(it->second->getSocket(), sent_msg.c_str(), sent_msg.length(), 0);
				}
				return ;
            }
			if (currIt == client->getCurrConnected().end())
			{
				std::string send_msg = ":I.R.SIUSIU 442 " + client->getNickname() + dest + " :" RED "You aren't connected to that channel.\n" RESET;
				send(client->getSocket(), send_msg.c_str(), send_msg.size(), 0);
			}
        }
        else
        {
            if (!msg.empty() && msg[0] == ' ' && msg[1] == ':')
                msg.erase(0, 2);
            std::string send_msg;
            for (std::map<int, Clients*>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter)
            {
                if (iter->second->getNickname() == dest)
                {
                    send_msg = ":" + client->getNickname() + " " + msg;
                    send(iter->second->getSocket(), send_msg.c_str(), send_msg.size(), 0);
                    return ;
                }
            }
            std::string errMsg = ":I.R.SIUSIU 401 " + client->getNickname() + " " + dest + " :" RED "No such user\n" RESET;
            send(client->getSocket(), errMsg.c_str(), errMsg.size(), 0);
        }
    }
}
