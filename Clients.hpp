/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llaigle <llaigle@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:50:02 by llaigle           #+#    #+#             */
/*   Updated: 2024/05/23 16:41:51 by llaigle          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "libs.hpp"

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
		int			get_Socket(void);
		void		set_Socket(int new_Socket);
		std::string get_Nickname(void);
		void		set_Nickname(std::string new_Nickname);
		std::string get_Username(void);
		void		set_Username(std::string new_Username);
		std::string	get_Realname(void);
		void		set_Realname(std::string new_Realname);
		status		get_Status(void);
		void		set_Status(status new_Status);
		std::string get_Channel(void);
		void		set_Channel(std::string new_Channel);
		
	private:
		int						_Socket;
		std::string 			_Nickname;
		std::string 			_Username;
		std::string				_Realname;
		status					_Status;
		std::string 			_channel;
		///stocker les modes du client dans un tableau / conteneur
};