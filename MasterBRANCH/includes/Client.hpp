/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:33:53 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/09 14:51:39 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Ft_irc.hpp"
# include <string>
# include <vector>
# include <cstddef>
# include <map>
# include <sys/socket.h>
# include <poll.h>
# include <algorithm>

class Client
{
	public:
		Client(); // Constructor
		~Client(); // Destructor

		void handle_client(int server_fd, const std::string &password, char ** /* av */);
	private:
		std::vector<pollfd> clients;
};

#endif