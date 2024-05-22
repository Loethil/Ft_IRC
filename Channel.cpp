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

std::string	Channel::get_Topic(void)
{
	return (this->_Topic);
}

void	Channel::set_Topic(std::string new_Topic)
{
	this->_Topic = new_Topic;
	return ;
}

std::map<std::string, Clients *>	&Channel::get_connUsers(void)
{
	return (this->_connUsers);
}
