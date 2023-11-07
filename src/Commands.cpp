/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:53:51 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/07 21:48:36 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"
#include "Channel.hpp"

std::map<std::string, std::vector<std::string> > user_messages;  // Messages associated with usernames
std::vector<Channel> channels;

Commands::Commands() {}  // Constructor

Commands::~Commands() {} // Destructor

Channel* find_channel_by_user(const User& user)
{
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		for (std::vector<User>::iterator u_it = it->users_in_channel.begin(); u_it != it->users_in_channel.end(); ++u_it)
		{
			if (u_it->nickname == user.nickname)
				return &(*it); // return pointer to the channel
		}
	}
	return (NULL); // user not in any channel
}

bool Commands::msg_channel(User& user, const std::string& channel_name, const std::string& message)
{
	// Find the channel
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->name == channel_name)
			break;
	}

	// If the channel is not found, return false
	if (it == channels.end())
	{
		std::string error_msg = "ERROR: Channel " + channel_name + " not found.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.size(), MSG_NOSIGNAL);
		return false;
	}

	// Broadcast the message to all users in the channel
	std::vector<User>::iterator usr_it;
	for (usr_it = it->users_in_channel.begin(); usr_it != it->users_in_channel.end(); ++usr_it)
	{
		if (usr_it->fd != user.fd) // Don't send to the sender
		{
			std::string channel_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " PRIVMSG " + channel_name + " :" + message + "\r\n";
			send(usr_it->fd, channel_msg.c_str(), channel_msg.size(), MSG_NOSIGNAL);
		}
	}
	return true;
}

bool Commands::handle_privmsg(User& user, const std::string& message)
{
	// Expecting format: PRIVMSG <recipient> <message>
	std::string::size_type space_pos = message.find(' ', 8);  // Find space after "PRIVMSG "
	if (space_pos == std::string::npos)
		return (false);

	std::string recipient = message.substr(8, space_pos - 8);
	std::string actual_msg = message.substr(space_pos + 1);

	if (recipient[0] == '#')
		return msg_channel(user, recipient, actual_msg);
	else
	{
		// Find the user
		std::vector<User>::iterator it;
		for (it = users.begin(); it != users.end(); ++it)
		{
			if (it->nickname == recipient)
				break ;
		}

		if (it == users.end())
		{
			std::string error_msg = "ERROR: User " + recipient + " not found.\r\n";
			send(user.fd, error_msg.c_str(), error_msg.size(), MSG_NOSIGNAL);
			return (false);
		}

		// Send the private message
		std::string pm_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " PRIVMSG " + recipient + " :" + actual_msg + "\r\n";
		send(it->fd, pm_msg.c_str(), pm_msg.size(), MSG_NOSIGNAL);
		std::cout << "DEBUG: " << pm_msg << std::endl;
	}

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
		if(n > 0 && buffer[n - 1] == '\n')
			buffer[n - 1] = '\0';
		if(n > 0 && buffer[n - 2] == '\r')
				buffer[n - 2] = '\0';
		std::string message(buffer);
		std::cout << "Command received: |" << message << "|" << std::endl;
		//print size of command
		std::cout << "Command size: " << message.size() << std::endl;
		if(message.find("JOIN") == 0)
		{
			std::cout << "JOIN command received" << std::endl;
			handle_join(user, message);
		}
		else if(message.find("CREATE") == 0)
			handle_channel(user, message);
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

bool Commands::handle_join(User& user, const std::string& message)
{
	std::string channel_name = message.substr(5);
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		std::cout << "Channel name:" << channel_name << "|" << std::endl;
		std::cout << "real|" << ch_it->name << "|" << std::endl;
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it) {
				if (u_it->nickname == user.nickname)
				{
					std::cout << "User already in channel" << std::endl;
					return (false);
				}
			}
			ch_it->users_in_channel.push_back(user);
			std::string join_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " JOIN :" + channel_name + "\r\n";
			send(user.fd, join_msg.c_str(), join_msg.length(), MSG_NOSIGNAL);
			std::cout << "Joined channel successfully!" << std::endl;
			return (true);
		}
	}
	std::cout << "Channel does not exist" << std::endl;
	std::string error_msg = "ERROR: Channel does not exist.\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return false;
}


bool Commands::handle_channel(User& user, const std::string& message)
{
	if(!user.is_admin)
	{
		std::string error_msg = "ERROR: Only admin can create channels.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	std::string channel_name = message.substr(7);

	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->name == channel_name)
		{
			std::string error_msg = "ERROR: Channel already exists.\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}

	Channel new_channel;
	new_channel.name = "#" + channel_name;
	new_channel.admin = user;
	new_channel.users_in_channel.push_back(user);
	channels.push_back(new_channel);
	send(user.fd, "SUCCESS: Channel created successfully!\r\n", 40, MSG_NOSIGNAL);
	std::cout << "Channel created successfully!" << std::endl;
	return (true);
}

bool Commands::handle_kick(User& user, const std::string& message)
{
	if(!user.is_admin)
	{
		std::string error_msg = "ERROR: Only admin can kick users.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	std::string channel_name = message.substr(6);
	std::string nickname = message.substr(6 + channel_name.size() + 1);

	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it) {
				if (u_it->nickname == nickname)
				{
					ch_it->users_in_channel.erase(u_it);
					std::string kick_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " KICK " + channel_name + " " + nickname + "\r\n";
					send(user.fd, kick_msg.c_str(), kick_msg.length(), MSG_NOSIGNAL);
					std::cout << "Kicked user successfully!" << std::endl;
					return (true);
				}
			}
			std::string error_msg = "ERROR: User not found in channel.\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}
	std::string error_msg = "ERROR: Channel does not exist.\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return (false);
}

bool Commands::handle_invite(User& user, const std::string& message)
{
	if(!user.is_admin)
	{
		std::string error_msg = "ERROR: Only admin can invite users.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	std::string channel_name = message.substr(7);
	std::string nickname = message.substr(7 + channel_name.size() + 1);

	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it) {
				if (u_it->nickname == nickname)
				{
					std::string error_msg = "ERROR: User already in channel.\r\n";
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
			}
			for (std::vector<User>::iterator u_it = users.begin(); u_it != users.end(); ++u_it) {
				if (u_it->nickname == nickname)
				{
					ch_it->users_in_channel.push_back(*u_it);
					std::string invite_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " INVITE " + nickname + " " + channel_name + "\r\n";
					send(u_it->fd, invite_msg.c_str(), invite_msg.length(), MSG_NOSIGNAL);
					std::cout << "Invited user successfully!" << std::endl;
					return (true);
				}
			}
			std::string error_msg = "ERROR: User not found.\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}
	std::string error_msg = "ERROR: Channel does not exist.\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return (false);
}

