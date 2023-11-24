/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/24 08:58:26 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	Server::_loop_state = 1;

/* Default constructor */
Server::Server(): _port(0), _server_fd(-1)
{
	std::cout << CYAN << "Default constructor Server called" << RESET 
		<< std::endl;
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET; // IPv4
	_address.sin_addr.s_addr = INADDR_ANY; // Any available network interface
	_address.sin_port = htons(_port); // Port
}
/* Parameter constructor */
Server::Server(int port, std::string password): _port(port), _password(password)
{
	std::signal(SIGINT, signal_handler);
	std::cout << CYAN << "Parameter constructor Server called" << RESET 
		<< std::endl;
	// check password
	if (password_checker(password) == 1)
		throw(std::runtime_error(""));
	memset(&_address, 0, SOCKLEN);
	//sets the address family of the socket to IPv4.
	_address.sin_family = AF_INET;
	//the socket accepts connections from any available network interface on the machine.
	_address.sin_addr.s_addr = INADDR_ANY;
	//htons()converts the port number from host byte order to network byte order, which is the byte order used by the network protocol. This ensures that the port number is stored in the correct byte order for communication over the network.
	_address.sin_port = htons(_port);
	init_server();
}

/* Copy constructor */
Server::Server(const Server &src)
{
	std::cout << CYAN << "Copy constructor Server called" << RESET 
		<< std::endl;
	*this = src;
}

/* Destructor */
Server::~Server()
{
	std::cout << RED << "Destructor Server called" << RESET 
		<< std::endl;
	C_IT end = _clients.end();
	for (C_IT it = _clients.begin(); it != end; it++)
		close(it->get_client_fd());
	close(_server_fd);
	close(_epoll_fd);
	std::signal(SIGINT, SIG_DFL);
}

/* Assignment operator overload (Update) */
Server &Server::operator=(const Server &src)
{
	std::cout << YELLOW << "Assignment operator Server called" << RESET 
		<< std::endl;
	if (this != &src)
	{
		_port = src._port;
		_server_fd = src._server_fd;
		_address = src._address;
	}
	return *this;
}

/* start_listening: Starts listening for connections
** listen() marks the socket referred to by server_fd as a passive socket, that
** is, as a socket that will be used to accept incoming connection requests
** using accept(). The socket must be bound to a local address using bind()
** before this call can be made. The backlog parameter defines the maximum
** length to which the queue of pending connections for server_fd may grow.
** If a connection request arrives when the queue is full, the client may
** receive an error with an indication of ECONNREFUSED or, if the underlying
** protocol supports retransmission, the request may be ignored so that a
** later reattempt at connection succeeds.
*/
int Server::start_listening(void)
{
	if (listen(_server_fd, MAX_REQUESTS))
	{
		close(_server_fd);
		return (-1);
	}
	std::cout << GREEN << "Server listening on port " << _port << RESET
		<< std::endl;
	return 0;
}

/* bind_socket: Binds the socket to the address and port number
** bind() assigns the address specified by addr to the socket referred to by
** the file descriptor server_fd. addrlen specifies the size, in bytes, of the
** address structure pointed to by addr. Traditionally, this operation is
** called “assigning a name to a socket”. It is normally necessary to assign a
** local address using bind() before a SOCK_STREAM socket may receive
** connections.
*/
int Server::bind_socket(void)
{
	if (bind(_server_fd, (struct sockaddr*)&_address, SOCKLEN))
	{
		close(_server_fd);
		return -1;
	}
	return 0;
}

/* set_socket_options: Sets the socket options
** Note: setsockopt() sets the value of a socket option. The parameters are:
** 1. Socket file descriptor
** 2. Level: SOL_SOCKET (socket-level options)
** 3. Option name: SO_REUSEADDR (reuses the address and port)
** 4. Option value: 1 (true)
** 5. Option length: sizeof(int)
** Returns true if the socket options are set successfully, false otherwise
*/ 
int Server::set_socket_options(void) // use int
{
	int opt = 1;
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, 
		sizeof(opt)) == -1) 
	{
		close (this->_server_fd);
		return -1;
	}
	return 0;
}

int	Server::unblock_socket(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "Error when setting O_NONBLOCK flag\n";
		return -1;
	}
	return 0;
}

/* create_socket: Creates the socket
** Note: socket() returns a file descriptor (int). The parameters are:
** 1. Domain: AF_INET (IPv4) or AF_INET6 (IPv6)
** 2. Type: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
** 3. Protocol: 0 (IP)
** Returns true if the socket is created successfully, false otherwise
*/
int Server::create_socket(void) // use int
{
	_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_server_fd == -1)
		return -1;
	return 0;
}

/* init_server: Initializes the server
** 1. Creates the socket
** 2. Sets the socket options
** 3. Binds the socket
** 4. Starts listening
** Returns true if all steps are successful, false otherwise
*/
void Server::init_server(void) // use int
{
	if (create_socket() == -1)
		throw(std::runtime_error("Could not create socket"));
	if (unblock_socket(_server_fd) == -1)
		throw(std::runtime_error("Error when setting flags"));
	if (set_socket_options() == -1)
		throw(std::runtime_error("Could not set socket options"));
	if (bind_socket() == -1) 
		throw(std::runtime_error("Could not bind server fd"));
	if (start_listening() == -1)
		throw(std::runtime_error("Error while listening"));
	create_epoll();
}

void	Server::create_epoll()
{
	memset(&_events, 0, sizeof(_events));
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw(std::runtime_error("Error when creating epoll"));
	memset(&_main_event, 0, sizeof(_main_event));
	_main_event.events = EPOLLIN;
	_main_event.data.fd = _server_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &_main_event) == -1)
		throw(std::runtime_error("Error in epoll_ctl"));
}
