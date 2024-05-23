#include "Channel.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName)
{
    // std::cout << "Channel " << chanName << " was created" << std::endl;
}

Channel::Channel(void) {}

Channel::~Channel()
{
    // std::cout << "Channel " << _chanName << " was destroyed" << std::endl;
}

std::string   Channel::getChanName()
{
    return _chanName;
}

std::map<std::string, Clients *>	&Channel::get_connUsers(void)
{
	return (this->_connUsers);
}
std::string Channel::getTopic()
{
	return _topic;
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}