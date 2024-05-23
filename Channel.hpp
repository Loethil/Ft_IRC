#pragma once
# include "libs.hpp"
# include "Clients.hpp"

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_topic;
        std::map<std::string, Clients *>	_connUsers;
        std::map<char, bool>                _modes;

    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

        std::string 						getChanName();
		std::map<std::string, Clients *>	&get_connUsers(void);

        void                                setTopic(std::string topic);
        std::string                         getTopic();

        bool                                changeMode(char mode, bool adding);
        std::string                         getModes() const;
};
