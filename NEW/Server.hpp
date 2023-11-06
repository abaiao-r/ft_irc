/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/06 10:28:47 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <vector>
#include <algorithm>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "colours.hpp"
#include "Client.hpp"
// #include "Channel.hpp"

class Client;
class Channel;

#define C_IT std::vector<Client>::iterator
#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5
#define MAX_EVENTS 1024
#define MAX_RETRIES 3
#define BUFFER_READ_SIZE 1024
#define MAX_LEN 10
#define MIN_LEN 3


class Server
{
	private:
		int					_port;
		const std::string	_password;
		int					_server_fd;
		sockaddr_in			_address;
		int					_epoll_fd;
		epoll_event			_main_event;
		epoll_event			_events[MAX_EVENTS];
		std::vector<Client>	_clients;
		static int			_loop_state;

		// private because we don't want to allow copies of this class
		Server(void);
		Server(const Server &src);
		Server &operator=(const Server &src);


	public:
		Server(int port, std::string password);
		~Server();

		//DELETE?
		int			get_port(void) const;
		int			get_server_fd(void) const;
		sockaddr_in get_address(void) const;
		//

		void		init_server(void);
		// auxiliary functions of init_server
		int			create_socket(void);
		int			unblock_socket(int fd);
		int			set_socket_options(void);
		int			bind_socket(void);
		int			start_listening(void);

		void		create_epoll();
		void		connection();
		void		client_connection();
		void		client_actions(Client &client);
		void		authenticate(Client &client);
		bool		pass_validation(std::string check) const;
		bool		name_validation(std::string check);
		int			nick_validation(std::string check);
		Client		*find_client(int fd);
		void		disconnect_client(int fd);
		static void	signal_handler(int sig);
		static void	signal_reset();
		static void	signal_global();
};
