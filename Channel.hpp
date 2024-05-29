#pragma once
# include "libs.hpp"

class Clients;

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_topic;
        std::map<std::string, Clients *>	_connUsers;
		bool								_mode[4];
        std::vector<std::string>            _invitedUsers;

    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

        std::string 						getChanName();
		std::map<std::string, Clients *>	&getConnUsers(void);

        void                                setTopic(std::string& topic);
        std::string                         getTopic();

		void								setMode(bool new_Mode, int i);
		bool								getMode(int i);
        void                                addInvite(const std::string& nickname);
        bool                                isInvited(const std::string& nickname);
};

std::ostream	&operator<<(std::ostream &o, Channel &rhs);
