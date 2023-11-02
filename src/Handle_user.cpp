/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_user.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:17 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/02 12:52:14 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "utils.hpp"

bool handle_user(User& user, const std::string& message)
{
	//Usage: USER <realname> <admin/operator>
	std::istringstream iss(message);
	std::string cmd, name, adminStr;

	iss >> cmd >> name >> adminStr;

	if (cmd != "USER")
		return (false);

	// Check if the name is a single word with only letters
	if (name.empty() || !isAlpha(name) || name.size() > 12)
		return (false);

	if (adminStr == "1")
		user.is_admin = true;
	else if (adminStr == "0")
		user.is_admin = false;
	else
		return (false);

	user.realname = name; 

	return (true);
}


bool isNickInUse(const std::string& nickname)
{
	for (std::vector<User>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->nickname == nickname)
			return (true);
	}
	return (false);
}

bool handle_nick(User& user, const std::string& message)
{
	// Expecting format: NICK <nickname>
	size_t space_pos = message.find(' ');
	if(space_pos == std::string::npos || space_pos + 1 == message.size())
		return (false);

	std::string nickname = message.substr(space_pos + 1);
	/* if(nickname.empty() || !isAlpha(nickname) || nickname.size() > 9)
		return (false); */
	// Check if the nickname is already in use
	if (isNickInUse(nickname))
	{
		std::cout << "ERROR: Nickname already being used!" << std::endl;
		return (false);
	}

	user.nickname = nickname;
	user.nick_registered = true;
	std::cout << "Registered successfully! Nickname:" << nickname << " for fd: " << user.fd << std::endl;
	return (true);
}


bool handle_pass(User& user, const std::string& message, const std::string& server_password)
{
	// Extract password from message
	// Format: PASS <password>
	size_t space_pos = message.find(' ');
	// Invalid message format
	if(space_pos == std::string::npos || space_pos + 1 == message.size())
		return (false);
	
	std::string provided_password = message.substr(space_pos + 1);
	provided_password.resize(provided_password.size() - 1); //Delete new line
	std::cout << "Provided password:" << provided_password << "|" << std::endl;
	std::cout << "Server password:" << server_password << "|" << std::endl;

	if(provided_password.compare(server_password) == 0)
	{
		std::cout << "Passwords match!" << std::endl;
		user.has_authenticated = true;
		return (true);
	}
	return (false);
}