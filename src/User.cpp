/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:55:06 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/06 14:52:24 by joao-per         ###   ########.fr       */
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

bool authenticate_user(int client_fd, const std::string& initialCommand, const std::string& password, User &user)
{
	char buffer[512];
	int retry_count = 0;
	const int max_retries = 3;  // Example limit
	std::string message;

	while (retry_count < max_retries && !user.has_authenticated)
	{
		
		if (retry_count == 0)
		{
			message = initialCommand;
		}
		else
		{
			ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
			buffer[n] = '\0';
			std::cout << "Received:1" << buffer << ":" << std::endl;
			if(n > 0 && buffer[n - 1] == '\n')
				buffer[n - 1] = '\0';
			if(n > 0 && buffer[n - 2] == '\r')
				buffer[n - 2] = '\0';
			std::cout << "Received:2" << buffer << ":" << std::endl;
			message = buffer;
		}

		if (message.find("PASS") != 0 || !handle_pass(user, message, password))
		{
			send(client_fd, "ERROR: Invalid password. Please try again.\r\n", 46, MSG_NOSIGNAL);
			std::cout << "ERROR: Invalid password. Correct password is: " << password << std::endl;
			if (retry_count >= max_retries)
			{
				send(client_fd, "ERROR: Too many incorrect attempts. Closing connection.\r\n", 60, MSG_NOSIGNAL);
				close(client_fd);
				return (false);
			}
		}
		else
			send(client_fd, "SUCCESS: Password accepted!\r\n", 29, MSG_NOSIGNAL);
		retry_count++;
	}
	while(!user.is_registered)
	{
		ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if(n <= 0)
		{
			std::cerr << "Error: Failed to receive message or client disconnected" << std::endl;
			close(client_fd);
			break ;
		}
		buffer[n] = '\0';  // Null terminate the received message
		message.assign(buffer);

		if(message.find("NICK") == 0)
		{
			if(!handle_nick(user, message))
				send(client_fd, "ERROR: Nickname is invalid or already in use\r\n", 46, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Nickname set successfully!\r\n", 37, MSG_NOSIGNAL);
		}
		else if(message.find("USER") == 0)
		{
			if(!handle_user(user, message))
				send(client_fd, "ERROR: invalid name. Usage: USER <name> <mode> <servername> <:realname>\r\n", 74, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Name set successfully!\r\n", 34, MSG_NOSIGNAL);
		}

		// Check if user is now registered
		if(!user.nickname.empty() && !user.realname.empty())
		{
			user.is_registered = true;

			// Send welcome messages...
			std::string welcome_message = "\nWelcome to the Internet Relay Network " + user.realname + "! \r\n" + "Username: @" + user.nickname + "\r\n" + "Operator: " + (user.is_admin ? "Yes" : "No") + "\r\n";
			send(client_fd, welcome_message.c_str(), welcome_message.length(), MSG_NOSIGNAL);
			std::string your_host_message = "Your host is running on version 0.1\r\n";
			send(client_fd, your_host_message.c_str(), your_host_message.length(), MSG_NOSIGNAL);
			std::string created_message = "This server was created abaiao-r, joao-per and gacorrei \r\n";
			send(client_fd, created_message.c_str(), created_message.length(), MSG_NOSIGNAL);
		}
	}
	std::cout << "User:" << user.nickname << "Registered" << std::endl;
	return (user.has_authenticated);
}