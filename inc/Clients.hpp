#pragma once
# include "libs.hpp"
# include "Channel.hpp"

class Clients
{
	public:
		Clients(void);
		~Clients(void);
		std::string			partialData;
		typedef enum		status
		{
			USERNAME,
			COMPLETED,
		}					status;
		int						getSocket(void);
		void					setSocket(int	newSocket);
		std::string 			getNickname(void);
		void					setNickname(std::string newNickname);
		std::string				getUsername(void);
		void					setUsername(std::string newUsername);
		std::string				getRealname(void);
		void					setRealname(std::string newRealname);
		status					getStatus(void);
		void					setStatus(status newStatus);
		std::vector<Channel *>& getCurrConnected(void);
		void					setCurrConnected(std::vector<Channel *>& newCurrConnected);
	private:
		int						_Socket;
		std::string 			_Nickname;
		std::string 			_Username;
		std::string				_Realname;
		status					_Status;
		std::vector<Channel *>	_currConnected;
};

std::ostream	&operator<<(std::ostream &o, Clients &rhs);
