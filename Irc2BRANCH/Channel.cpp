/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:34:24 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/09 13:44:55 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
	:_name(""), _password(""), _topic(""), _mode(-1), 
	_channel_limit(0), _channel_invite_only(false)
{
	std::cout << CYAN << "Channel: Default constructor called" << RESET
		<< std::endl;
}

/* Parameter constructor: only name as parameter*/
Channel::Channel(std::string name)
{
	std::cout << CYAN << "Channel: Parameter constructor called" << RESET
		<< std::endl;
	_name = name;
	_password = "";
	_topic = "";
	_mode = -1;
	_channel_limit = UINT_MAX;
	_channel_invite_only = false;
}

/*/* Parameter constructor: name and pass*/
Channel::Channel(std::string name, std::string password)
	:_name(name), _password(password), _topic(""), _mode(-1), 
	_channel_limit(UINT_MAX), _channel_invite_only(false)
{
	std::cout << CYAN << "Channel: Parameter constructor called" << RESET
		<< std::endl;
}

Channel::Channel(const Channel &copy)
{
	*this = copy;
}

Channel::~Channel(void)
{
	std::cout << RED << "Channel: Destructor called" << RESET << std::endl;
}

Channel	&Channel::operator=(const Channel &copy)
{
	if (this != &copy) // this prevents self-assignment. Ex: Channel c1; c1 = c1;
	{
		_name = copy._name;
		_password = copy._password;
		_topic = copy._topic;
		_mode = copy._mode;
		_channel_invite_only = copy._channel_invite_only;
		_channel_limit = copy._channel_limit;
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

/* set_password() sets the password of the channel. */
void	Channel::set_password(std::string password)
{
	//add check for validation of password
	_password = password;
}

/* get_password() returns the password of the channel. */
std::string	Channel::get_password() const
{
	return (_password);
}

/* set_channel_invite_only() sets the channel to invite only. */
void	Channel::set_channel_invite_only(bool invite_only)
{
	_channel_invite_only = invite_only;
}

/* get_channel_invite_only() returns true if the channel is invite only. */
bool	Channel::get_channel_invite_only() const
{
	return (_channel_invite_only);
}

/* get_channel_limit() returns the maximum number of users allowed in the
 * channel. */
unsigned int	Channel::get_channel_limit() const
{
	return (_channel_limit);
}

/* set_channel_limit() sets the maximum number of users allowed in the channel.*/
void	Channel::set_channel_limit(unsigned int limit)
{
	_channel_limit = limit;
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

/* add_client() adds a client to the channel. */
void	Channel::add_client(Client &client)
{
	_clients_in_channel.push_back(client);
}


/* get_clients_banned:
** 1. Returns the vector of clients that are banned.
*/
std::vector<Client> &Channel::get_clients_banned(void)
{
	return (_clients_banned);
}

Client	*Channel::find_banned_client(const std::string &client_banned)
{
	std::vector<Client>::iterator it = find(_clients_banned.begin(), _clients_banned.end(), client_banned);

	if (it == _clients_banned.end())
		return (NULL);
	return &(*it);
}

/* get_clients_operator_channel:
** 1. Returns the vector of clients that are operator of the channel.
*/
std::vector<Client> &Channel::get_clients_operator_channel(void)
{
	return (_clients_operator_channel);
}

Client	*Channel::find_clients_operator_channel(const std::string &clients_operator_channel)
{
	std::vector<Client>::iterator it = find(_clients_operator_channel.begin(), _clients_operator_channel.end(), clients_operator_channel);

	if (it == _clients_operator_channel.end())
		return (NULL);
	return &(*it);
}

/* get_clients_invited_to_channel:
** 1. Returns the vector of clients that were invited to the channel
*/
std::vector<Client> &Channel::get_clients_invited_to_channel(void)
{
	return (_clients_invited_to_channel);
}

Client	*Channel::find_clients_invited_to_channel(const std::string &clients_invited_to_channel)
{
	std::vector<Client>::iterator it = find(_clients_invited_to_channel.begin(), _clients_invited_to_channel.end(), clients_invited_to_channel);

	if (it == _clients_invited_to_channel.end())
		return (NULL);
	return &(*it);
}

/* add_client_to_clients_operator_vector:
** 1. Adds a client to the vector of clients that are operator of the channel.
*/
void	Channel::add_client_to_clients_operator_vector(Client &client)
{
	_clients_operator_channel.push_back(client);
}
