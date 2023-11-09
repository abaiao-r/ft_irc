/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:34:24 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/09 16:34:27 by abaiao-r         ###   ########.fr       */
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

/* Parameter constructor: name and pass*/
Channel::Channel(std::string name, std::string password)
	:_name(name), _password(password), _topic(""), _mode(-1), 
	_channel_limit(UINT_MAX), _channel_invite_only(false)
{
	std::cout << CYAN << "Channel: Parameter constructor called" << RESET
		<< std::endl;
}

/* Copy constructor */
Channel::Channel(const Channel &copy)
{
	*this = copy;
}

/* Destructor */
Channel::~Channel(void)
{
	std::cout << RED << "Channel: Destructor called" << RESET << std::endl;
}

/* Operator overload */
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

/* Operator overload */
std::string	Channel::get_name(void) const
{
	return (_name);
}

/* set_name() sets the name of the channel. */
void	Channel::set_name(std::string name)
{
	_name = name;
}

/* get_password() returns the password of the channel. */
std::string	Channel::get_password(void) const
{
	return (_password);
}

/* set_password() sets the password of the channel. */
int	Channel::set_password(std::string password)
{
	_password = password;
}

/* get_topic() returns the topic of the channel. */
std::string	Channel::get_topic(void) const
{
	return (_topic);
}

/* set_topic() sets the topic of the channel. */
void	Channel::set_topic(std::string topic)
{
	_topic = topic;
}

/* get_mode() returns the mode of the channel. */
int	Channel::get_mode(void) const
{
	return (_mode);
}

/* set_mode() sets the mode of the channel. */
void	Channel::set_mode(int mode)
{
	_mode = mode;
}

/* get_channel_limit() returns the maximum number of users allowed in the
 * channel. */
unsigned int	Channel::get_channel_limit(void) const
{
	return (_channel_limit);
}

/* set_channel_limit() sets the maximum number of users allowed in the channel.*/
void	Channel::set_channel_limit(unsigned int limit)
{
	_channel_limit = limit;
}

/* get_channel_invite_only() returns true if the channel is invite only. */
bool	Channel::get_channel_invite_only() const
{
	return (_channel_invite_only);
}

/* set_channel_invite_only() sets the channel to invite only. */
void	Channel::set_channel_invite_only(bool invite_only)
{
	_channel_invite_only = invite_only;
}

/* get_clients_in_channel() returns a vector of clients in the channel. */
std::vector<Client>	Channel::get_clients_in_channel(void)
{
	return (_clients_in_channel);
}

/* add_client() adds a client to the channel. */
void	Channel::add_client(Client &client)
{
	_clients_in_channel.push_back(client);
}

/* remove_client() removes a client from the channel. */
void	Channel::remove_client(Client &client)
{
	C_IT	it = _clients_in_channel.begin();

	for (; it < _clients_in_channel.end(); it++)
	{
		if (*it == client)
		{
			_clients_in_channel.erase(it);
			return ;
		}
	}
}

/* get_clients_banned() returns a vector of clients banned from the channel. */
std::vector<Client> &Channel::get_clients_banned(void)
{
	return (_clients_banned);
}

/* add_client_banned() adds a client to the list of banned clients. */
void	Channel::add_client_banned(Client &client)
{
	_clients_banned.push_back(client);
}

/* remove_client_banned() removes a client from the list of banned clients. */
void	Channel::remove_client_banned(Client &client)
{
	C_IT	it = _clients_banned.begin();

	for (; it < _clients_banned.end(); it++)
	{
		if (*it == client)
		{
			_clients_banned.erase(it);
			return ;
		}
	}
}

/* get_clients_operator_channel() returns a vector of clients that are
 * operators in the channel. */
std::vector<Client> &Channel::get_clients_operator_channel(void)
{
	return (_clients_operator_channel);
}

/* add_client_operator_channel() adds a client to the list of operators in the
 * channel. */
void	Channel::add_client_operator_channel(Client &client)
{
	_clients_operator_channel.push_back(client);
}

/* remove_client_operator_channel() removes a client from the list of operators
 * in the channel. */
void	Channel::remove_client_operator_channel(Client &client)
{
	C_IT	it = _clients_operator_channel.begin();

	for (; it < _clients_operator_channel.end(); it++)
	{
		if (*it == client)
		{
			_clients_operator_channel.erase(it);
			return ;
		}
	}
}

/* get_clients_invited_to_channel() returns a vector of clients that have voice
 * in the channel. */
std::vector<Client> &Channel::get_clients_invited_to_channel(void)
{
	return (_clients_invited_to_channel);
}

/* add_client_invited_to_channel() adds a client to the list of clients that
 * have voice in the channel. */
void	Channel::add_client_to_clients_invited_vector(Client &client)
{
	_clients_invited_to_channel.push_back(client);
}

/* remove_client_invited_to_channel() removes a client from the list of clients
 * that have voice in the channel. */
void	Channel::remove_client_from_clients_invited_vector(Client &client)
{
	C_IT	it = _clients_invited_to_channel.begin();

	for (; it < _clients_invited_to_channel.end(); it++)
	{
		if (*it == client)
		{
			_clients_invited_to_channel.erase(it);
			return ;
		}
	}
}

/* find_client() returns a pointer to the client if it is found in the channel.
 * Otherwise, it returns NULL. */
Client	*Channel::find_client(Client &client)
{
	std::vector<Client>::iterator it = find(_clients_in_channel.begin(), _clients_in_channel.end(), client);

	if (it == _clients_in_channel.end())
		return (NULL);
	return (&(*it));
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

/* find_clients_operator_channel: looks for a client in the vector of clients*/
Client	*Channel::find_clients_operator_channel(Client &client)
{
	std::vector<Client>::iterator it = find(_clients_operator_channel.begin(), _clients_operator_channel.end(), client);

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


/* find_clients_operator_channel(std::string &nickname_to_find):
** 1. Looks for a client in the vector of clients that are operator of the channel.
** 2. Returns a pointer to the client if it is found in the vector.
** 3. Otherwise, it returns NULL.
*/
Client	*Channel::find_clients_operator_channel(std::string &nickname_to_find)
{
	std::vector<Client>::iterator it = _clients_operator_channel.begin();

	for (; it < _clients_operator_channel.end(); it++)
	{
		if (it->get_nickname() == nickname_to_find)
			return (&(*it));
	}
	return (NULL);
}

