/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:53:51 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/09 14:25:51 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"
#include "Channel.hpp"
#include "colours.hpp"

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
		std::string message(buffer);
		//delete /r and /n from message if they exist
		if(message.find("\r\n") != std::string::npos)
			message.erase(message.find("\r\n"), 2);
		else if(message.find("\n") != std::string::npos)
			message.erase(message.find("\n"), 1);
		std::cout << "Command received: |" << message << "|" << std::endl;
		//print size of command
		std::cout << "Command size: " << message.size() << std::endl;
		if(message.find("JOIN") == 0)
			handle_join(user, message);
		else if(message.find("CREATE") == 0)
			handle_channel(user, message);
		else if(message.find("PRIVMSG") == 0)
			handle_privmsg(user, message);
		else if(message.find("INVITE") == 0)
			handle_invite(user, message);
		else if(message.find("TOPIC") == 0)
			handle_topic(user, message);
		else if(message.find("KICK") == 0)
			handle_kick(user, message);
		else if(message.find("MODE") == 0)
			handle_mode(user, message);
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
	//istream
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string password;
	iss >> command >> channel_name >> password;
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		std::cout << "Channel name:" << channel_name << "|" << std::endl;
		std::cout << "real|" << ch_it->name << "|" << std::endl;
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it)
			{
				if (u_it->nickname == user.nickname)
				{
					std::cout << "User already in channel" << std::endl;
					return (false);
				}
			}
			//verification if channel is full
			if(ch_it->limit > 0)
			{
				if(ch_it->users_in_channel.size() >= ch_it->limit)
				{
					std::string error_msg = std::string("RED") + "ERROR: Channel is full.\r\n" + std::string(RESET);
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
			}
			//verification if channel is invite only
			if(ch_it->is_invite_only)
			{
				for (std::vector<std::string>::iterator it = ch_it->invite_list.begin(); it != ch_it->invite_list.end(); ++it)
				{
					if (*it == user.nickname)
					{
						ch_it->users_in_channel.push_back(user);
						std::string join_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " JOIN :" + channel_name + "\r\n";
						send(user.fd, join_msg.c_str(), join_msg.length(), MSG_NOSIGNAL);
						std::string topic_message = ":" + user.hostname + " 332 " + user.nickname + " " + channel_name + " :" + ch_it->topic + "\r\n";
						send(user.fd, topic_message.c_str(), topic_message.size(), 0);
						std::cout << "Joined channel successfully!" << std::endl;
						return (true);
					}
				}
				std::string error_msg = "ERROR: Channel is invite only.\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			//verification if channel has password
			if(ch_it->password != "")
			{
				//check if password is correct
				if(ch_it->password != password)
				{
					std::string error_msg = std::string(RED) + "ERROR: Wrong password." + std::string(RESET) + "\nUsage: JOIN <channel> <password>\r\n";
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
			}
			ch_it->users_in_channel.push_back(user);
			std::string join_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " JOIN :" + channel_name + "\r\n";
			send(user.fd, join_msg.c_str(), join_msg.length(), MSG_NOSIGNAL);
			std::string topic_message = ":" + user.hostname + " 332 " + user.nickname + " " + channel_name + " :" + ch_it->topic + "\r\n";
    		send(user.fd, topic_message.c_str(), topic_message.size(), 0);
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

	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string topic;
	//if there is topic:
	iss >> command >> channel_name >> topic;

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
	new_channel.is_invite_only = false;
	if (topic != "")
		new_channel.topic = topic;
	else
		new_channel.topic = "No topic";
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
	
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
    std::string nickname;
	iss >> command >> channel_name >> nickname;
	
	std::vector<Channel>::iterator ch_it = channels.begin();
	for (; ch_it != channels.end(); ++ch_it)
	{
		
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it)
			{
				if (u_it->nickname == nickname)
				{
					//Check if user is not already kicked
					if(ch_it->ban_list.size() > 0)
					{
						for (std::vector<std::string>::iterator it = ch_it->ban_list.begin(); it != ch_it->ban_list.end(); ++it)
						{
							if (*it == nickname)
							{
								std::string error_msg = "ERROR: User already kicked.\r\n";
								send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
								return (false);
							}
						}
					}
					ch_it->users_in_channel.erase(u_it);
					std::string kick_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " KICK " + channel_name + " " + nickname + "\r\n";
					send(user.fd, kick_msg.c_str(), kick_msg.length(), MSG_NOSIGNAL);
					std::cout << "Kicked user successfully!" << std::endl;
					ch_it->ban_list.push_back(nickname);
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
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
    std::string nickname;
	std::string resto;
	iss >> command >> channel_name >> nickname >> resto;

	//if variable resto is not empty, return false with error message
	if(resto != "")
	{
		std::string error_msg = "ERROR: Usage: INVITE <channel> <user>\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it)
	{
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it)
			{
				if (u_it->nickname == nickname)
				{
					std::string error_msg = "ERROR: User already in channel.\r\n";
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
			}
			//verification if channel is not full
			if(ch_it->limit > 0)
			{
				if(ch_it->users_in_channel.size() >= ch_it->limit)
				{
					std::string error_msg = std::string(RED) + "ERROR: Channel is full, you can't invite more users." + std::string(RESET) + "\r\n";
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
			}
			
			for (std::vector<User>::iterator u_it = users.begin(); u_it != users.end(); ++u_it)
			{
				if (u_it->nickname == nickname)
				{
					ch_it->users_in_channel.push_back(*u_it);
					ch_it->invite_list.push_back(nickname);
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

bool Commands::handle_topic(User& user, const std::string& message)
{
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
    std::string topic;
	iss >> command >> channel_name >> topic;

	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		if (ch_it->name == channel_name)
		{
			if (ch_it->users_in_channel.size() == 0)
			{
				std::string error_msg = "ERROR: No users in channel.\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			if (ch_it->users_in_channel[0].nickname != user.nickname)
			{
				std::string error_msg = "ERROR: Only admin can change topic.\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			ch_it->topic = topic;
			std::string topic_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " TOPIC " + channel_name + " :" + topic + "\r\n";
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it) {
				send(u_it->fd, topic_msg.c_str(), topic_msg.length(), MSG_NOSIGNAL);
			}
			std::cout << "Changed topic successfully!" << std::endl;
			return (true);
		}
	}
	std::string error_msg = "ERROR: Channel does not exist.\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return (false);
}

bool Commands::handle_mode(User& user, const std::string& message)
{
	// Expecting format: MODE <channel> <mode>
	size_t space_pos = message.find(' ', 5);  // Find space after "MODE "
	if (space_pos == std::string::npos)
		return (false);

	//do a istream
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string mode;
	std::string argument;
	iss >> command >> channel_name >> mode >> argument;

	Channel* channel = NULL;
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
		std::cout << "Channel name:" << channel_name << "|" << std::endl;
		std::cout << "real|" << ch_it->name << "|" << std::endl;
		if (ch_it->name == channel_name)
		{
			channel = &(*ch_it);
			break ;
		}
	}

	if (!channel)
	{
		std::string error_msg = "ERROR: Channel " + channel_name + " not found.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	if (mode == "+o")
	{
		//bool user_found = false;
		for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
		{
			if (ch_it->nickname == argument)
			{
				//user_found = true;
				ch_it->is_admin = true;
				break ;
			}
		}
	}
	else if (mode == "-o")
	{
		//bool user_found = false;
		for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
		{
			if (ch_it->nickname == argument)
			{
				//user_found = true;
				ch_it->is_admin = false;
				break ;
			}
		}
	}
	else if (mode == "+k")
	{
		// Expecting format: MODE <channel> +k <password>
		channel->password = argument;
		std::cout << "Password set to " << argument << " on channel: " << channel->name << std::endl;
		send(user.fd, "SUCCESS: Password set successfully!\r\n", 38, MSG_NOSIGNAL);
	}
	else if (mode == "-k")
	{
		// Expecting format: MODE <channel> -k
		channel->password = "";
		std::cout << "Password removed from channel: " << channel->name << std::endl;
		send(user.fd, "SUCCESS: Password removed successfully!\r\n", 42, MSG_NOSIGNAL);
	}
	else if (mode == "+i")
	{
		// Expecting format: MODE <channel> +i
		channel->is_invite_only = true;
		std::cout << "Channel " << channel->name << " is now invite only." << std::endl;
		send(user.fd, "SUCCESS: Channel is now invite only.\r\n", 39, MSG_NOSIGNAL);
	}
	else if (mode == "-i")
	{
		// Expecting format: MODE <channel> -i
		channel->is_invite_only = false;
		std::cout << "Channel " << channel->name << " is no longer invite only." << std::endl;
		send(user.fd, "SUCCESS: Channel is no longer invite only.\r\n", 45, MSG_NOSIGNAL);
	}
	else if (mode == "+t")
	{
		// Expecting format: MODE <channel> +t
		channel->is_topic_settable = true;
		std::cout << "Channel " << channel->name << " topic is now settable." << std::endl;
		send(user.fd, "SUCCESS: Channel topic is now settable.\r\n", 42, MSG_NOSIGNAL);
	}
	else if (mode == "-t")
	{
		// Expecting format: MODE <channel> -t
		channel->is_topic_settable = false;
		std::cout << "Channel " << channel->name << " topic is no longer settable." << std::endl;
		send(user.fd, "SUCCESS: Channel topic is no longer settable.\r\n", 48, MSG_NOSIGNAL);
	}
	else if (mode == "+l")
	{
		// Expecting format: MODE <channel> +l <limit>

		// Transform argument into a number
		std::istringstream issz(argument);
		long unsigned int limit;
		issz >> limit;
		
		channel->limit = limit;
		std::cout << "Channel " << channel->name << " limit is now set to " << channel->limit << "." << std::endl;
		send(user.fd, "SUCCESS: Channel limit is now set.\r\n", 37, MSG_NOSIGNAL);
	}
	else if (mode == "-l")
	{
		// Expecting format: MODE <channel> -l
		channel->limit = 0;
		std::cout << "Channel " << channel->name << " limit is now removed." << std::endl;
		send(user.fd, "SUCCESS: Channel limit is now removed.\r\n", 41, MSG_NOSIGNAL);
	}
	else
	{
		std::string error_msg = "ERROR: Invalid mode.\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	return (true);
}