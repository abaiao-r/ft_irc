/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:34:24 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/06 16:13:48 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
	:_name(""), _password(), _topic(""), _mode(-1), _dm(false), _max_users(0) {}

Channel::Channel(std::string name, std::string password, std::string topic)
	:_name(name), _password(password), _topic(topic), _mode(-1), _dm(false),
	_max_users(0) {}

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
	_name = copy._name;
	_password = copy._password;
	_topic = copy._topic;
	_mode = copy._mode;
	_dm = copy._dm;
	_max_users = copy._max_users;
	_clients_in_channel = copy._clients_in_channel;
	return *this;
}

bool	Channel::operator==(std::string copy)
{
	return (_name == copy);
}

std::string	Channel::get_name() const
{
	return _name;
}

std::string	Channel::get_topic() const
{
	return _topic;
}

int	Channel::get_mode() const
{
	return _mode;
}

std::vector<Client>	&Channel::get_clients_in_channel()
{
	return (_clients_in_channel);
}

bool	Channel::is_dm() const
{
	return _dm;
}

void	Channel::set_name(const Client &user, std::string name)
{
	// if (!user.is_operator())
	// 	return;
	_name = name;
}

void	Channel::set_topic(const Client &user, std::string topic)
{
	// if (!user.is_operator())
	// 	return;
	_topic = topic;
}

void	Channel::set_mode(const Client &user, int mode)
{
	// if (!user.is_operator())
	// 	return;
	_mode = mode;
}
