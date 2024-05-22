#pragma once
# include "libs.hpp"
# include "Clients.hpp"

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_Topic;
        std::map<std::string, Clients *>	_connUsers;
    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

        std::string 						getChanName();
		std::string							get_Topic(void);
		void								set_Topic(std::string new_Topic);
		std::map<std::string, Clients *>	&get_connUsers(void);
};
