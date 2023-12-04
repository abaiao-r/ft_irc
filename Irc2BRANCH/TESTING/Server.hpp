/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/12/04 13:35:11 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "colours.hpp"
#include "ServerUtils.hpp"

class Client;
class Channel;
class ServerConnection;

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5
#define MAX_RETRIES 3

class Server
{
	private:
		int						_port;
		sockaddr_in				_address;
		// private because we don't want to allow copies of this class
		Server(const Server &src);
		Server &operator=(const Server &src);
	protected:
		Server(void);
		const std::string		_password;
		int						_server_fd;
	public:
		Server(int port, std::string password);
		virtual ~Server();

		// init server
		void		init_server(void);
		int			create_socket(void);
		int			set_socket_options(void);
		int			bind_socket(void);
		int			start_listening(void);
};

