#include "Channel.hpp"
#include "Clients.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName), _topic(""), _invite(false), _topicMode(false), _pwd(""), _maxUser(0) {}

Channel::Channel():_chanName(""), _topic(""), _invite(false), _topicMode(false), _pwd(""), _maxUser(0) {}

Channel::~Channel()
{
	if (!_connUsers.empty())
	{
		for (std::map<std::string, Clients *>::iterator delIt = _connUsers.begin(); delIt != _connUsers.end(); ++delIt)
			delete delIt->second;
		_connUsers.clear();
	}
}

std::string   Channel::getChanName()
{
    return _chanName;
}

std::map<std::string, Clients *>	&Channel::getConnUsers(void)
{
	return (this->_connUsers);
}

std::string Channel::getTopic()
{
	return _topic;
}

void	Channel::setTopic(std::string& newTopic)
{
	_topic = newTopic;
}

bool	Channel::getInvite(void)
{
	return (this->_invite);
}

void	Channel::setInvite(bool key)
{
	if (this->_invite == true && key == false)
		std::cout << this->getChanName() << " is set on 'no invite only'" << std::endl;
	else if (this->_invite == false && key == true)
		std::cout << this->getChanName() << " is set on 'invite only'" << std::endl;
	else if (this->_invite == true && key == true)
	{
		std::cout << this->getChanName() << " is already set on 'invite only'" << std::endl;
		return ;
	}
	else if (this->_invite == false && key == false)
	{
		std::cout << this->getChanName() << " is already set on 'no invite only'" << std::endl;
		return ;
	}
	this->_invite = key;
}

bool	Channel::getTopicMode(void)
{
	return (this->_topicMode);
}

void	Channel::setTopicMode(bool key)
{
	if (this->_topicMode == true && key == false)
		std::cout << this->getChanName() << " is set on 'topic for operator only'" << std::endl;
	else if (this->_topicMode == false && key == true)
		std::cout << this->getChanName() << " is set on 'topic for everyone'" << std::endl;
	else if (this->_topicMode == true && key == true)
	{
		std::cout << this->getChanName() << " is already set on 'topic for operator only'" << std::endl;
		return ;
	}
	else if (this->_topicMode == false && key == false)
	{
		std::cout << this->getChanName() << " is already set on 'topic for everyone'" << std::endl;
		return ;
	}
	this->_topicMode = key;
}

std::string		Channel::getPwd(void)
{
	return (this->_pwd);
}

void	Channel::setPwd(std::string newPwd)
{
	this->_pwd = newPwd;
}

int		Channel::getMaxUser(void)
{
	return (this->_maxUser);
}

void	Channel::setMaxUser(int newMaxUser)
{
	this->_maxUser = newMaxUser;
}

bool	Channel::getOpStatus(std::string op)
{
	std::vector<std::string>::iterator it;
	for (it = this->_operator.begin(); it != this->_operator.end(); ++it)
	{
		if ((*it) == op)
			return (true); // the user is an operator
	}
	return (false);
}

std::vector<std::string>&	Channel::getOperator()
{
	return _operator;
}

bool Channel::setOperator(Clients *client, bool key, std::string newOp)
{
    std::vector<std::string>::iterator it;
	std::map<std::string, Clients*>::iterator connIt = _connUsers.find(newOp);
	if (connIt == _connUsers.end())
	{
		std::string msg = ":I.R.SIUSIU PRIVMSG " + _chanName + " :This client isn't connected on the channel\n";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		return false;
	}
	int	newOpSocket = connIt->second->getSocket();
	if (key)
	{
		for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
		{
			if (*it == newOp)
			{
				std::string msg = ":I.R.SIUSIU PRIVMSG " + _chanName + " :You already are an operator\n";
				send(newOpSocket, msg.c_str(), msg.size(), 0);
				return false; // user already operator
			}
		}
		this->_operator.push_back(newOp);
		std::string msg = ":I.R.SIUSIU PRIVMSG " + _chanName + " :You now are an operator\n";
		send(newOpSocket, msg.c_str(), msg.size(), 0);
		return true; // the user is successfully added
	}
	else
	{
		for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
		{
			if (*it == newOp)
			{
				this->_operator.erase(it);
				std::string msg = ":I.R.SIUSIU PRIVMSG " + _chanName + " :You are not an operator anymore\n";
				send(newOpSocket, msg.c_str(), msg.size(), 0);
				return true; // user has been removed from operators
			}
		}
		std::string msg = ":I.R.SIUSIU PRIVMSG " + _chanName + " :You weren't an operator\n";
		send(newOpSocket, msg.c_str(), msg.size(), 0);
		return false; // the user was not an operator
	}
}

std::ostream	&operator<<(std::ostream &o, Channel &rhs)
{
	o << "ChanName : " << rhs.getChanName() << std::endl;
	o << "Topic : " << rhs.getTopic() << std::endl;
	for (std::map<std::string, Clients*>::iterator it = rhs.getConnUsers().begin(); it != rhs.getConnUsers().end(); ++it)
	{
		o << "Connected Users on this channel : " << it->second->getNickname() << std::endl;
	}
	return (o);
}

void Channel::addInvite(const std::string& nickname)
{
    _invitedUsers.push_back(nickname);
}

bool Channel::isInvited(const std::string& nickname)
{
    return std::find(_invitedUsers.begin(), _invitedUsers.end(), nickname) != _invitedUsers.end();
}

std::vector<std::string>	&Channel::getInvitedUsers()
{
	return (_invitedUsers);
}