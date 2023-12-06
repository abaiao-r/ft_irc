/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_user.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:17 by abaiao-r          #+#    #+#             */
<<<<<<< HEAD:src/Handle_user.cpp
/*   Updated: 2023/11/11 14:15:41 by joao-per         ###   ########.fr       */
=======
/*   Updated: 2023/11/08 00:22:22 by joao-per         ###   ########.fr       */
>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "utils.hpp"
#include "colours.hpp"

bool handle_user(User& user, const std::string& message)
{
	//Usage: USER <username> <mode> <unused> :<realname>
	std::istringstream iss(message);
	std::string cmd, username, mode, unused, realname;

	iss >> cmd >> username >> mode >> unused;
<<<<<<< HEAD:src/Handle_user.cpp
=======

	// Extract the realname (after the colon)
	if(iss.str().find(':') != std::string::npos)
		realname = iss.str().substr(iss.str().find(':') + 1);
>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp

	// Extract the realname (after the colon)
	if(iss.str().find(':') != std::string::npos)
		realname = iss.str().substr(iss.str().find(':') + 1);

	// Check if the username is a single word with only letters
	if (username.empty() || !isAlpha(username) || username.size() > 12)
	{
		send(user.fd, "ERROR: Username empty or non-Alpha or too big\r\n", 48, MSG_NOSIGNAL);
		return (false);
	}

<<<<<<< HEAD:src/Handle_user.cpp
	// Check if the realname is a single word with only letters
	if (realname.empty())
	{
		send(user.fd, "ERROR: Realname empty\r\n", 37, MSG_NOSIGNAL);
=======
	// Check if the username is a single word with only letters
	if (username.empty() || !isAlpha(username) || username.size() > 12 || realname.empty())
>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp
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
<<<<<<< HEAD:src/Handle_user.cpp
	//if realname has \n, delete it
	if (realname[realname.size() - 1] == '\n')
		realname.resize(realname.size() - 1);
	user.realname = realname;
	std::cout << GREEN << "Username: " << BOLDGREEN << username << GREEN << " registered!" << RESET << std::endl;
=======
	user.realname = realname; 
	std::cout << "Registered successfully! Username:" << username << " for fd: " << user.fd << std::endl;
>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp

	return (true);
}

<<<<<<< HEAD:src/Handle_user.cpp
=======


>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp
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
	std::cout << GREEN << "Nickname: " << BOLDGREEN << nickname << GREEN << " registered!" << RESET << std::endl;
	return (true);
}


bool handle_pass(User& user, const std::string& message, const std::string& server_password)
{
	// Format: PASS <password>
<<<<<<< HEAD:src/Handle_user.cpp
	std::istringstream iss(message);
	std::string cmd, provided_password;
	iss >> cmd >> provided_password;
=======
	size_t space_pos = message.find(' ');
	// Invalid message format
	if(space_pos == std::string::npos || space_pos + 1 == message.size())
		return (false);
	
	std::string provided_password = message.substr(space_pos + 1);
	//if last char is \n, delete it
	/* if (provided_password[provided_password.size() - 1] == '\n')
		provided_password.resize(provided_password.size() - 1); */

	
	std::cout << "Provided password:" << provided_password << "|" << std::endl;
	std::cout << "Server password:" << server_password << "|" << std::endl;

>>>>>>> irc2:MasterBRANCH/src/Handle_user.cpp
	if(provided_password.compare(server_password) == 0)
	{
		std::cout << GREEN << "Correct password!" << RESET << std::endl;
		user.has_authenticated = true;
		return (true);
	}
	return (false);
}