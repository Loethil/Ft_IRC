/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:50:02 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 17:26:58 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		int						get_Socket(void);
		void					set_Socket(int	new_Socket);
		std::string 			get_Nickname(void);
		void					set_Nickname(std::string new_Nickname);
		std::string				get_Username(void);
		void					set_Username(std::string new_Username);
		std::string				get_Realname(void);
		void					set_Realname(std::string new_Realname);
		status					get_Status(void);
		void					set_Status(status new_Status);
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
