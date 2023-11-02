/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:33:53 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/02 16:10:56 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Ft_irc.hpp"
#include "colours.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <map>
#include <sys/socket.h>
#include <poll.h>
#include <algorithm>

class Client
{
	private:
		std::vector<pollfd> clients;


		//private copy constructor
		Client(const Client &src);
		//private assignment operator
		Client &operator=(const Client &src);
		
	public:
		Client(); // Constructor
		~Client(); // Destructor

		//void handle_client(int server_fd, const std::string &password, char ** /* av */);

		void handle_client(int server_fd, const std::string &password, char ** /* av */);

		int handleNewClient(int server_fd);
		int handleAuthenticatedClient(int client_fd, const std::string &password);
};

#endif