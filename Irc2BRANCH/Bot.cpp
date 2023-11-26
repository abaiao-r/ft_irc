/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:32:48 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/26 12:41:11 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot() {}

Bot::Bot(std::string name): _name(name) {}

Bot::Bot(const Bot &copy)
{
	*this = copy;
}

Bot &Bot::operator=(const Bot &copy)
{
	_name = copy._name;
	return *this;
}

Bot::~Bot() {}

void	Bot::greeting(Client &client)
{
	sendBotMessage(client.get_client_fd(), "Hello, I'm Clippy. Type HALP to check what you can do.\r\n");
}

void	Bot::cmd_help(Client &client)
{
	int fd = client.get_client_fd();
	sendBotMessage(fd, ": Available commands:\r\n");
	sendBotMessage(fd, ": PASS <password>\r\n");
	sendBotMessage(fd, ": USER <username>\r\n");
	sendBotMessage(fd, ": NICK <nickname>\r\n");
	sendBotMessage(fd, ": JOIN <#channel>\r\n");
	sendBotMessage(fd, ": PRIVMSG <nickname | #channel>\r\n");
	if (client.get_is_admin())
	{
		sendBotMessage(fd, ": Available operator commands:\r\n");
		sendBotMessage(fd, ": KICK <#channel> <nickname> [reason]\r\n");
		sendBotMessage(fd, ": INVITE <#channel> <nickname>\r\n");
		sendBotMessage(fd, ": TOPIC <#channel> <topic>\r\n");
		sendBotMessage(fd, ": MODE <#channel> <+|-><i|t|k|o|l> [argument]\r\n");
	}
}

int Bot::sendBotMessage(int client_fd, const std::string &msg)
{
	std::string full_msg = ":" + _name + "!" + _name + "@localhost PRIVMSG " + msg;
	if (send(client_fd, full_msg.c_str(), full_msg.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}
