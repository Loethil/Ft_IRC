#pragma once
# include "libs.hpp"
# include "Clients.hpp"

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_topic;
        std::map<std::string, Clients *>	_connUsers;

    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

        std::string 						getChanName();
		std::map<std::string, Clients *>	&getConnUsers(void);

        void                                setTopic(std::string topic);
        std::string                         getTopic();
};

std::ostream	&operator<<(std::ostream &o, Clients &rhs);
