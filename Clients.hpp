/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:50:02 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/16 20:51:55 by scarpent         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "libs.hpp"

class Clients
{
	public:
		Clients(void);
		~Clients(void);
		typedef enum		status
		{
			PASSWORD,
			USERNAME,
			NICKNAME,
			COMPLETED,
		}					status;
		int			get_Socket(void);
		void		set_Socket(int new_Socket);
		std::string get_Nickname(void);
		void		set_Nickname(std::string new_Nickname);
		std::string get_Username(void);
		void		set_Username(std::string new_Username);
		status		get_Status(void);
		void		set_Status(status new_status);
	private:
		int			_Socket;
		std::string _Nickname;
		std::string _Username;
		status		_Status;
};