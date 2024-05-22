#include "Channel.hpp"

Channel::Channel(const std::string & chanName): _chanName(chanName)
{
    std::cout << "Channel " << chanName << " was created" << std::endl;
}

Channel::~Channel()
{
    std::cout << "Channel " << _chanName << " was destroyed" << std::endl;
}

const std::string   Channel::getChanName()
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
