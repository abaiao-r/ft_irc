/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 09:01:12 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 11:32:30 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include "ServerCommands.hpp"

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_EVENTS 1024

class ServerConnection: public ServerCommands
{
	private:
		ServerConnection(const ServerConnection &copy);
		ServerConnection &operator=(const ServerConnection &copy);
	protected:
		static int				_loop_state;
		int						_epoll_fd;
		epoll_event				_main_event;
		epoll_event				_events[MAX_EVENTS];
		ServerConnection();
	public:
		virtual 	~ServerConnection();
		void		create_epoll(int server_fd);
		void		add_epoll(int fd);
		void		connection(int server_fd);
		void		client_connection(int server_fd);
		void		disconnect_client(int fd);
		void		disconnect_client(Client &client);
		void		leave_all_rooms(Client &client);
		void 		sendChannelUserListMessage(Channel *channel, const std::string &argument);
		std::string	get_users_string(Channel &channel);
		static void	signal_handler(int sig);
		void		info_print();
};
