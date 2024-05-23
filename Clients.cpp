/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:49:54 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 16:27:25 by llaigle          ###   ########.fr       */
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

std::string		Clients::get_Channel(void)
{
	return (this->_channel);
}

void		Clients::set_Channel(std::string new_Channel)
{
	this->_channel = new_Channel;
}

bool Clients::changeMode(char mode, bool adding)
{
    if (adding)
        _modes[mode] = true;
    else
        _modes[mode] = false;
    return true;
}

std::string Clients::getModes() const
{
    std::string modeStr;
    for (std::map<char, bool>::const_iterator it = _modes.begin(); it != _modes.end(); ++it)
    {
        if (it->second)
            modeStr += it->first;
    }
    return modeStr;
}