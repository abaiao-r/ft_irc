/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:29:04 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/26 12:24:40 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include "Client.hpp"

class Bot
{
	private:
		std::string	_name;
		Bot();
		Bot(const Bot &copy);
		Bot &operator=(const Bot &copy);
	public:
		Bot(std::string name);
		~Bot();
		void	greeting(Client &client);
		void	cmd_help(Client &client);
		int 	sendBotMessage(int client_fd, const std::string	&successMessage);
};
