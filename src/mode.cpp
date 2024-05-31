#include "Server.hpp"

void	Server::caseK(bool type, Clients *client, std::istringstream &lineStream, std::vector<Channel *>::iterator currIt)
{
	if (type)
	{
		std::string pwd;
		if (lineStream >> pwd)
		{
			if (!pwd.empty())
			{
				(*currIt)->setPwd(pwd);
				std::cout << BLUE << (*currIt)->getPwd() << RESET << std::endl;
			}
		}
		else
		{
			std::string errMsg = "No password specified for +k mode\n";
			send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
		}
	}
	else
		(*currIt)->setPwd("");
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
		std::string errMsg = ":I.R.SIUSIU PRIVMSG " + chan + " :You don't have the privilege to give operator rights\n";;
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
	std::cout << "chan : " << chan << std::endl;
	std::cout << "mode : " << mode << std::endl;
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
	if (mode[0] == '+' || mode[0] == '-')
	{
		bool type = (mode[0] == '+') ? true : false;
		for (size_t i = 1; i < mode.size(); ++i)
		{
			char modeChar = mode[i];
			switch (modeChar)
			{
				case 'i':
					(*currIt)->setInvite(type);
					break;
				case 't':
					(*currIt)->setTopicMode(type);
					break;
				case 'k':
					caseK(type, client, lineStream, currIt);
					break;
				case 'o':
					caseO(type, client, lineStream, currIt, chan);
					break;
				// case 'l':
				// 	if (type)
				// 	{
				// 		int userLimit;
				// 		lineStream >> userLimit;
				// 		if (lineStream.fail())
				// 		{
				// 			std::string errMsg = "No user limit specified for +l mode\n";
				// 			send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
				// 			lineStream.clear();
				// 		}
				// 		else
				// 			(*currIt)->setUserLimit(userLimit);
				// 	}
				// 	else
				// 		(*currIt)->setUserLimit(0);
				// 	break;
				default:
				{
					std::string errMsg = "This option is not handled, see IRC subject\n";
					send(client->getSocket(), errMsg.c_str(), errMsg.length(), 0);
				}
				break;
			}
		}
	}
}
