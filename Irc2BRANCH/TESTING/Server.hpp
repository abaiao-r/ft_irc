/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/12/05 14:07:32 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "ServerConnection.hpp"
#include "ServerUtils.hpp"
#include "colours.hpp"

class Client;
class Channel;
class ServerConnection;

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5
#define MAX_RETRIES 3

class Server: public ServerConnection
{
	private:
		int						_port;
		int						_server_fd;
		const std::string		_password;
		sockaddr_in				_address;
		Server(void);
		Server(const Server &src);
		Server &operator=(const Server &src);
	public:
		Server(int port, std::string password);
		virtual ~Server();

		// init server
		void	init_server(void);
		int		create_socket(void);
		int		set_socket_options(void);
		int		bind_socket(void);
		int		start_listening(void);
		int		password_checker(std::string password) const;
		bool	pass_validation(std::string check) const;
};
