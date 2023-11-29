/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao--r <abaiao--r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:33:53 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/03 16:22:04 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <netinet/in.h>
#include "colours.hpp"

class Client
{
	private:
		int			_client_fd;
		bool		_authenticated;
		bool		_registered;
		int			_tries;
		int			_strikes;
		std::string _nickname; // can not be repeated
		std::string _username; // can be repeated
		bool		_is_admin;
		std::string	_cmd_buffer;

	public:
		Client(void);
		Client(const Client &copy);
		~Client(void);
		Client 	&operator=(const Client &copy);
		
		//public variable 
		sockaddr_in	_client_addr;

		//getters and setters
		const int &get_client_fd(void) const;
		void set_client_fd(const int &value);
		
		const bool &get_registered(void) const;
		void set_registered(const bool &value);

		const int &get_strikes(void) const;
		void set_strikes(const int &value);
		
		const bool &get_authenticated(void) const;
		void set_authenticated(const bool &value);
		
		const std::string &get_nickname(void) const;
		void set_nickname(const std::string &value);
		
		const std::string &get_username(void) const;
		void set_username(const std::string &value);
		
		const bool &get_is_admin(void) const;
		void set_is_admin(const bool &value);

		std::string	get_cmd() const;
		void	add_to_cmd(std::string str);
		void	clear_cmd();
		
		//operators
		bool	operator==(const Client &comp);
		bool	operator==(const std::string &nick);
		bool	operator==(const int &comp);
	
		//other methods
		int		pass_counter(int flag, int val);
};
