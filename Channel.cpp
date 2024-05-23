#include "Channel.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName), _topic("")
{
    // std::cout << "Channel " << chanName << " was created" << std::endl;
}

Channel::Channel():_chanName(""), _topic("") {}

Channel::~Channel()
{
    // std::cout << "Channel " << _chanName << " was destroyed" << std::endl;
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

void	Channel::setTopic(std::string new_topic)
{
	_topic = new_topic;
}