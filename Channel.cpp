#include "Channel.hpp"
#include "Clients.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName), _topic("") {}

Channel::Channel():_chanName(""), _topic("") {}

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