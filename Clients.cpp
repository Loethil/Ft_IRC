#include "Clients.hpp"

Clients::Clients(void)
{

}

Clients::~Clients(void)
{

}

int	Clients::get_Socket(void)
{
	return(this->_Socket);
}

void	Clients::set_Socket(int new_Socket)
{
	this->_Socket = new_Socket;
	return ;
}

std::string Clients::get_Nickname(void)
{
	return(this->_Nickname);
}

void	Clients::set_Nickname(std::string new_Nickname)
{
	this->_Nickname = new_Nickname;
	return ;
}

std::string	Clients::get_Username(void)
{
	return (this->_Username);
}

void	Clients::set_Username(std::string new_Username)
{
	this->_Username = new_Username;
	return ;
}

Clients::status		Clients::get_Status(void)
{
	return (this->_Status);
}

void		Clients::set_Status(status new_status)
{
	this->_Status = new_status;
	return ;
}