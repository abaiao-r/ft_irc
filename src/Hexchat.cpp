/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hexchat.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 14:53:19 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/09 14:53:21 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Handle_user.hpp"

std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		tokens.push_back(token);
	}

	tokens.push_back(s.substr(pos_start));
	return tokens;
}

bool handle_hexchat(const std::string& firstMessage, const std::string& secondMessage, User& user, const std::string& password, int client_fd)
{
	// Check first message for CAP
	if(firstMessage.substr(0, 3) != "CAP")
		return (false);

	// Split the second message by "\r\n"
	std::vector<std::string> commands;
	std::string::size_type start = 0, end;
	while ((end = secondMessage.find("\r\n", start)) != std::string::npos)
	{
		commands.push_back(secondMessage.substr(start, end - start));
		start = end + 2; // 2 for length of "\r\n"
	}

	// print vector commands
	for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it)
		std::cout << "Command: " << *it << std::endl;

	// Handle PASS
	if(!handle_pass(user, commands[1], password))
		return (false);

	// Handle NICK
	if(!handle_nick(user, commands[2]))
		return (false);

	// Handle USER
	if(!handle_user(user, commands[3]))
		return (false);

	user.is_registered = true;
	user.has_authenticated = true;
	std::cout << "Registered successfully! Nickname:" << user.nickname << " for fd: " << client_fd << std::endl;
	std::string welcome_msg = ":FT_IRC 001 " + user.nickname + " :Welcome to the IRC network, " + user.nickname + "!\n" + user.username + "@" + user.hostname + "\r\n";
	send(client_fd, welcome_msg.c_str(), welcome_msg.size(), MSG_NOSIGNAL);
	return (true);
}
