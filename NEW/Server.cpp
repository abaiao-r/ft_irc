/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/06 13:14:32 by gacorrei         ###   ########.fr       */
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
	std::cout << CYAN << "Default constructor Server called" << RESET 
		<< std::endl;
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
	signal_reset();
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

/* get_port: Returns the port number */
int Server::get_port(void) const
{
	return (_port);
}

/* get_server_fd: Returns the server file descriptor */
int Server::get_server_fd() const
{
	return (_server_fd);
}

/* get_address: Returns the server address */
sockaddr_in Server::get_address(void) const
{
	return (_address);
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
		return -1;
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
	int flags;

	if ((flags = fcntl(fd, F_GETFL)) == -1)
	{
		std::cout << "Error when getting fcntl flags\n";
		std::cout << strerror(errno) << std::endl;
		return -1;
	}
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
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
	_main_event.events = EPOLLIN;
	_main_event.data.fd = _server_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &_main_event) == -1)
		throw(std::runtime_error("Error in epoll_ctl"));
}

void	Server::connection()
{
	int		count;
	int		client_fd;
	C_IT	match;

	std::cout << "WAITING FOR CONNECTIONS ON PORT: " << _port << std::endl;
	while (_loop_state)
	{
		//ONLY FOR TESTING!!! DELETE AFTER
		std::cout << "There are " << _clients.size() << " clients connected\n";
		for (C_IT it = _clients.begin(); it != _clients.end(); it++)
			std::cout << "Client fd = " << it->get_client_fd() << "\n";
		//
		count = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
		if (count == -1)
			throw(std::runtime_error("Error in epoll_wait"));
		for (int i = 0; i < count; i++)
		{
			if (_events[i].data.fd == _server_fd)
			{
				client_connection();
				client_fd = _clients.back().get_client_fd();
				_main_event.events = EPOLLIN;
				_main_event.data.fd = client_fd;
				if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_main_event) == -1)
					throw(std::runtime_error("Error in epoll_ctl"));
			}
			else if ((_events[i].events & EPOLLERR)
				|| (_events[i].events & EPOLLHUP)
				|| (!(_events[i].events & EPOLLIN)))
				disconnect_client(_events[i].data.fd);
			else
			{
				client_fd = _events[i].data.fd;
				match = std::find(_clients.begin(), _clients.end(), client_fd);
				if (match == _clients.end())
					throw(std::runtime_error("Error. Could not find client"));
				client_actions(*match);
			}
		}
	}
}

void	Server::client_connection()
{
	Client		client;
	socklen_t	len = SOCKLEN;
	int			client_fd = accept(_server_fd, (struct sockaddr*)&client._client_addr, &len);

	if (client_fd == -1)
		throw(std::runtime_error("Error. Could not accept client connection"));
	if (unblock_socket(client_fd))
	{
		close(client_fd);
		throw(std::runtime_error("Error when setting flags"));
	}
	client.set_client_fd(client_fd);
	_clients.push_back(client);
	send(client_fd, "Input password\r\n", 17, MSG_NOSIGNAL);
}

void	Server::client_actions(Client &client)
{
	if (!client.get_authenticated())
		authenticate(client);
	else
		client_cmds(client);
}

void	Server::client_cmds(Client &client)
{
	char		buffer[BUFFER_READ_SIZE];
	std::string	input("");
	int			fd = client.get_client_fd();
	ssize_t		n;

	while (1) //THIS MIGHT BLOCK!! TEST!!
	{
		n = recv(fd, buffer, sizeof(buffer) - 1, 0);
		if (n == -1)
			throw(std::runtime_error("Error. Failed in rcv."));
		if (n == 0)
			break;
		buffer[n] = 0;
		if (n > 0 && buffer[n - 1] == '\n')
			buffer[n - 1] = 0;
		input += static_cast<std::string>(buffer);
	}

	std::stringstream	s(input);
	std::string			s_buffer;
	int					test;

	s >> s_buffer;
	test = get_cmd(s_buffer);
	switch (test)
	{
		case 0:
			cmd_join(client, input);
			break;
		case 1:
			cmd_msg(client, input);
			break;
		case 2:
			cmd_privmsg(client, input);
			break;
		case 3:
			cmd_create(client, input);
			break;
		case 4:
			cmd_kick(client, input);
			break;
		case 5:
			cmd_invite(client, input);
			break;
		case 6:
			cmd_topic(client, input);
			break;
		case 7:
			cmd_mode(client, input);
			break;
		default:
			send(fd, "Error. Unknown command\r\n", 25, MSG_NOSIGNAL);
	}
}

int	Server::get_cmd(std::string cmd)
{
	for (int i = 0; i < CMDS; i++)
		if (cmd == _cmds[i])
			return i;
	return -1;
}

void	Server::authenticate(Client &client)
{
	char		buffer[BUFFER_READ_SIZE];
	int			fd = client.get_client_fd();
	int			test;
	ssize_t		n = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (n == -1)
		throw(std::runtime_error("Error. Failed in rcv."));
	buffer[n] = 0;
	if (n > 0 && buffer[n - 1] == '\n')
		buffer[n - 1] = 0;
	switch (client.pass_counter(1, 0))
	{
		case 0:
		case 1:
		case 2:
			if (pass_validation(static_cast<std::string>(buffer)))
			{
				send(fd, "Choose Nickname\r\n", 18, MSG_NOSIGNAL);
				while (client.pass_counter(0, 0) != 3)
					client.pass_counter(1, 0);
				return;
			}
			send(fd, "Wrong password\r\n", 17, MSG_NOSIGNAL);
			if (client.pass_counter(0, 0) == 3)
			{
				send(fd, "Too many wrong attempts, disconnecting\r\n", 41, MSG_NOSIGNAL);
				disconnect_client(fd);
			}
			break;
		case 3:
			test = nick_validation(static_cast<std::string>(buffer));
			if (!test)
			{
				client.set_nickname(static_cast<std::string>(buffer));
				send(fd, "Choose Username\r\n", 18, MSG_NOSIGNAL);
				return;
			}
			else if (test == 1)
				send(fd, "Name can't have spaces and must be between 3 and 10 characters long\r\n", 70, MSG_NOSIGNAL);
			else
				send(fd, "Nickname already in use, choose another\r\n", 42, MSG_NOSIGNAL);
			client.pass_counter(2, 3);
			break;
		case 4:
			if (name_validation(static_cast<std::string>(buffer)))
			{
				client.set_username(static_cast<std::string>(buffer));
				client.set_authenticated(true);
				return;
			}
			send(fd, "Name can't have spaces and must be between 3 and 10 characters long\r\n", 70, MSG_NOSIGNAL);
			client.pass_counter(2, 4);
	}
}

bool	Server::pass_validation(std::string check) const
{
	return (check == _password);
}

bool	Server::name_validation(std::string check)
{
	int	len = check.size();

	if (len > MAX_LEN || len < MIN_LEN)
		return false;
	for (int i = 0; i < len; i++)
		if (std::isspace(check[i]))
			return false;
	return true;
}

int	Server::nick_validation(std::string check)
{
	if (!name_validation(check))	
		return 1;

	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, check);

	if (match != end)
		return 2;
	return 0;
}

void	Server::disconnect_client(int fd)
{
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	close(fd);
	_clients.erase(match);
}

void	Server::signal_handler(int sig)
{
	if (sig == SIGINT)
		_loop_state = 0;
}

void	Server::signal_global()
{
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}

void	Server::signal_reset()
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
