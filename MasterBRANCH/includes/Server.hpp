/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/11 19:26:58 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <sys/socket.h>
# include <netinet/in.h>
# include <stdexcept>
# include <unistd.h>

# include "Ft_irc.hpp"
# include "User.hpp"
# include "Client.hpp"
# include "utils.hpp"
# include "colours.hpp"

class Server
{
	private:
		int			_port;
		int			_server_fd;
		sockaddr_in _address;
		epoll_event					_main_event;
		epoll_event					_events[5];
		int							_epoll_fd;

		// private because we don't want to allow copies of this class
		Server(void);
		Server(const Server &src);
		Server &operator=(const Server &src);
	public:
		Server(int port);
		~Server();

		int get_port(void) const;
		int get_server_fd(void) const;
		sockaddr_in get_address(void) const;
		int init_server(void);
		// auxiliar functions of init_server
		int create_socket(void);
		int set_socket_options(void);
		int bind_socket(void);
		int start_listening(void);
		int	unblock_socket(int fd);
		void create_epoll();
		int	add_fd_to_epoll(int epoll_fd, int fd);
		int make_socket_non_blocking(int fd);
		void handle_client(int server_fd, const std::string &password, char ** /* av */);
};

#endif
