/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:53:51 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/10 12:30:25 by joao-per         ###   ########.fr       */
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

bool Commands::isuser_onchannel(User& user, const std::string& channel_name)
{
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it)
	{
		if (ch_it->name == channel_name)
		{
			for (std::vector<User>::iterator u_it = ch_it->users_in_channel.begin(); u_it != ch_it->users_in_channel.end(); ++u_it)
			{
				if (u_it->nickname == user.nickname)
					return (true);
			}
		}
	}
	return (false);
}

bool Commands::verify_operator(User& user, const std::string& channel_name)
{
	if(user.is_admin == true)
		return (true);
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it)
	{
		if (ch_it->name == channel_name)
		{
			for (std::vector<std::string>::iterator it = ch_it->operator_list.begin(); it != ch_it->operator_list.end(); ++it)
			{
				if (*it == user.nickname)
					return (true);
				else if(user.is_admin == true)
					return (true);
			}
		}
	}
	return (false);
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
		std::string error_msg = "\033[31mERROR: Channel " + channel_name + " not found.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.size(), MSG_NOSIGNAL);
		return false;
	}

	if(isuser_onchannel(user, channel_name) == false)
	{
		std::string error_msg = "\033[31mERROR: You are not in the channel.\033[0m\r\n";
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
			std::string error_msg = "\033[31mERROR: User " + recipient + " not found.\033[0m\r\n";
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
		//std::cout << "Command received: |" << message << "|" << std::endl;
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
			std::string error_msg = "\033[31mERROR: No command found\033[0m\r\n";
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
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it)
	{
		if (ch_it->name == channel_name)
		{
			if(isuser_onchannel(user, channel_name))
			{
				std::string error_msg = "\033[31mERROR: You are already in the channel.\033[0m\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			//verification if channel is full
			if(ch_it->limit > 0)
			{
				if(ch_it->users_in_channel.size() >= ch_it->limit)
				{
					std::string error_msg = std::string(RED) + "ERROR: Channel is full.\r\n" + std::string(RESET);
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
						std::cout << "\033[33mUser: " << user.nickname << " \033[32mjoined " << channel_name << " channel successfully!\033[0m" << std::endl;
						return (true);
					}
				}
				std::string error_msg = "\033[31mERROR: Channel is invite only.\033[0m\r\n";
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
			std::cout << "\033[33mUser: " << user.nickname << " \033[32mjoined " << channel_name << " channel successfully!\033[0m" << std::endl;
			return (true);
		}
	}
	//remove the first character from channel_name
	channel_name.erase(0, 1);
	handle_channel(user, "CREATE " + channel_name);
	return (true);
}


bool Commands::handle_channel(User& user, const std::string& message)
{
	//expected format: CREATE <channel> <OPTIONAL:topic>
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string topic;
	//if there is topic:
	iss >> command >> channel_name >> topic;

	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (it->name == channel_name)
		{
			std::string error_msg = "\033[31mERROR: Channel already exists.\033[0m\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}

	Channel new_channel;
	new_channel.name = "#" + channel_name;
	new_channel.admin = user;
	new_channel.users_in_channel.push_back(user);
	std::string join_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " JOIN :" + channel_name + "\r\n";
	send(user.fd, join_msg.c_str(), join_msg.length(), MSG_NOSIGNAL);
	new_channel.is_invite_only = false;
	if (topic != "")
		new_channel.topic = topic;
	else
		new_channel.topic = "Not defined";
	channels.push_back(new_channel);
	//add user to operator_list
	new_channel.operator_list.push_back(user.nickname);
	
	send(user.fd, "\033[32mSUCCESS: Channel created successfully!\033[0m\r\n", 50, MSG_NOSIGNAL);
	std::cout << "\033[33mChannel: " << channel_name << " \033[32mwas created successfully!\033[0m" << std::endl;
	return (true);
}

bool Commands::handle_kick(User& user, const std::string& message)
{
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string nickname;
	iss >> command >> channel_name >> nickname;
	//verify if user is in operator_list
	if(!verify_operator(user, channel_name))
	{
		std::string error_msg = "\033[31mERROR: You are not an admin.\033[0m\r\n";
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
					//Check if user is not already kicked
					if(ch_it->ban_list.size() > 0)
					{
						for (std::vector<std::string>::iterator it = ch_it->ban_list.begin(); it != ch_it->ban_list.end(); ++it)
						{
							if (*it == nickname)
							{
								std::string error_msg = "\033[31mERROR: User's already kicked.\033[0m\r\n";
								send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
								return (false);
							}
						}
					}
					ch_it->users_in_channel.erase(u_it);
					std::string kick_msg = ":" + user.nickname + "!" + user.username + "@" + user.hostname + " KICK " + channel_name + " " + nickname + "\r\n";
					send(user.fd, kick_msg.c_str(), kick_msg.length(), MSG_NOSIGNAL);
					std::cout << "\033[33m" << user.nickname <<" kicked " << nickname << " from channel: " << channel_name << "\033[0m" << std::endl;
					ch_it->ban_list.push_back(nickname);
					return (true);
				}
			}
			std::string error_msg = "\033[31mERROR: User not found in channel.\033[0m\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}
	std::string error_msg = "\033[31mERROR: Channel does not exist.\033[0m\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return (false);
}

bool Commands::handle_invite(User& user, const std::string& message)
{
	if(!user.is_admin)
	{
		std::string error_msg = "\033[31mERROR: Only admin can invite users.\033[0m\r\n";
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
		std::string error_msg = "\033[31mERROR: \033[33mUsage: INVITE <channel> <user>\033[0m\r\n";
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
					std::string error_msg = "\033[31mERROR: User already in channel.\033[0m\r\n";
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
			std::string error_msg = "\033[31mERROR: User not found.\033[0m\r\n";
			send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
			return (false);
		}
	}
	std::string error_msg = "\033[31mERROR: Channel does not exist.\033[0m\r\n";
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
			if (isuser_onchannel(user, channel_name) == false)
			{
				std::string error_msg = "\033[31mERROR: You are not in the channel.\033[0m\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			if (ch_it->users_in_channel.size() == 0)
			{
				std::string error_msg = "\033[31mERROR: No users in channel.\033[0m\r\n";
				send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
				return (false);
			}
			//if topic is empty, show the current topic
			if (topic == "")
			{
				std::string topic_msg = ":" + user.hostname + " 332 " + user.nickname + " " + channel_name + " :" + ch_it->topic + "\r\n";
				send(user.fd, topic_msg.c_str(), topic_msg.size(), 0);
				return (true);
			}
			if (ch_it->is_topic_settable == false)
			{
				if(verify_operator(user, channel_name) == false)
				{
					std::string error_msg = "\033[31mERROR: topic is only settable for operators\033[0m\r\n";
					send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
					return (false);
				}
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
	std::string error_msg = "\033[31mERROR: Channel does not exist.\033[0m\r\n";
	send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
	return (false);
}

bool Commands::handle_mode(User& user, const std::string& message)
{
	// Expecting format: MODE <channel> <mode> <OPCIONAL:argument>
	std::istringstream iss(message);
	std::string command;
	std::string channel_name;
	std::string mode;
	std::string argument;
	iss >> command >> channel_name >> mode >> argument;

	Channel* channel = NULL;
	for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it)
	{
		if (ch_it->name == channel_name)
		{
			channel = &(*ch_it);
			break ;
		}
	}
	if (!channel)
	{
		std::string error_msg = "\033[31mERROR: Channel " + channel_name + " not found.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}

	if(!verify_operator(user, channel_name))
	{
		std::string error_msg = "\033[31mERROR: You are not an admin.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	if(!isuser_onchannel(user, channel_name))
	{
		std::string error_msg = "\033[31mERROR: You are not in the channel.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	
	if (mode == "+o")
	{
		// Expecting format: MODE <channel> +o <nickname>
		for (std::vector<std::string>::iterator it = channel->operator_list.begin(); it != channel->operator_list.end(); ++it)
		{
			if (*it == user.nickname)
			{
				for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
				{
					if (ch_it->nickname == argument)
					{
						std::string error_msg = "\033[31mERROR: User is already an admin.\033[0m\r\n";
						send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
						return (false);
					}
				}
				break ;
			}
			else if (user.is_admin == true)
			{
				for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
				{
					if (ch_it->nickname == argument)
					{
						//pushback the user in admin_list
						channel->operator_list.push_back(argument);

						send(user.fd, "\033[32mSUCCESS: User is now admin.\033[0m\r\n", 31, MSG_NOSIGNAL);
						break ;
					}
				}
			}
		}
	}
	else if (mode == "-o")
	{
		// Expecting format: MODE <channel> -o <nickname>
		//verification if user is in operator_list
		for (std::vector<std::string>::iterator it = channel->operator_list.begin(); it != channel->operator_list.end(); ++it)
		{
			if (*it == user.nickname)
			{
				for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
				{
					if (ch_it->nickname == argument)
					{
						//remove the user from admin_list
						channel->operator_list.erase(it);

						send(user.fd, "\033[32mSUCCESS: User is no longer admin.\033[0m\r\n", 45, MSG_NOSIGNAL);
						break ;
					}
				}
				break ;
			}
			else if (user.is_admin == true)
			{
				for (std::vector<User>::iterator ch_it = channel->users_in_channel.begin(); ch_it != channel->users_in_channel.end(); ++ch_it)
				{
					if (ch_it->nickname == argument)
					{
						//pushback the user in admin_list
						channel->operator_list.erase(it);

						send(user.fd, "\033[32mSUCCESS: User is no longer admin.\033[0m\r\n", 45, MSG_NOSIGNAL);
						break ;
					}
				}
			}
		}
		std::string error_msg = "\033[31mERROR: You are not an admin.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	else if (mode == "+k")
	{
		// Expecting format: MODE <channel> +k <password>
		channel->password = argument;
		std::cout << "Password set to " << argument << " on channel: " << channel->name << std::endl;
		send(user.fd, "\033[32mSUCCESS: Password set successfully!\r\n\033[0m", 47, MSG_NOSIGNAL);
	}
	else if (mode == "-k")
	{
		// Expecting format: MODE <channel> -k
		channel->password = "";
		std::cout << "Password removed from channel: " << channel->name << std::endl;
		send(user.fd, "\033[32mSUCCESS: Password removed successfully!\r\n\033[0m", 51, MSG_NOSIGNAL);
	}
	else if (mode == "+i")
	{
		// Expecting format: MODE <channel> +i
		channel->is_invite_only = true;
		std::cout << "Channel " << channel->name << " is now invite only." << std::endl;
		send(user.fd, "\033[32mSUCCESS: Channel is now invite only.\r\n\033[0m", 48, MSG_NOSIGNAL);
	}
	else if (mode == "-i")
	{
		// Expecting format: MODE <channel> -i
		channel->is_invite_only = false;
		std::cout << "Channel " << channel->name << " is no longer invite only." << std::endl;
		send(user.fd, "\033[32mSUCCESS: Channel is no longer invite only.\033[0m\r\n", 54, MSG_NOSIGNAL);
	}
	else if (mode == "+t")
	{
		// Expecting format: MODE <channel> +t
		channel->is_topic_settable = true;
		std::cout << "Channel " << channel->name << " topic is now settable." << std::endl;
		send(user.fd, "\033[32mSUCCESS: Channel topic is now settable.\033[0m\r\n", 51, MSG_NOSIGNAL);
	}
	else if (mode == "-t")
	{
		// Expecting format: MODE <channel> -t
		channel->is_topic_settable = false;
		std::cout << "Channel " << channel->name << " topic is no longer settable." << std::endl;
		send(user.fd, "\033[32mSUCCESS: Channel topic is no longer settable.\033[0m\r\n", 57, MSG_NOSIGNAL);
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
		send(user.fd, "\033[32mSUCCESS: Channel limit is now set.\033[0m\r\n", 46, MSG_NOSIGNAL);
	}
	else if (mode == "-l")
	{
		// Expecting format: MODE <channel> -l
		channel->limit = 0;
		std::cout << "Channel " << channel->name << " limit is now removed." << std::endl;
		send(user.fd, "\033[32mSUCCESS: Channel limit is now removed.\033[0m\r\n", 50, MSG_NOSIGNAL);
	}
	else
	{
		std::string error_msg = "\033[31mERROR: Invalid mode.\033[0m\r\n";
		send(user.fd, error_msg.c_str(), error_msg.length(), MSG_NOSIGNAL);
		return (false);
	}
	return (true);
}