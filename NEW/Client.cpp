/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:34:09 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/03 17:14:28 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* Default constructor */
Client::Client(void)
	: _client_fd(-1), _authenticated(false), _tries(0)
{
	std::cout << CYAN << "Client: Default constructor called" << RESET 
		<< std::endl;
}

/* copy constructor: is private  so we can't copy */
Client::Client(const Client &copy)
	: _client_fd(copy._client_fd), _authenticated(copy._authenticated),
	_tries(copy._tries), _client_addr(copy._client_addr)
{
	std::cout << CYAN << "Client:copy constructor called" << RESET
		<< std::endl;
	*this = copy;
}

/* destructor */
Client::~Client(void)
{
	std::cout << RED << "Client: Destructor called" << RESET 
		<< std::endl;
}

/* assignment operator: is private so we can't copy */
Client &Client::operator=(const Client &copy)
{
	std::cout << CYAN << "Client: Assignment operator called" << RESET
		<< std::endl;
	_client_fd = copy._client_fd;
	_client_addr = copy._client_addr;
	_authenticated = copy._authenticated;
	_tries = copy._tries;
	return *this;
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

bool Client::operator==(const Client &comp)
{
	return (_client_fd == comp._client_fd);
}

bool Client::operator==(const std::string &comp)
{
	return (_nickname == comp);
}

bool Client::operator==(const int &comp)
{
	return (_client_fd == comp);
}

int	Client::pass_counter(int flag, int val)
{
	if (flag == 1)
		return _tries++;
	if (flag == 2)
		return (_tries = val);
	return _tries;
}
