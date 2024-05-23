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


bool Channel::changeMode(char mode, bool adding)
{
    if (adding)
        _modes[mode] = true;
    else
        _modes[mode] = false;
    return true;
}

std::string Channel::getModes() const
{
    std::string modeStr;
    for (std::map<char, bool>::const_iterator it = _modes.begin(); it != _modes.end(); ++it)
    {
        if (it->second)
            modeStr += it->first;
    }
    return modeStr;
}
