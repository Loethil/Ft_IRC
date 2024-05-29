#include "Channel.hpp"
#include "Clients.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName), _topic("") {}

Channel::Channel():_chanName(""), _topic("") , _invit(false), _topic_mode(false) {}

Channel::~Channel()
{
	if (!_connUsers.empty())
	{
		for (std::map<std::string, Clients *>::iterator delIt = _connUsers.begin(); delIt != _connUsers.end(); ++delIt)
		{
			delete delIt->second;
		}
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

void	Channel::setTopic(std::string& new_topic)
{
	_topic = new_topic;
}

bool	Channel::get_invit(void)
{
	return (this->_invit);
}

void	Channel::set_invit(bool key)
{
	if (this->_invit == true && key == false)
		std::cout << this->getChanName() << " is set on 'invit only'" << std::endl;
	else if (this->_invit == false && key == true)
		std::cout << this->getChanName() << " is set on 'no invit only'" << std::endl;
	else if (this->_invit == true && key == true)
	{
		std::cout << this->getChanName() << " is already set on 'invit only'" << std::endl;
		return ;
	}
	else if (this->_invit == false && key == false)
	{
		std::cout << this->getChanName() << " is already set on 'no invit only'" << std::endl;
		return ;
	}
	this->_invit = key;
}

bool	Channel::get_topic_mode(void)
{
	return (this->_topic_mode);
}

void	Channel::set_topic_mode(bool key)
{
	if (this->_topic_mode == true && key == false)
		std::cout << this->getChanName() << " is set on 'topic for operator only'" << std::endl;
	else if (this->_topic_mode == false && key == true)
		std::cout << this->getChanName() << " is set on 'topic for everyone'" << std::endl;
	else if (this->_topic_mode == true && key == true)
	{
		std::cout << this->getChanName() << " is already set on 'topic for operator only'" << std::endl;
		return ;
	}
	else if (this->_topic_mode == false && key == false)
	{
		std::cout << this->getChanName() << " is already set on 'topic for everyone'" << std::endl;
		return ;
	}
	this->_topic_mode = key;
}

std::string		Channel::get_pwd(void)
{
	return (this->_pwd);
}

void	Channel::set_pwd(std::string new_pwd)
{
	this->_pwd = new_pwd;
}

int		Channel::get_max_user(void)
{
	return (this->_max_user);
}

void	Channel::set_max_user(int new_max_user)
{
	this->_max_user = new_max_user;
}

bool	Channel::get_operator(std::string op)
{
	std::vector<std::string>::iterator it;
	for (it = this->_operator.begin(); it != this->_operator.end(); ++it)
	{
		if ((*it) == op)
			return (true); // the user is an operator
	}
	return (false);
}

bool Channel::set_operator(std::string new_op, bool key)
{
    std::vector<std::string>::iterator it;
	if (key) 
	{
		for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
		{
			if (*it == new_op)
			{
				std::cout << "User " << new_op << " is already an operator" << std::endl;
				return false; // user already operator
			}
		}
		this->_operator.push_back(new_op);
		std::cout << "User " << new_op << " is now operator" << std::endl;
		return true; // the user is successfully added
	}
	else 
	{
		for (it = this->_operator.begin(); it != this->_operator.end(); ++it) 
		{
			if (*it == new_op) 
			{
				this->_operator.erase(it);
				std::cout << "User " << new_op << " isn't an operator anymore" << std::endl;
				return true; // user has been removed from operators
			}
		}
		std::cout << "User " << new_op << " was not an operator" << std::endl;
		return false; // the user was not an operator
	}
}

std::ostream	&operator<<(std::ostream &o, Channel &rhs)
{
	o << "ChanName : " << rhs.getChanName() << std::endl;
	o << "Topic : " << rhs.getTopic() << std::endl;
	for (std::map<std::string, Clients*>::iterator it = rhs.getConnUsers().begin(); it != rhs.getConnUsers().end(); ++it)
	{
		o << "Connected Users on this channel : " << it->second->get_Nickname() << std::endl;
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
