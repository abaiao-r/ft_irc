/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:53:51 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/25 16:13:16 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"
#include <map>
#include <vector>
#include <sys/socket.h>

std::map<std::string, std::vector<std::string> > user_messages;  // Messages associated with usernames

Commands::Commands() {}  // Constructor

Commands::~Commands() {} // Destructor

bool Commands::handle_join(User& user)
{
	// Expecting format: JOIN #channel
	// Check if user is already in the chat
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].nickname == user.nickname)
			return (false);
	}
	users.push_back(user);
	return (true);
}


bool Commands::handle_msg(User& user, const std::string& message)
{
	// Expecting format: MSG <message>
	std::string actual_msg = message.substr(4); // Strip off the "MSG " part
	user_messages[user.nickname].push_back(actual_msg);

	// Broadcast this message to all users
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].fd != user.fd) // Don't send to the sender
		{
			std::string broadcast_msg = user.nickname + ": " + actual_msg + "\r\n";
			send(users[i].fd, broadcast_msg.c_str(), broadcast_msg.length(), MSG_NOSIGNAL);
		}
	}
	return true;
}


bool Commands::handle_privmsg(User& user, const std::string& message)
{
	// Expecting format: PRIVMSG <recipient> <message>
	size_t space_pos = message.find(' ', 8);  // Find space after "PRIVMSG "
	if (space_pos == std::string::npos)
		return (false);

	std::string recipient_nick = message.substr(8, space_pos - 8);
	std::string actual_msg = message.substr(space_pos + 1);
	std::cout << "Currently connected users: ";
	for (size_t i = 0; i < users.size(); i++)
	{
		std::cout << users[i].nickname << " ";
	}
	std::cout << std::endl;
	std::cout << "Attempting to send private message to " << recipient_nick << std::endl;
	bool recipient_found = false;
	std::cout << "Number of users: " << users.size() << std::endl;
	for (size_t i = 0; i < users.size(); i++)
	{
		// Remove trailing newline from nickname
		if (!users[i].nickname.empty() && users[i].nickname[users[i].nickname.size() - 1] == '\n')
			users[i].nickname.resize(users[i].nickname.size() - 1);
		//std::cout << "Comparing|" << users[i].nickname << "|with|" << recipient_nick << "|" << std::endl;
		if (users[i].nickname == recipient_nick)
		{
			recipient_found = true;
			std::string pm_msg = ":" + user.nickname + " PRIVMSG " + recipient_nick + " :" + actual_msg + "\r\n";
			send(users[i].fd, pm_msg.c_str(), pm_msg.length(), MSG_NOSIGNAL);
			break ;
		}
	}

	if (!recipient_found)
	{
		std::string error_msg = "ERROR: User " + recipient_nick + " not found.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	// Optionally: Confirm to the sender that the message was sent successfully.
	std::string confirmation = "Message sent to " + recipient_nick + ".\r\n";
	send(user.fd, confirmation.c_str(), confirmation.length(), MSG_NOSIGNAL);
	std::cout << user.nickname << " sent a private message to " << recipient_nick << std::endl;
	return (true);
}


bool Commands::handle_commands(int client_fd, User &user)
{
	char buffer[512];
	bool client_connected = true;
	if(client_connected)
	{
		ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if(n <= 0)
		{
			std::cerr << "Client disconnected" << std::endl;
			client_connected = false;
			return false;
		}
		buffer[n] = '\0';
		std::string message(buffer);
		if(message.find("JOIN") == 0)
			handle_join(user);
		else if(message.find("MSG") == 0)
			handle_msg(user, message);
		else if(message.find("PRIVMSG") == 0)
			handle_privmsg(user, message);
		else
		{
			std::string error_msg = "ERROR: No command found\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		}
	}
	return (true);
}

