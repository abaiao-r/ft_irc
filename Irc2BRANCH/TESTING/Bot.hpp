/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:29:04 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 16:11:34 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "colours.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Bot
{
  private:
	std::string 				_name;
	std::vector<std::string> 	_naughty_words;
	bool 						_loadNaughtyWordsFromFile;
	Bot();
	Bot(const Bot &copy);
	Bot &operator=(const Bot &copy);

  public:
	Bot(std::string name);
	~Bot();

	std::string		get_name() const;
	void 			setLoadNaughtyWordsFromFile();
	bool 			getLoadNaughtyWordsFromFile() const;
	void 			readNaughtyWordsFromFile(const std::string &filename);
	void 			greeting(Client &client);
	void 			cmd_help(Client &client);
	int 			sendBotMessage(int client_fd, const std::string &successMessage);
	bool 			big_brother(Channel &channel, Client &client, std::string msg);
};
