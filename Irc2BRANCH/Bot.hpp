/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:29:04 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/29 12:48:29 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"

class Bot
{
	private:
		std::string	_name;
		std::string	_naughty_words[5];
		Bot();
		Bot(const Bot &copy);
		Bot &operator=(const Bot &copy);
	public:
		Bot(std::string name);
		~Bot();
		void	greeting(Client &client);
		void	cmd_help(Client &client);
		int 	sendBotMessage(int client_fd, const std::string	&successMessage);
		bool	big_brother(Channel *channel, Client &client, std::string msg);
};
