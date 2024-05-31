#include "Clients.hpp"

Clients::Clients(void) {}

Clients::~Clients(void)
{
	_Status = status::USERNAME;
	close(_Socket);
}

int	Clients::getSocket(void)
{
	return(this->_Socket);
}

void	Clients::setSocket(int newSocket)
{
	this->_Socket = newSocket;
}

std::string	Clients::getRealname(void)
{
	return _Realname;
}

void	Clients::setRealname(std::string realname)
{
	_Realname = realname;
}

std::string Clients::getNickname(void)
{
	return(this->_Nickname);
}

void	Clients::setNickname(std::string new_Nickname)
{
	this->_Nickname = new_Nickname;
}

std::string	Clients::getUsername(void)
{
	return (this->_Username);
}

void	Clients::setUsername(std::string new_Username)
{
	this->_Username = new_Username;
}

Clients::status		Clients::getStatus(void)
{
	return (this->_Status);
}

void		Clients::setStatus(status new_status)
{
	_Status = new_status;
}

std::vector<Channel *>&		Clients::getCurrConnected(void)
{
	return (this->_currConnected);
}

void	Clients::setCurrConnected(std::vector<Channel *>& newCurrConnected)
{
	this->_currConnected = newCurrConnected;
}

std::ostream	&operator<<(std::ostream &o, Clients &rhs)
{
	o << "Username : " << rhs.getUsername() << std::endl;
	o << "Nickname : " << rhs.getNickname() << std::endl;
	o << "Realname : " << rhs.getRealname() << std::endl;
	o << "Socket : " << rhs.getSocket() << std::endl;
	o << "status : " << rhs.getStatus() << std::endl;
	//channel map
	return (o);
}