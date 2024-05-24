/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:49:54 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 17:07:36 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Clients.hpp"

Clients::Clients(void) {}

Clients::~Clients(void) {}

int	Clients::get_Socket(void)
{
	return(this->_Socket);
}

void	Clients::set_Socket(int new_Socket)
{
	this->_Socket = new_Socket;
	return ;
}

std::string	Clients::get_Realname(void)
{
	return _Realname;
}

void	Clients::set_Realname(std::string realname)
{
	_Realname = realname;
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

std::vector<Channel *>&		Clients::getCurrConnected(void)
{
	return (this->_currConnected);
}

void		Clients::setCurrConnected(std::vector<Channel *>& newCurrConnected)
{
	this->_currConnected = newCurrConnected;
}

std::ostream	&operator<<(std::ostream &o, Clients &rhs)
{
	o << "Username : " << rhs.get_Username() << std::endl;
	o << "Nickname : " << rhs.get_Nickname() << std::endl;
	o << "Realname : " << rhs.get_Realname() << std::endl;
	o << "Socket : " << rhs.get_Socket() << std::endl;
	o << "status : " << rhs.get_Status() << std::endl;
	//channel map
	return (o);
}