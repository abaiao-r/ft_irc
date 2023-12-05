/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:32:48 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/05 10:09:30 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot()
{
	_loadNaughtyWordsFromFile = false;
	readNaughtyWordsFromFile("naughty_words.txt");
}

Bot::Bot(std::string name): _name(name)
{
	_loadNaughtyWordsFromFile = false;
	readNaughtyWordsFromFile("naughty_words.txt");
}

Bot::Bot(const Bot &copy)
{
	_name = copy._name;
	_loadNaughtyWordsFromFile = copy._loadNaughtyWordsFromFile;
	_naughty_words = copy._naughty_words;
}

Bot &Bot::operator=(const Bot &copy)
{
	
	if (this != &copy)
	{
		_name = copy._name;
		_loadNaughtyWordsFromFile = copy._loadNaughtyWordsFromFile;
		_naughty_words = copy._naughty_words;
	}
	return (*this);
}

Bot::~Bot()
{
	std::cout << RED << "BOT destructor called" << RESET << std::endl;
}

std::string	Bot::get_name() const
{
	return _name;
}

void Bot::setLoadNaughtyWordsFromFile()
{
	_loadNaughtyWordsFromFile = true;
}

bool Bot::getLoadNaughtyWordsFromFile() const
{
	return (_loadNaughtyWordsFromFile);
}

void Bot::readNaughtyWordsFromFile(const std::string &filename)
{
	std::ifstream file(filename.c_str());

	if (file.is_open())
	{
		// use streams to store all the line in the vector but the
		// skip leading whitespaces and and skip the whitespaces after the last printable character
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string word;

			// skip leading whitespaces
			iss >> std::ws;
			//store all the line in word, use getline to get the whitespaces
			while (std::getline(iss, word))
			{
				// lowercase all the line
				 std::transform(word.begin(), word.end(), word.begin(), tolower);
				//remove the whitespaces after the last printable character
				word.erase(word.find_last_not_of(" \t\n\r\f\v") + 1);
				_naughty_words.push_back(word);
			}
			// clear the stream
			iss.clear();
		}
		file.close();
		setLoadNaughtyWordsFromFile();
	}
	else
	{
		std::cerr << RED << "Unable to open file: " << filename << RESET << std::endl;
	}
}

void	Bot::greeting(Client &client)
{
	sendBotMessage(client.get_client_fd(), "Hello, I'm Clippy. Type HALP to check what you can do.\r\n");
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
	if (getLoadNaughtyWordsFromFile() == false)
	{
		std::cerr << RED << "Error: " << RESET << "Naughty words not loaded" 
			<< std::endl;
		return (false);
	}
	std::string	chan_msg = ":" + _name + "!" + _name + "@" + "localhost" 
	+ " PRIVMSG " + channel.get_name() + " :" + client.get_nickname();

	std::transform(msg.begin(), msg.end(), msg.begin(), tolower);
	// print the vector of naughty words DEBUG
	for (std::vector<std::string>::iterator it1 = _naughty_words.begin(); it1 != _naughty_words.end(); it1++)
	{
		std::cout << ORANGE << "|" << *it1 << "|" << RESET <<  std::endl;
	}

	// iterate through the vector of naughty words
	for (std::vector<std::string>::iterator it = _naughty_words.begin(); it != _naughty_words.end(); it++)
	{
		std::string word;
		//word equals to all the word in *it
		word = *it;
		// check if the message contains a naughty word
		if (msg.find(word) != std::string::npos)
		{
			//debug
			std::cout << RED << "Naughty word found: " << word << RESET << std::endl;
			// if the client has 2 strikes, ban him
			if (client.get_strikes() == 1)
			{
				sendBotMessage(client.get_client_fd(), "Strike two for bad language. You are now banned.\r\n");
				chan_msg += ": Has been banned (Strike two for bad language).\r\n";
				channel.info_message(chan_msg);
				return true;
			}
			// if the client has 1 strike, warn him
			sendBotMessage(client.get_client_fd(), "Strike one for bad language. Next time you will be banned.\r\n");
			chan_msg += ": Strike one for bad language. Next time there will be a ban.\r\n";
			channel.info_message(chan_msg);
			client.set_strikes(1);
			return false;
		}
	}
	return false;
}
