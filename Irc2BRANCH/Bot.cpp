/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:32:48 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/30 14:51:44 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot()
{
	_naughty_words[0] = "shit";
	_naughty_words[1] = "kys";
	_naughty_words[2] = "fuck";
	_naughty_words[3] = "nazi";
	_naughty_words[4] = "semprini";
}

Bot::Bot(std::string name): _name(name)
{
	_naughty_words[0] = "shit";
	_naughty_words[1] = "kys";
	_naughty_words[2] = "fuck";
	_naughty_words[3] = "nazi";
	_naughty_words[4] = "semprini";
}

Bot::Bot(const Bot &copy)
{
	*this = copy;
}

Bot &Bot::operator=(const Bot &copy)
{
	_name = copy._name;
	_naughty_words[0] = copy._naughty_words[0];
	_naughty_words[1] = copy._naughty_words[1];
	_naughty_words[2] = copy._naughty_words[2];
	_naughty_words[3] = copy._naughty_words[3];
	_naughty_words[4] = copy._naughty_words[4];
	return *this;
}

Bot::~Bot() {}

std::string	Bot::get_name() const
{
	return _name;
}

void	Bot::greeting(Client &client)
{
	sendBotMessage(client.get_client_fd(), "Hello, I'm " + _name + ". Type HALP to check what you can do.\r\n");
}

void	Bot::cmd_help(Client &client)
{
	int fd = client.get_client_fd();
	sendBotMessage(fd, "Available commands:\r\n");
	sendBotMessage(fd, "PASS <password>\r\n");
	sendBotMessage(fd, "USER <username>\r\n");
	sendBotMessage(fd, "NICK <nickname>\r\n");
	sendBotMessage(fd, "JOIN <#channel>\r\n");
	sendBotMessage(fd, "PRIVMSG <nickname | #channel>\r\n");
	if (client.get_is_admin())
	{
		sendBotMessage(fd, "Available operator commands:\r\n");
		sendBotMessage(fd, "KICK <#channel> <nickname> [reason]\r\n");
		sendBotMessage(fd, "INVITE <#channel> <nickname>\r\n");
		sendBotMessage(fd, "TOPIC <#channel> <topic>\r\n");
		sendBotMessage(fd, "MODE <#channel> <+|-><i|t|k|o|l> [argument]\r\n");
	}
}

int Bot::sendBotMessage(int client_fd, const std::string &msg)
{
	std::string full_msg = ":" + _name + "!" + _name + "@localhost PRIVMSG " + ": " + msg;
	if (send(client_fd, full_msg.c_str(), full_msg.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}

bool	Bot::big_brother(Channel &channel, Client &client, std::string msg)
{
	std::string	chan_msg = ":" + _name + "!" + _name + "@" + "localhost" + " PRIVMSG "
							+ channel.get_name() + " :" + client.get_nickname();

	std::transform(msg.begin(), msg.end(), msg.begin(), tolower);
	for (unsigned int i = 0; i <= _naughty_words->size(); i++)
	{
		if (msg.find(_naughty_words[i]) != std::string::npos)
		{
			if (client.get_strikes() == 1)
			{
				sendBotMessage(client.get_client_fd(), "Strike two for bad language. You are now banned.\r\n");
				chan_msg += ": Has been banned (Strike two for bad language).\r\n";
				channel.info_message(chan_msg);
				return true;
			}
			sendBotMessage(client.get_client_fd(), "Strike one for bad language. Next time you will be banned.\r\n");
			chan_msg += ": Strike one for bad language. Next time there will be a ban.\r\n";
			channel.info_message(chan_msg);
			client.set_strikes(1);
			return false;
		}
	}
	return false;
}
