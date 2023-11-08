/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:34:24 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/07 15:03:13 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
	:_name(""), _password(), _topic(""), _mode(-1), _dm(false), _max_users(0)
{
	std::cout << CYAN << "Channel: Default constructor called" << RESET
		<< std::endl;
}

Channel::Channel(std::string name, std::string password, std::string topic)
	:_name(name), _password(password), _topic(topic), _mode(-1), _dm(false),
	_max_users(0)
{
	std::cout << CYAN << "Channel: Parameter constructor called" << RESET
		<< std::endl;
}

Channel::Channel(Client &user1, Client &user2)
	:_name("DM" + user1.get_nickname() + ":" + user2.get_nickname()), _password(""),
	_topic(_name), _mode(10), _dm(true), _max_users(2)
{
	_clients_in_channel.push_back(user1);
	_clients_in_channel.push_back(user2);
}

Channel::Channel(Channel &copy)
{
	*this = copy;
}

Channel::~Channel() {}

Channel	&Channel::operator=(Channel &copy)
{
	if (this != &copy) // this prevents self-assignment. Ex: Channel c1; c1 = c1;
	{
		_name = copy._name;
		_password = copy._password;
		_topic = copy._topic;
		_mode = copy._mode;
		_dm = copy._dm;
		_max_users = copy._max_users;
		_clients_in_channel = copy._clients_in_channel;
	}
	return (*this);
}

bool	Channel::operator==(std::string copy)
{
	return (_name == copy);
}

std::string	Channel::get_name() const
{
	return (_name);
}

std::string	Channel::get_topic() const
{
	return (_topic);
}

int	Channel::get_mode() const
{
	return _mode;
}

bool	Channel::is_dm() const
{
	return _dm;
}

void	Channel::set_name(std::string name)
{
	_name = name;
}

void	Channel::set_topic(std::string topic)
{
	_topic = topic;
}

void	Channel::set_mode(int mode)
{
	_mode = mode;
}

/* get_clients_in_channel() returns a const reference to the vector of clients
 * in the channel. This is to avoid copying the vector when it is not necessary.
 * The vector is const, so it cannot be modified. */
std::vector<Client> &Channel::get_clients_in_channel(void)
{
	return (_clients_in_channel);
}

Client	*Channel::find_client(const std::string &nickname)
{
	std::vector<Client>::iterator it = find(_clients_in_channel.begin(), _clients_in_channel.end(), nickname);

	if (it == _clients_in_channel.end())
		return NULL;
	return &(*it);
}

void	Channel::message(Client &client, std::string msg)
{
	C_IT		it = _clients_in_channel.begin();
	int			fd;
	std::string	final_msg = ":" + client.get_nickname() + "!"
	+ client.get_username() + "@" + "localhost" + " PRIVMSG "
	+ _name + " :" + msg + "\r\n";

	for (; it < _clients_in_channel.end(); it++)
	{
		if (*it == client)
			continue;
		fd = it->get_client_fd();
		send(fd, final_msg.c_str(), final_msg.length(), MSG_NOSIGNAL);
	}
}
