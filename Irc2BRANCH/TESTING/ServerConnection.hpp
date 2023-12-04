/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 09:01:12 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/04 13:33:38 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include "Client.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "Numerics.hpp"
#include "Server.hpp"

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_EVENTS 1024

class ServerConnection: public Server
{
	private:
		int						_epoll_fd;
		static int				_loop_state;
		epoll_event				_main_event;
		epoll_event				_events[MAX_EVENTS];
		ServerConnection(const ServerConnection &copy);
		ServerConnection &operator=(const ServerConnection &copy);
	protected:
		ServerConnection();
		std::vector<Client>		_clients;
		std::vector<Channel>	_channels;
		Bot						_Clippy;
	public:
		ServerConnection(int server_fd);
		virtual ~ServerConnection();
		void		create_epoll();
		void		connection();
		void		client_connection();
		void		disconnect_client(int fd);
		void		disconnect_client(Client &client);
		void		leave_all_rooms(Client &client);
		void 		sendChannelUserListMessage(Channel *channel, const std::string &argument);
		std::string	get_users_string(Channel &channel);
		static void	signal_handler(int sig);
};
