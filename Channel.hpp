#pragma once
# include "libs.hpp"
# include "Clients.hpp"

class Channel
{
    private:
        const std::string               _chanName;
		std::string						_Topic;
        std::map<std::string, Clients*>	_connUsers;
    public:
        Channel(const std::string & chanName);
        ~Channel();

        const std::string 	getChanName();
		std::string			get_Topic(void);
		void				set_Topic(std::string new_Topic);
};
