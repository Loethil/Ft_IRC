#pragma once
# include "libs.hpp"

class Clients;

class Channel
{
    private:
       	std::string              			_chanName;
		std::string							_topic;
        std::map<std::string, Clients *>	_connUsers;
        std::vector<std::string>            _invitedUsers;
		bool								_invite;
		bool								_topicMode;
		std::string							_pwd;
		int									_maxUser;
		int									_currentUser;
		std::vector<std::string>			_operator;

    public:
        Channel(std::string & chanName);
		Channel();
        ~Channel();

		void    							notifyChannel(const std::string &message);

        std::string 						getChanName();
		std::map<std::string, Clients *>	&getConnUsers(void);

        std::string                         getTopic();
        void                                setTopic(std::string& topic);

		bool								getInvite(void);
		void								setInvite(bool key);

		bool								getTopicMode(void);
		void								setTopicMode(bool key);

		std::string							getPwd(void);
		void								setPwd(std::string newPwd);

		int									getMaxUser(void);
		void								setMaxUser(int newMaxUser);
		int									getCurrentUser(void);
		void								setCurrentUser(bool key);

        void                                addInvite(const std::string& nickname);
        bool                                isInvited(const std::string& nickname);
		std::vector<std::string>&			getOperator();
		bool								setOperator(Clients *client, bool key, std::string newOp);
		bool								getOpStatus(std::string op);
        std::vector<std::string>&           getInvitedUsers(void);
};

std::ostream	&operator<<(std::ostream &o, Channel &rhs);
