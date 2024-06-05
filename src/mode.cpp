#include "Server.hpp"

void	Server::caseK(bool type, Clients *client, std::istringstream &lineStream, std::vector<Channel *>::iterator currIt)
{
	if (type)
	{
		std::string pwd;
		if (lineStream >> pwd)
		{
			(*currIt)->setPwd(pwd);
			std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + (*currIt)->getChanName() + " :(+k) Channel needs the password\n";
			(*currIt)->notifChan(notifyMsg);
		}
		else
		{
			std::string errMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :No password was given for mode (+k)\n";
			send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
		}
	}
	else
	{
		(*currIt)->setPwd("");
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + (*currIt)->getChanName() + " :(-k) Channel doesn't need a password anymore\n";
		(*currIt)->notifChan(notifyMsg);
	}
}

void	Server::caseO(bool type, Clients *client, std::istringstream &lineStream, std::vector<Channel *>::iterator currIt, std::string chan)
{
	std::string newOp;
	if (lineStream >> newOp)
	{
		for (std::vector<std::string>::iterator opIt = _Channel[chan]->getOperator().begin(); opIt != _Channel[chan]->getOperator().end(); ++opIt)
		{
			if ((*opIt).compare(client->getNickname()) == 0)
			{
				(*currIt)->setOperator(client, type, newOp);
				return ;
			}
		}
		std::string errMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + chan + " :You don't have the privilege to give operator rights\n";
		send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
	}
	else
	{
		std::string errMsg = (type ? "No operator specified for +o mode\n" : "No operator specified for -o mode\n");
		send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
	}
}

void	Server::mode(Clients *client, std::istringstream &lineStream)
{
	std::string chan;
	std::string mode;
	std::vector<Channel *>::iterator currIt;

	lineStream >> chan >> mode;
	for (currIt = client->getCurrConnected().begin(); currIt != client->getCurrConnected().end(); ++currIt)
	{
		if (chan == (*currIt)->getChanName())
		{
			if ((*currIt)->getConnUsers().find(client->getNickname()) == (*currIt)->getConnUsers().end())
			{
				std::string errMsg = ":I.R.SIUSIU 401 " + client->getNickname() + " " + chan + " :" RED "No such user\n" RESET;
				send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
				return ;
			}
			break ;
		}
	}
	if (currIt == client->getCurrConnected().end() && mode != "+o")
	{
		if (chan == client->getNickname())
			return ;
		std::string errMsg = "This channel does not exist\n";
		send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
		return ;
	}
	if ((*currIt)->getOpStatus(client->getNickname()))
	{
		if (mode[0] == '+' || mode[0] == '-')
		{
			bool type = (mode[0] == '+') ? true : false;
			for (size_t i = 1; i < mode.size(); ++i)
			{
				char modeChar = mode[i];
				switch (modeChar)
				{
					case 'i':
						(*currIt)->setInvite(client, type);
						break;
					case 't':
						(*currIt)->setTopicMode(client, type);
						break;
					case 'k':
						caseK(type, client, lineStream, currIt);
						break;
					case 'o':
						caseO(type, client, lineStream, currIt, chan);
						break;
					case 'l':
						if (type)
						{
							int userLimit;
							lineStream >> userLimit;
							if (lineStream.fail())
							{
								std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :No limit specified for (+l) mode\n";
								send(client->getSocket(), msg.c_str(), msg.size(), 0);
								lineStream.clear();
							}
							else
							{
								std::stringstream userLimitStr;
								userLimitStr << userLimit;
								std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :Limit of max users in this channel set to ";
								msg += userLimitStr.str() + " (+l) \n";
								(*currIt)->setMaxUser(userLimit);
								(*currIt)->notifChan(msg);
							}
						}
						else
						{
							std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :Channel set to no max users limit \n";
							(*currIt)->setMaxUser(0);
							(*currIt)->notifChan(msg);
						}
						break;
					default:
					{
						std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :This mode option isn't handled\n";
						send(client->getSocket(), msg.c_str(), msg.size(), 0);
					}
					break;
				}
			}
		}
	}
	else
	{
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + (*currIt)->getChanName() + " :You're not an operator\n";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
	}
}

