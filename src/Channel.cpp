#include "Channel.hpp"
#include "Clients.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName), _topic(""), _invite(false), _topicMode(false), _pwd(""), _maxUser(0), _currentUser(0) {}

Channel::Channel():_chanName(""), _topic(""), _invite(false), _topicMode(false), _pwd(""), _maxUser(0), _currentUser(0) {}

Channel::~Channel() {}

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

void	Channel::setInvite(Clients *client, bool key)
{
	if (this->_invite == true && key == false)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(-i) Channel is not in invite only anymore\n";
		notifChan(notifyMsg);
		_invite = key;
	}
	else if (this->_invite == false && key == true)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(+i) Channel is now in invite only\n";
		notifChan(notifyMsg);
		_invite = key;
	}
	else if (this->_invite == true && key == true)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(+i) Channel was already in invite only\n";
		notifChan(notifyMsg);
	}
	else if (this->_invite == false && key == false)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(-i) Channel wasn't in invite only\n";
		notifChan(notifyMsg);
	}
}

bool	Channel::getTopicMode(void)
{
	return (this->_topicMode);
}

void	Channel::setTopicMode(Clients *client, bool key)
{
	if (this->_topicMode == true && key == false)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(-t) Anyone can topic\n";
		notifChan(notifyMsg);
		_topicMode = key;
	}
	else if (this->_topicMode == false && key == true)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(+t) Only ops can topic\n";
		notifChan(notifyMsg);
		_topicMode = key;
	}
	else if (this->_topicMode == true && key == true)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(+t) Already in topic mode\n";
		notifChan(notifyMsg);
	}
	else if (this->_topicMode == false && key == false)
	{
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " :(-t) Already not in topic mode\n";
		notifChan(notifyMsg);
	}
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

int		Channel::getCurrentUser(void)
{
	return (this->_currentUser);
}

void	Channel::setCurrentUser(bool key)
{
	if (key)
		this->_currentUser += 1;
	else if (this->_currentUser > 0)
		this->_currentUser -= 1;
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
    std::map<std::string, Clients*>::iterator connIt = _connUsers.find(newOp);
    if (connIt == _connUsers.end())
    {
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + _chanName + " :This client isn't connected on the channel\n";
        send(client->getSocket(), msg.c_str(), msg.size(), 0);
        return false;
    }
    int newOpSocket = connIt->second->getSocket();
    std::vector<std::string>::iterator it;
    std::string notifyMsg;
    if (key)
    {
        for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
        {
            if (*it == newOp)
            {
                std::string msg = ":I.R.SIUSIU " + _chanName + " :You already are an operator\n";
                send(newOpSocket, msg.c_str(), msg.size(), 0);
                return false;
            }
        }
		this->_operator.push_back(newOp);
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + _chanName + " :You are now an operator\n";
		send(newOpSocket, msg.c_str(), msg.size(), 0);
		std::string notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " +o " + newOp + "\n";
		notifChan(notifyMsg);
		return true;
    }
    else
    {
        for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
        {
            if (*it == newOp)
            {
                this->_operator.erase(it);
                notifyMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU MODE " + _chanName + " -o " + newOp + "\n";
                notifChan(notifyMsg);
				std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + _chanName + " :You are not an operator anymore\n";
                send(newOpSocket, msg.c_str(), msg.size(), 0);
                return true;
            }
        }
		std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@I.R.SIUSIU NOTICE " + _chanName + " :You weren't an operator\n";
        send(newOpSocket, msg.c_str(), msg.size(), 0);
        return false;
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
