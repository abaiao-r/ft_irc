/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:55:06 by abaiao-r          #+#    #+#             */
<<<<<<< HEAD:src/User.cpp
/*   Updated: 2023/11/10 15:22:58 by joao-per         ###   ########.fr       */
=======
/*   Updated: 2023/11/07 20:12:06 by joao-per         ###   ########.fr       */
>>>>>>> irc2:MasterBRANCH/src/User.cpp
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Handle_user.hpp"
#include "colours.hpp"

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

<<<<<<< HEAD:src/User.cpp
bool authenticate_user(int client_fd, const std::string& message, const std::string& password, User &user)
{
	if(!user.has_authenticated)
	{
		if (message.find("PASS") != 0 || !handle_pass(user, message, password))
		{
			send(client_fd, "\033[33mERROR: Invalid password. Please try again.\033[0m\r\n", 54, MSG_NOSIGNAL);
			close(client_fd);
			return (false);
		}
		else
		{
			send(client_fd, "\033[32mSUCCESS: Password accepted!\033[0m\r\n", 39, MSG_NOSIGNAL);
			user.has_authenticated = true;
			return (true);
		}
=======
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
>>>>>>> irc2:MasterBRANCH/src/User.cpp
	}
	
	if(message.find("NICK") == 0)
	{
<<<<<<< HEAD:src/User.cpp
		if(!handle_nick(user, message))
			return (true);
		else
=======
		ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if(n <= 0)
>>>>>>> irc2:MasterBRANCH/src/User.cpp
		{
			send(client_fd, "\033[32mSUCCESS: Nickname set successfully!\033[0m\r\n", 47, MSG_NOSIGNAL);
			user.nick_registered = true;
		}
<<<<<<< HEAD:src/User.cpp
	}
	else if(message.find("USER") == 0)
	{
		if(!handle_user(user, message))
		{
			send(client_fd, "Usage: USER <name> <mode> <servername> <:realname>\r\n", 74, MSG_NOSIGNAL);
			std::cout << "fd: " << user.fd << " - ERROR: invalid name. Usage: USER <name> <mode> <servername> <:realname>" << std::endl;
			return (true);
=======
		buffer[n] = '\0';  // Null terminate the received message
		message.assign(buffer);

		if(message.find("NICK") == 0)
		{
			if(!handle_nick(user, message))
				send(client_fd, "ERROR: Nickname is invalid or already in use\r\n", 46, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Nickname set successfully!\r\n", 37, MSG_NOSIGNAL);
>>>>>>> irc2:MasterBRANCH/src/User.cpp
		}
		else
		{
<<<<<<< HEAD:src/User.cpp
			send(client_fd, "\033[32mSUCCESS: Name set successfully!\033[0m\r\n", 43, MSG_NOSIGNAL);
			std::cout << "\033[32mSUCCESS: Name set successfully!\033[0m" << std::endl;
			user.user_registered = true;
=======
			if(!handle_user(user, message))
				send(client_fd, "ERROR: invalid name. Usage: USER <name> <mode> <servername> <:realname>\r\n", 74, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Name set successfully!\r\n", 34, MSG_NOSIGNAL);
>>>>>>> irc2:MasterBRANCH/src/User.cpp
		}
	}
	// Check if user is now registered
	if(user.user_registered == true && user.nick_registered == true)
	{
		user.is_registered = true;

<<<<<<< HEAD:src/User.cpp
		// Send welcome messages...
		std::string welcome_message = "\nWelcome to the Internet Relay Network " + user.realname + "! \r\n" + "Username: @" + user.nickname + "\r\n" + "Operator: " + (user.is_admin ? "Yes" : "No") + "\r\n";
		send(client_fd, welcome_message.c_str(), welcome_message.length(), MSG_NOSIGNAL);
		std::string your_host_message = "Your host is running on version 0.1\r\n";
		send(client_fd, your_host_message.c_str(), your_host_message.length(), MSG_NOSIGNAL);
		std::string created_message = "This server was created abaiao-r, gacorrei and joao-per \r\n";
		send(client_fd, created_message.c_str(), created_message.length(), MSG_NOSIGNAL);
		std::cout << CYAN << "User: " << BOLDCYAN << user.nickname << " registered" << RESET << std::endl;
		
=======
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
>>>>>>> irc2:MasterBRANCH/src/User.cpp
	}
	
	return (true);
}
