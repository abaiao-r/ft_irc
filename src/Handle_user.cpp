/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_user.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:17 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/09 17:17:48 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "utils.hpp"

bool handle_user(User& user, const std::string& message)
{
	//Usage: USER <username> <mode> <unused> :<realname>
	std::istringstream iss(message);
	std::string cmd, username, mode, unused, realname;

	iss >> cmd >> username >> mode >> unused;

	// Extract the realname (after the colon)
	if(iss.str().find(':') != std::string::npos)
		realname = iss.str().substr(iss.str().find(':') + 1);

	// Check if the username is a single word with only letters
	if (username.empty() || !isAlpha(username) || username.size() > 12)
	{
		send(user.fd, "ERROR: Username empty or non-Alpha or too big\r\n", 48, MSG_NOSIGNAL);
		return (false);
	}

	// Check if the realname is a single word with only letters
	if (realname.empty() || !isAlpha(realname))
	{
		send(user.fd, "ERROR: Realname empty or non-Alpha\r\n", 37, MSG_NOSIGNAL);
		return (false);
	}

	// You can adjust this based on your needs. 
	// Here, I'm using '0' to indicate a normal user and '1' to indicate an admin/operator.
	if (mode == "1")
		user.is_admin = true;
	else if (mode == "0")
		user.is_admin = false;
	else
		return (false);
	user.username = username;
	user.hostname = "localhost";
	//if realname has \n, delete it
	if (realname[realname.size() - 1] == '\n')
		realname.resize(realname.size() - 1);
	user.realname = realname;
	std::cout << "Registered successfully! Username:" << username << " for fd: " << user.fd << std::endl;

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
	//istream
	std::istringstream iss(message);
	std::string cmd, nickname, resto;
	iss >> cmd >> nickname >> resto;
	
	if(resto != "")
	{
		send(user.fd, "ERROR: Nickname can only be 1 word\r\n", 37, MSG_NOSIGNAL);
		return (false);
	}
	if(nickname.empty() || nickname.size() > 9)
	{
		send(user.fd, "ERROR: Nickname size should be between 1 and 9\r\n", 49, MSG_NOSIGNAL);
		return (false);
	}
	// Check if the nickname is already in use
	if (isNickInUse(nickname))
	{
		send(user.fd, "ERROR: Nickname is already in use\r\n", 36, MSG_NOSIGNAL);
		return (false);
	}
	//if last char is \n, delete it
	if (nickname[nickname.size() - 1] == '\n')
		nickname.resize(nickname.size() - 1);
	user.nickname = nickname;
	user.nick_registered = true;
	std::cout << "Nickname:" << nickname << std::endl;
	return (true);
}


bool handle_pass(User& user, const std::string& message, const std::string& server_password)
{
	// Format: PASS <password>
	std::istringstream iss(message);
	std::string cmd, provided_password;
	iss >> cmd >> provided_password;
	if(provided_password.compare(server_password) == 0)
	{
		std::cout << "Passwords match!" << std::endl;
		user.has_authenticated = true;
		return (true);
	}
	return (false);
}