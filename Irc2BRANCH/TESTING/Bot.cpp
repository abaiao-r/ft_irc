/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 09:32:48 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 18:18:23 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

/* Default constructor: */
Bot::Bot()
{
	//std::cout << CYAN << "BOT default constructor called" << RESET << std::endl;
	_loadNaughtyWordsFromFile = false;
	readNaughtyWordsFromFile("naughty_words.txt");
}

/* Parametric constructor: */
Bot::Bot(std::string name): _name(name)
{
	//std::cout << CYAN << "BOT parametric constructor called" << RESET << std::endl;
	_loadNaughtyWordsFromFile = false;
	readNaughtyWordsFromFile("naughty_words.txt");
}

/* Copy constructor: */
Bot::Bot(const Bot &copy)
{
	//std::cout << CYAN << "BOT copy constructor called" << RESET << std::endl;
	_name = copy._name;
	_loadNaughtyWordsFromFile = copy._loadNaughtyWordsFromFile;
	_naughty_words = copy._naughty_words;
}

/* Assignation operator: */
Bot &Bot::operator=(const Bot &copy)
{
	//std::cout << CYAN << "BOT assignation operator called" << RESET << std::endl;
	if (this != &copy)
	{
		_name = copy._name;
		_loadNaughtyWordsFromFile = copy._loadNaughtyWordsFromFile;
		_naughty_words = copy._naughty_words;
	}
	return (*this);
}

/* Destructor: */
Bot::~Bot()
{
	//std::cout << RED << "BOT destructor called" << RESET << std::endl;
}

/* get_name: returns the name of the bot */
std::string	Bot::get_name() const
{
	return (_name);
}

/* set_name: sets the name of the bot */
void Bot::setLoadNaughtyWordsFromFile()
{
	_loadNaughtyWordsFromFile = true;
}

/* getLoadNaughtyWordsFromFile: returns the flag of loadNaughtyWordsFromFile */
bool Bot::getLoadNaughtyWordsFromFile() const
{
	return (_loadNaughtyWordsFromFile);
}

/*readNaughtyWordsFromFile:
** 1. checks if it can open the file
** if yes:
** 2. store all the naughty words in a vector
** 3. sets the flag of loadNaughtyWordsFromFile to true.
** if no
** 4. it sends error message and the bot will not moderate language. */
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

/* greeting: sends a greeting message to the client */
void	Bot::greeting(Client &client)
{
	sendBotMessage(client.get_client_fd(), "Hello, I'm Clippy. Type HALP to check what you can do.\r\n");
}

/* cmd_help: sends a list of available commands to the client */
void	Bot::cmd_help(Client &client)
{
	int fd = client.get_client_fd();
	sendBotMessage(fd, "Available commands:\r\n");
	sendBotMessage(fd, "PASS <password>\r\n");
	sendBotMessage(fd, "USER <username>\r\n");
	sendBotMessage(fd, "NICK <nickname>\r\n");
	sendBotMessage(fd, "JOIN <#channel>\r\n");
	sendBotMessage(fd, "PRIVMSG <nickname | #channel> <message>\r\n");
	if (client.get_is_admin())
	{
		sendBotMessage(fd, "Available operator commands:\r\n");
		sendBotMessage(fd, "KICK <#channel> <nickname> [reason]\r\n");
		sendBotMessage(fd, "INVITE <#channel> <nickname>\r\n");
		sendBotMessage(fd, "TOPIC <#channel> [topic]\r\n");
		sendBotMessage(fd, "MODE <#channel> <+|-><i|t|k|o|l> [argument]\r\n");
	}
}

/* sendBotMessage: sends a message to the client */
int Bot::sendBotMessage(int client_fd, const std::string &msg)
{
	std::string full_msg = ":" + _name + "!" + _name + "@localhost PRIVMSG " 
		+ ": " + msg;
	if (send(client_fd, full_msg.c_str(), full_msg.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}

/* big_brother: checks if the message contains a naughty word
** 1. checks if the naughty words are loaded
** 2. checks if the message contains a naughty word by iterating through the vector
** 3. if yes, it sends a warning message to the client and the channel
** at the second time it kicks the client from the channel
** 4. if no, it sends the message to the channel */
bool	Bot::big_brother(Channel &channel, Client &client, std::string msg)
{
	std::istringstream iss(msg);
	std::vector<std::string> words;
	std::string word;

	if (getLoadNaughtyWordsFromFile() == false)
	{
		std::cerr << RED << "Error: " << RESET << "Naughty words not loaded" 
			<< std::endl;
		return (false);
	}
	std::string	chan_msg = ":" + _name + "!" + _name + "@" + "localhost" 
	+ " PRIVMSG " + channel.get_name() + " :" + client.get_nickname();

	std::transform(msg.begin(), msg.end(), msg.begin(), tolower);
	// parse the msg
	while (iss >> word)
		words.push_back(word);
	// iterate through the vector of naughty words
	for (std::vector<std::string>::iterator it = _naughty_words.begin(); 
		it != _naughty_words.end(); it++)
	{
		// check if the message contains a naughty word word by word
		for (std::vector<std::string>::iterator it2 = words.begin(); 
			it2 != words.end(); it2++)
		{
			std::string  match = *it2;
			// if the message contains a naughty word
			if (match == *it)
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
	}
	return false;
}
