/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/30 14:18:35 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/30 14:48:58 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include "Client.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "colours.hpp"
#include "utils.hpp"
#include <map>
#include <algorithm>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <sstream>

#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5
#define MAX_EVENTS 1024
#define MAX_RETRIES 3
#define BUFFER_READ_SIZE 1024
#define MAX_LEN 10
#define MIN_LEN 3

class ServerConnection
{
	private:
		sockaddr_in				_address;
		int						_epoll_fd;
		epoll_event				_main_event;
		epoll_event				_events[MAX_EVENTS];
		std::vector<Client>		_clients;
		std::vector<Channel>	_channels;
		static int				_loop_state;
		Bot						_Clippy;
		ServerConnection(const ServerConnection &copy);
		ServerConnection &operator=(const ServerConnection &copy);
	public:
		ServerConnection();
		~ServerConnection();
		void		create_epoll();
		void		connection();
		void		client_connection();
		int 		client_cmds(Client &client);
		int			choose_cmd(Client &client, std::string in);
		void 		login(Client &client, const std::string &buffer);
		void 		parseLoginLine(const std::string &line, std::map<std::string, std::string> &cmds);
};
