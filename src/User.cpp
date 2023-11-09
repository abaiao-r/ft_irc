/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:55:06 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/09 17:08:37 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Handle_user.hpp"

std::vector<User> users;  // List of connected users

User::User() : is_registered(false), fd(-1) {}

User* find_user_by_fd(int fd)
{
	for(size_t i = 0; i < users.size(); i++)
	{
		if(users[i].fd == fd)
			return (&users[i]);
	}
	return (NULL);
}

bool authenticate_user(int client_fd, const std::string& message, const std::string& password, User &user)
{
	if(!user.has_authenticated)
	{
		if (message.find("PASS") != 0 || !handle_pass(user, message, password))
		{
			send(client_fd, "ERROR: Invalid password. Please try again.\r\n", 45, MSG_NOSIGNAL);
			close(client_fd);
			return (false);
		}
		else
		{
			send(client_fd, "SUCCESS: Password accepted!\r\n", 29, MSG_NOSIGNAL);
			user.has_authenticated = true;
			return (true);
		}
	}
	
	if(message.find("NICK") == 0)
	{
		if(!handle_nick(user, message))
		{
			std::cout << "fd: " << user.fd << " - ERROR: Nickname is invalid or already in use" << std::endl;
			return (true);
		}
		else
		{
			send(client_fd, "SUCCESS: Nickname set successfully!\r\n", 38, MSG_NOSIGNAL);
			user.nick_registered = true;
		}
	}
	else if(message.find("USER") == 0)
	{
		if(!handle_user(user, message))
		{
			send(client_fd, "Usage: USER <name> <mode> <servername> <:realname>\r\n", 74, MSG_NOSIGNAL);
			std::cout << "fd: " << user.fd << " - ERROR: invalid name. Usage: USER <name> <mode> <servername> <:realname>" << std::endl;
			return (true);
		}
		else
		{
			send(client_fd, "SUCCESS: Name set successfully!\r\n", 34, MSG_NOSIGNAL);
			std::cout << "SUCCESS: Name set successfully!" << std::endl;
			user.user_registered = true;
		}
	}
	// Check if user is now registered
	if(user.user_registered == true && user.nick_registered == true)
	{
		user.is_registered = true;

		// Send welcome messages...
		std::string welcome_message = "\nWelcome to the Internet Relay Network " + user.realname + "! \r\n" + "Username: @" + user.nickname + "\r\n" + "Operator: " + (user.is_admin ? "Yes" : "No") + "\r\n";
		send(client_fd, welcome_message.c_str(), welcome_message.length(), MSG_NOSIGNAL);
		std::string your_host_message = "Your host is running on version 0.1\r\n";
		send(client_fd, your_host_message.c_str(), your_host_message.length(), MSG_NOSIGNAL);
		std::string created_message = "This server was created abaiao-r, gacorrei and joao-per \r\n";
		send(client_fd, created_message.c_str(), created_message.length(), MSG_NOSIGNAL);
		std::cout << "User:" << user.nickname << "Registered" << std::endl;
		
	}
	
	return (true);
}
