/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:34:09 by joao-per          #+#    #+#             */
/*   Updated: 2023/12/06 19:46:26 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* Default constructor */
Client::Client(void)
	: _client_fd(-1), _authenticated(false), _registered(false), _tries(0),
	_strikes(0), _nickname(""), _username(""), _is_admin(false)
{
	//std::cout << CYAN << "Client: Default constructor called" << RESET 
	//	<< std::endl;
}

/* copy constructor */
Client::Client(const Client &copy)
{
	//std::cout << CYAN << "Client: Copy constructor called" << RESET
	//	<< std::endl;
	*this = copy;
}

/* destructor */
Client::~Client(void)
{
	//std::cout << RED << "Client: Destructor called" << RESET 
	//	<< std::endl;
}

/* assignment operator */
Client &Client::operator=(const Client &copy)
{
	//std::cout << CYAN << "Client: Assignment operator called" << RESET
	//	<< std::endl;
	if (this != &copy) // this prevents self-assignment. Ex: Client c1; c1 = c1;
	{
		_client_fd = copy._client_fd;
		_client_addr = copy._client_addr;
		_authenticated = copy._authenticated;
		_registered = copy._registered;
		_tries = copy._tries;
		_strikes = copy._strikes;
		_nickname = copy._nickname;
		_username = copy._username;
		_is_admin = copy._is_admin;
	}
	return (*this);
}

/* get client_fd: return the client_fd */
const int &Client::get_client_fd(void) const
{
	return (_client_fd);
}

/* set client_fd: set the client_fd */
void	Client::set_client_fd(const int &value)
{
	_client_fd = value;
}

/* get_registered: return the registered */
const bool &Client::get_registered(void) const
{
	return (_registered);
}

/* set_registered: set the registered */
void	Client::set_registered(const bool &value)
{
	_registered = value;
}

/* get_strikes: return the strikes */
const int &Client::get_strikes(void) const
{
	return (_strikes);
}

/* set_strikes: set the strikes */
void Client::set_strikes(const int &value)
{
	_strikes = value;
}


/*get_authenticated: return the authenticated */
const bool &Client::get_authenticated(void) const
{
	return (_authenticated);
}

/* set_authenticated: set the authenticated */
void	Client::set_authenticated(const bool &value)
{
	_authenticated = value;
}

/*get_nickname: return the nickname */
const std::string &Client::get_nickname(void) const
{
	return (_nickname);
}

/* set_nickname: set the nickname */
void	Client::set_nickname(const std::string &value)
{
	_nickname = value;
}

/*get_username: return the username */
const std::string &Client::get_username(void) const
{
	return (_username);
}

/* set_username: set the username */
void	Client::set_username(const std::string &value)
{
	_username = value;
}

/* get_is_admin: return the is_admin */
const bool &Client::get_is_admin(void) const
{
	return (_is_admin);
}

/* set_is_admin: set the is_admin */
void	Client::set_is_admin(const bool &value)
{
	_is_admin = value;
}

/* get_cmd: return the cmd_buffer */
std::string	Client::get_cmd() const
{
	return _cmd_buffer;
}

/* add_to_cmd: add to the cmd_buffer */
void	Client::add_to_cmd(std::string str)
{
	_cmd_buffer += str;
}

/* clear_cmd: clear the cmd_buffer */
void	Client::clear_cmd()
{
	_cmd_buffer.clear();
}

/* operator == (overload): compare two clients */
bool Client::operator==(const Client &comp)
{
	return (_client_fd == comp._client_fd);
}

/* operator == (overload): compare a client with a string */
bool Client::operator==(const std::string &nick)
{
	return (_nickname == nick);
}

/* operator == (overload): compare a client with a int */
bool Client::operator==(const int &comp)
{
	return (_client_fd == comp);
}

/* pass_counter: if flag == 1, increment tries, if flag == 2, set tries to val
 * if flag == 0, return tries */
int	Client::pass_counter(int flag, int val)
{
	if (flag == 1)
		return _tries++;
	if (flag == 2)
		return (_tries = val);
	return _tries;
}
