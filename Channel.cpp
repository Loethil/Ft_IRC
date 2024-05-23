#include "Channel.hpp"

Channel::Channel(std::string & chanName): _chanName(chanName)
{
    // std::cout << "Channel " << chanName << " was created" << std::endl;
}

Channel::Channel() {}

Channel::~Channel()
{
    // std::cout << "Channel " << _chanName << " was destroyed" << std::endl;
}

std::string   Channel::getChanName()
{
    return _chanName;
}

std::string	Channel::getTopic()
{
	return (this->_topic);
}

void	Channel::setTopic(std::string new_topic)
{
	this->_topic = new_topic;
	return ;
}

std::map<std::string, Clients *>	&Channel::getConnUsers()
{
	return (this->_connUsers);
}
