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
