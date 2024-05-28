#pragma once
# include "libs.hpp"

class Clients;

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_topic;
        std::map<std::string, Clients *>	_connUsers;
		bool								_invit;
		bool								_topic_mode;
		std::string							_pwd;
		int									_max_user;
		std::vector<std::string>			_operator;

    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

        std::string 						getChanName();
		std::map<std::string, Clients *>	&getConnUsers(void);

        std::string                         getTopic();
        void                                setTopic(std::string& topic);

		bool								get_invit(void);
		void								set_invit(bool key);

		bool								get_topic_mode(void);
		void								set_topic_mode(bool key);

		std::string							get_pwd(void);
		void								set_pwd(std::string new_pwd);

		int									get_max_user(void);
		void								set_max_user(int new_max_user);

		bool								get_operator(std::string op);
		bool								set_operator(std::string new_op, bool key);
};

std::ostream	&operator<<(std::ostream &o, Channel &rhs);
