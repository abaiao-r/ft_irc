/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/30 15:30:50 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include "colours.hpp"

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5

class Server
{
	private:
		int						_port;
		const std::string		_password;
		int						_server_fd;
		sockaddr_in				_address;

		// private because we don't want to allow copies of this class
		Server(void);
		Server(const Server &src);
		Server &operator=(const Server &src);

	public:
		Server(int port, std::string password);
		~Server();

		void	init_server(void);
		int		create_socket(void);
		int		unblock_socket(int fd);
		int		set_socket_options(void);
		int		bind_socket(void);
		int		start_listening(void);
		int		password_checker(std::string password);
};
