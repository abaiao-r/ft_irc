/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:51:11 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/09 11:54:40 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Ft_irc.hpp"
#include "User.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sys/socket.h>

class Commands
{
	public:
		Commands();  // Constructor
		~Commands(); // Destructor

		bool handle_join(User& user, const std::string& message);
		bool handle_msg(User& user, const std::string& message);
		bool handle_privmsg(User& user, const std::string& message);
		bool handle_commands(int client_fd, User &user);
		bool handle_channel(User& user, const std::string& message);
		bool handle_mode(User& user, const std::string& message);
		bool msg_channel(User& user, const std::string& channel_name, const std::string& message);
		bool handle_kick(User& user, const std::string& message);
		bool handle_invite(User& user, const std::string& message);
		bool handle_topic(User& user, const std::string& message);

	private:
		std::map<std::string, std::vector<std::string> > user_messages;  // Messages associated with usernames
};

#endif