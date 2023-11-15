/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/15 10:35:21 by gacorrei         ###   ########.fr       */
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

void	Server::connection()
{
	int		count;
	int		client_fd;
	C_IT	match;

	while (_loop_state)
	{
		// for debugging Print the number of clients connected, the number of channels,the clients connected,  the number of clients in each channel, the clients in each channel, the operations in each channel, the invites in each channel, the bans in each channel,
		std::cout << "-----------SERVER INFO-----------\n";
		// number of clients connected to the server
		std::cout << "There are " << _clients.size() << " clients connected\n";
		// clients connected to the server
		if (_clients.size() > 0)
		{
			std::cout << "\nClients connected to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				std::cout << it->get_nickname() << "\n";
			}
			std::cout << "\nClients authenticated to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				if (it->get_authenticated() == true)
					std::cout << it->get_nickname() << "\n";
			}
			std::cout << "\nClients registered to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				if (it->get_registered())
					std::cout << it->get_nickname() << "\n";
			}
		}
		// number of channels
		std::cout << "\nThere are " << _channels.size() << " channels\n";
		// names of the channels
		if (_channels.size() > 0)
		{
			std::cout << "\nChannels:\n";
			for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				std::cout << it->get_name() << "\n";
			}
		}
		// clients in each channel, operations in each channel, invites in each channel, bans in each channel
		if (_channels.size() > 0)
		{
			for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				std::cout << "\nChannel " << it->get_name() << " has " << it->get_clients_in_channel().size() << " clients\n";
				std::cout << "\nClients in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_in_channel().begin(); it2 != it->get_clients_in_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nOperators in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_operator_channel().begin(); it2 != it->get_clients_operator_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nInvites in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_invited_to_channel().begin(); it2 != it->get_clients_invited_to_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nBans in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_banned().begin(); it2 != it->get_clients_banned().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
			}
		}
		std::cout << "---------------------------------\n";
		// end of debugging
		count = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
		if (count == -1)
		{
			if (!_loop_state)
				break;
			throw(std::runtime_error("Error in epoll_wait"));
		}
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
				{
					std::cout << "Error. Could not find client\n";
					disconnect_client(match->get_client_fd());
					continue;
				}
				client_cmds(*match);
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
}

int	Server::client_cmds(Client &client)
{
	char		buffer[BUFFER_READ_SIZE];
	int			fd = client.get_client_fd();
	ssize_t		n = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (n == -1)
	{
		disconnect_client(fd);
		throw(std::runtime_error("Error. Failed in rcv."));
	}
	else if (n == 0) // recv returns 0 if client disconnects
	{
		disconnect_client(fd);
		return 0;
	}
	buffer[n] = 0;
	std::cout << "Received:" << buffer << ".\n";
	if (n > 0 && buffer[n - 1] == '\n')
	{
		buffer[n - 1] = 0;
		client.add_to_cmd(static_cast<std::string>(buffer));
		strncpy(buffer, client.get_cmd().c_str(), BUFFER_READ_SIZE);
		client.clear_cmd();
	}
	else if (n > 0 && buffer[n - 1] != '\n')
	{
		client.add_to_cmd(static_cast<std::string>(buffer));
		return 0;
	}
	if (n > 1 && buffer[n - 2] == '\r')
		buffer[n - 2] = 0;
	if (!client.get_authenticated() && !strncmp(buffer, "CAP LS 302", 10))
	{
		std::cout << "TEST\n";
		return 0;
	}

	std::stringstream	s(buffer);
	std::string			cmd;
	std::string			input;

	s >> cmd;
	std::getline(s, input);
	input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
	if (cmd == "PASS")
		cmd_pass(client, input);
	else if (cmd == "USER")
		cmd_user(client, input);
	else if (cmd == "NICK")
		cmd_nick(client, input);
	else if (cmd == "JOIN")
		cmd_join(client, input);
	else if (cmd == "PRIVMSG")
		cmd_privmsg(client, input);
	else if (cmd == "KICK")
		cmd_kick(client, input);
	else if (cmd == "INVITE")
		cmd_invite(client, input);
	else if (cmd == "TOPIC")
		cmd_topic(client, input);
	else if (cmd == "MODE")
		cmd_mode(client, input);
	// else if (cmd == "PART")
	// 	cmd_part(client, input);
	//USE ITERATORS FOR THIS INSTEAD OF REFERENCES TO MAKE ACCESS EASIER??
	else if (cmd == "EXIT")
		disconnect_client(client.get_client_fd());
	else
	{
		sendErrorMessage(fd, "Error. Command not found\r\n");
		return (-1);
	}
	return(0);
}

/* cmd_mode: set mode of channel
 * 1. Parse input into channel name and mode
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Set mode of channel
 */
int Server::cmd_mode(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string mode;
	std::string argument;

	// Parse input
	iss >> channel_to_find >> mode >> argument;
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = "Error[MODE]: Channel " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find if Client is in vector of clients operator_channel
	if (!channel->find_clients_operator_channel(client))
	{
		std::string error = "Error[MODE]: You are not an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Set mode of channel
	if (mode == "+o")
	{
		// Expecting format: MODE <channel> +o <nickname>
		// look if argument(client to become operator) is in the channel
		if (!channel->find_client_in_channel_by_nickname(argument))
		{
			std::string error = "Error[MODE +o]: " + argument + " is not in the channel " + channel->get_name() + "\r\n"; 
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// look for nickname in clients operator_channel
		if (channel->find_clients_operator_channel(argument))
		{
			std::string error = "Error[MODE +o]: " + argument + " is already an admin in channel " + channel_to_find + "\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// add nickname to clients operator_channel
		// find client by nickname
		Client *client_to_add = find_client(client, argument);
		if (!client_to_add)
		{
			std::string error = "Error[MODE +o]: Client " + argument + " does not exist\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		channel->add_client_to_clients_operator_vector(*client_to_add);
		std::string success = "Success[MODE +o]: " + argument + " is now an admin in channel " + channel_to_find + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		sendSuccessMessage(client_to_add->get_client_fd(), success);
		return (0);
	}
	else if (mode == "-o")
	{
		// Expecting format: MODE <channel> +o <nickname>
		// look if argument(client to become operator) is in the channel
		if (!channel->find_client_in_channel_by_nickname(argument))
		{
			std::string error = "Error[MODE -o]: " + argument + " is not in the channel " + channel->get_name() + "\r\n"; 
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// look for nickname in clients operator_channel
		if (!channel->find_clients_operator_channel(argument))
		{
			std::string error = "Error[MODE -o]: " + argument + " is not an admin in channel " + channel_to_find + "\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// remove nickname from clients operator_channel
		// find client by nickname
		Client *client_to_remove = find_client(client, argument);
		if (!client_to_remove)
		{
			std::string error = "Error[MODE -o]: Client " + argument + " does not exist\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		channel->remove_client_from_clients_operator_vector(*client_to_remove);
		std::string success = "Success[MODE -o]: " + argument + " is no longer an operator in channel " + channel_to_find + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "+k")
	{
		// Expecting format: MODE <channel> +k <password>
		// check if password is valid
		if (password_checker(argument, client.get_client_fd()) == 1)
			return (1);
		// change password of channel
		channel->set_password(argument);
		std::string success = "Success: Password of channel " + channel_to_find + " changed to " + argument + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "-k")
	{
		// Expecting format: MODE <channel> -k
		// check if channel has password
		if (channel->get_password().empty())
		{
			std::string error = "Error[MODE -k]: Channel " + channel_to_find + " does not have a password\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// remove password of channel
		channel->set_password("");
		std::string success = "Success[MODE -k]: Password of channel " + channel_to_find + " removed\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "+i")
	{
		// Expecting format: MODE <channel> +i
		// check if channel is invite only
		if (channel->get_channel_invite_only() == true)
		{
			std::string error = "Error[MODE +i]: Channel " + channel_to_find + " is already invite only\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// set channel to invite only
		channel->set_channel_invite_only(true);
		std::string success = "Success[MODE +i]: Channel " + channel_to_find + " is now invite only\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "-i")
	{
		// Expecting format: MODE <channel> -i
		// check if channel is invite only
		if (channel->get_channel_invite_only() == false)
		{
			std::string error = "Error[MODE -i]: Channel " + channel_to_find + "already not invite only\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// set channel to invite only
		channel->set_channel_invite_only(false);
		std::string success = "Success[MODE -i]: Channel " + channel_to_find + " is now not invite only\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "+t")
	{
		// Expecting format: MODE <channel> +t
		// check if channel topic_mode is true
		if (channel->get_topic_mode() == true)
		{
			std::string error = "Error[MODE +t]: Channel " + channel_to_find + " already has topic mode set\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// set channel topic_mode to true
		channel->set_topic_mode(true);
		std::string success = "Success[MODE +t]: Channel " + channel_to_find + " now has topic mode set\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "-t")
	{
		// Expecting format: MODE <channel> -t
		// check if channel topic_mode is false
		if (channel->get_topic_mode() == false)
		{
			std::string error = "Error[MODE -t]: Channel " + channel_to_find + " already has topic mode not set\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// set channel topic_mode to false
		channel->set_topic_mode(false);
		std::string success = "Success[MODE -t]: Channel " + channel_to_find + " now has topic mode not set\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "+l")
	{
		// Expecting format: MODE <channel> +l <limit>
		// transform argument into a number
		std::istringstream issz(argument);
		long unsigned int limit;
		issz >> limit;
		// change channel limit
		channel->set_channel_limit(limit);
		std::string success = "Success[MODE +l]: Channel " + channel_to_find + " now has limit " + argument + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else if (mode == "-l")
	{
		// Expecting format: MODE <channel> -l
		// check if channel limit is 0
		if (channel->get_channel_limit() == 0)
		{
			std::string error = "Error[MODE -l]: Channel " + channel_to_find + " already has no limit\r\n";
			sendErrorMessage(client.get_client_fd(), error);
			return (1);
		}
		// change channel limit
		channel->set_channel_limit(0);
		std::string success = "Success[MODE -l]: Channel " + channel_to_find + " now has no limit\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		return (0);
	}
	else
	{
		std::string error = "Error[MODE]: Usage: MODE <channel> [+|-][o|k|i|t|l] <argument>\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	return (0);
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
		if (std::isspace(check[i]) || (!std::isalnum(check[i])
			&& (check[i] != '_') && (check[i] != '-')))
			return false;
	return true;
}

int	Server::nick_validation(std::string check)
{
	if (!name_validation(check))
		return 1;

	C_IT	it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (name_compare(check, it->get_nickname()))
			return 2;
	}
	return 0;
}

int	Server::name_compare(std::string check, std::string comp)
{
	std::cout << "Name to check is: " << check << " comparing against: " << comp << "\n";
	std::transform(check.begin(), check.end(), check.begin(), tolower);
	std::transform(comp.begin(), comp.end(), comp.begin(), tolower);
	std::cout << "After tolower: " << check << " and " << comp << "\n";
	if (check == comp)
		return 1;
	return 0;
}

void	Server::disconnect_client(int fd)
{
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	leave_all_rooms(fd);
	close(fd);
	_clients.erase(match);
}

void	Server::leave_all_rooms(int fd)
{
	CH_IT	it = _channels.begin();
	Client	remove = *find(_clients.begin(), _clients.end(), fd).base();

	for (; it < _channels.end(); it++)
	{
		it->remove_client(remove);
		if (it->get_clients_in_channel().size() == 0)
			_channels.erase(it);
	}
}

void	Server::signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << std::endl;
		_loop_state = 0;
	}
}

void	Server::cmd_pass(Client &client, std::string input)
{
	int	fd = client.get_client_fd();

	if (client.get_authenticated())
	{
		std::string error = "Error[PASS]: You are already authenticated\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (pass_validation(input))
	{
		std::string success = "Success[PASS]: You are now authenticated\r\n";
		sendSuccessMessage(fd, success);
		client.set_authenticated(true);
		return;
	}
	if (client.pass_counter(0, 0) == 2)
	{
		std::string error = "Error[PASS]: Too many wrong attempts, disconnecting\r\n";
		sendErrorMessage(fd, error);
		disconnect_client(fd);
	}
	std::string error = "Error[PASS]: Wrong password\r\n";
	sendErrorMessage(fd, error);
	client.pass_counter(1, 0);
}

void	Server::cmd_user(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	std::stringstream	ss(input);
	std::string			username;

	ss >> username;
	if (client.get_registered())
	{
		std::string error = "Error[USER]: You are already registered\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_username().empty())
	{
		std::string error = "Error[USER]: You already have a username\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (name_validation(username))
	{
		client.set_username(username);
		std::string success = "Success[USER]: Username set to " + username + "\r\n";
		sendSuccessMessage(fd, success);
		if (!client.get_nickname().empty())
		{
			success = "Success[USER]: You are now registered\r\n";
			sendSuccessMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else
	{
		std::string error = "Error[USER]: Username can't have spaces or symbols (except '_' and '-') \
		and must be between 3 and 10 characters long\r\n";
		sendErrorMessage(fd, error);
	}
}

void	Server::cmd_nick(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	int	test;

	if (client.get_registered())
	{
		std::string error = "Error[NICK]: You are already registered\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_nickname().empty())
	{
		std::string error = "Error[NICK]: You already have a nickname\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	test = nick_validation(input);
	if (!test)
	{
		client.set_nickname(input);
		std::string success = "Success[NICK]: Nickname set to " + input + "\r\n";
		sendSuccessMessage(fd, success);
		if (!client.get_username().empty())
		{
			success = "Success[NICK]: You are now registered\r\n";
			sendSuccessMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else if (test == 1)
	{
		std::string error = "Error[NICK]: Nickname can't have spaces or symbols (except '_' and '-') and must be between 3 and 10 characters long\r\n";
		sendErrorMessage(fd, error);
	}
	else
	{
		std::string error = "Error[NICK]: Nickname already in use, choose another\r\n";
		sendErrorMessage(fd, error);
	}
}

/* channel_name_validation: check if channel name is valid
** 1. check if channel name starts with '#'
** 2. check if channel name has whitespaces
** 3. check if channel name is between 2 and 200 characters long
*/
int Server::channel_name_validation(int client_fd, std::string check)
{
	int	len = check.size();
	std::string message;

	if (check[0] != '#')
	{
		message = "Error[JOIN]: Channel name must start with '#'\r\n";
		sendErrorMessage(client_fd, message);
		return (1);
	}
	for (int i = 1; i < len; i++)
	{
		if (!std::isalnum(check[i]) && (check[i] != '_') && (check[i] != '-'))
		{
			message = "Error[JOIN]: Channel name can only contain alphanumeric characters, '_' and '-'\r\n";
			sendErrorMessage(client_fd, message);
			return (1);
		}
	}
	if (len < 2 || len > 200)
	{
		message = "Error[JOIN]: Channel name must be between 2 and 200 characters long\r\n";
		sendErrorMessage(client_fd, message);
		return (1);
	}
	return (0);
}

/* cmd_join: add client to channel
** 1. check if channel exists
** if not, create channel and add client as admin
** 2. if channel exists, check if client is already in channel
** if not, add client to channel if he is not banned 
** and if channel is not full and channel is not invite only
** 3. if client is already in channel, send error message
*/
int Server::cmd_join(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	std::string message;
	std::string input_channel_name;
	std::string input_password;
	std::vector<Client>	in_channel;

	// next word is input_channel_name use sstreams
	std::stringstream ss(input);
	ss >> input_channel_name;
	ss >> input_password;
	CH_IT	it = find(_channels.begin(), _channels.end(), input_channel_name);
	// check if client is registered
	if (client.get_registered() == false)
	{
		message = "Error[JOIN]: You must be registered to join a channel\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// if channel does not exist
	if (it == _channels.end())
	{
		if (channel_name_validation(fd, input_channel_name) == 1)
		{
			return (1);
		}
		// create channel		
		Channel new_channel(input_channel_name);
		_channels.push_back(new_channel);
		it = find(_channels.begin(), _channels.end(), input_channel_name);
		it->add_client(client);
		//set client as operator
		it->add_client_to_clients_operator_vector(client);
		message = "Success[JOIN]:" + client.get_nickname() + " has created the channel " + input_channel_name + "\r\n";
		sendSuccessMessage(fd, message);
		//send message to all clients in channel?
		return (0);
	}
	in_channel = it->get_clients_in_channel();
	// check if client is already in channel
	if (find(in_channel.begin(), in_channel.end(), client.get_client_fd()) != in_channel.end())
	{
		message = "Error[JOIN]: " +	client.get_nickname() + " is already in channel " + input_channel_name + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// check if channel is full, or invite only or it needs a password to do
	// checks if it needs a password and if the password is correct
	if (it->get_clients_in_channel().size() >= it->get_channel_limit() && it->get_channel_limit() != 0)
	{
		message = "Error[JOIN]: Channel " + input_channel_name + " is full\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	if (it->get_channel_invite_only() == true)
	{
		// check if not client is on invite list
		if (find(it->get_clients_invited_to_channel().begin(), it->get_clients_invited_to_channel().end(), client.get_client_fd()) == it->get_clients_invited_to_channel().end())
		{
			message = "Error[JOIN]: Channel " + input_channel_name + " is invite only\r\n";
			sendErrorMessage(fd, message);
			return (1);
		}
	}
	if (!it->get_password().empty())
	{
		if (it->get_password() != input_password)
		{
			message = "Error[JOIN]: Wrong password for channel " + input_channel_name + "\r\n";
			sendErrorMessage(fd, message);
			return (1);
		}
	}
	// check if client is banned use find_banned_client
	if (it->find_banned_client(client))
	{
		message = "Error[JOIN]: You (" + client.get_nickname() + ") are banned from channel " + input_channel_name + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}

	// add client to channel
	it->add_client(client);
	message = "Success[JOIN]: " + client.get_nickname() + " has joined channel " + input_channel_name + "\r\n";
	sendSuccessMessage(fd, message);
	// send message to all clients in channel loop through clients in channel and send message

	return (0);
}


void	Server::cmd_privmsg(Client &client, std::string input)
{
	int					fd = client.get_client_fd();
	std::stringstream	s(input);
	std::string			dest, msg;
	Channel				*ch_test = NULL;
	Client				*c_test = NULL;

	if (!client.get_registered())
	{
		std::string error = "Error[PRIVMSG]: You must be registered to send a message\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	s >> dest;
	getline(s, msg);
	msg.erase(0, msg.find_first_not_of(" \t\n\r\f\v"));
	if (msg[0] != ':')
	{
		std::string error = "Error[PRIVMSG]: Usage: /PRIVMSG <destination> :<message>\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	msg = msg.substr(1, msg.length() - 1);
	if (dest[0] == '#')
	{
		ch_test = findChannel(client, dest);
		if (!ch_test)
		{
			send(fd, "Error. Could not find destination\r\n", 35, MSG_NOSIGNAL);
			return;
		}
		ch_test->message(client, msg);
		return;
	}
	c_test = find_client(client, dest);
	if (!c_test)
	{
		send(fd, "Error. Could not find destination\r\n", 35, MSG_NOSIGNAL);
		return;
	}
	fd = c_test->get_client_fd();
	std::string	final_msg = ":" + client.get_nickname() + "!"
	+ client.get_username() + "@" + "localhost" + " PRIVMSG "
	+ c_test->get_nickname() + " :" + msg + "\r\n";
	sendSuccessMessage(fd, final_msg);
}


/* cmd_kick: kick a user from a channel (KICK <channel> <nickname> [<reason>])
 * 1. Parse input into channel name and nickname and reason
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Find the nickname in the channel
 * 5. Kick the user
 */
int Server::cmd_kick(Client &client, std::string input)
{
    std::istringstream iss(input);
    std::string channel_to_find;
    std::string nickname;
    std::string reason;
	
	// Parse input
	// skip till find '#'
	while (iss.peek() != '#')
		iss.ignore();
    iss >> channel_to_find >> nickname;
    std::getline(iss, reason);

	// if nickname is empty
	if (nickname.empty())
	{
		std::string error = "ERROR[KICK]: Usage: KICK <channel> <nickname> [<reason>]\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// if nickname is equal to client nickname
	if (nickname == client.get_nickname())
	{
		std::string error = "ERROR[KICK]: You can't kick yourself\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
    // Find the channel
    Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = "Error[KICK]: Channel " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find if Client is in vector of clients operator_channel
	if (!channel->find_clients_operator_channel(client))
	{
		std::string error = "Error[KICK]: " + client.get_nickname() + " is not an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// find if nickname is in operator_channel
	if (channel->find_clients_operator_channel(nickname))
	{
		std::string error = "Error[KICK]: " + nickname + " is an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
    // Find the client to kick in the channel
	Client *client_to_kick = channel->find_client_in_channel_by_nickname(nickname);
	if(!client_to_kick)
	{
		std::string error = "Error[KICK]: " + nickname + " is not in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
    // Kick the user
    if (kickClientFromChannel(channel, client_to_kick, reason) == -1)
        return (1);
    return (0);
}

/* is_client_admin: check if client is administrator
 * 1. Check if client is administrator
 * 2. Send error message if client is not administrator
 */
int Server::is_client_admin(Client &client)
{
	if (!client.get_is_admin())
	{
		std::string error = "Error: " + client.get_nickname() 
			+ " is not administrator\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	return 0;
}

/* sendSuccessMessage: send success message to client
 * 1. Send success message to client
 */
int Server::sendSuccessMessage(int client_fd, const std::string	&successMessage)
{
	if (send(client_fd, successMessage.c_str(), successMessage.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}

/* sendErrorMessage: send error message to client
 * 1. Send error message to client
 */
int Server::sendErrorMessage(int client_fd, const std::string	&errorMessage)
{
    if (send(client_fd, errorMessage.c_str(), errorMessage.size(), MSG_NOSIGNAL) == -1)
    {
        std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
        return (-1);
    }
    return (0);
}

/* findChannel: find channel
 * 1. Find channel by iterating through _channels
 * 2. If channel does not exist, send error message to client and return NULL
 * else return channel
 */
Channel	*Server::findChannel(Client &client, const std::string	&channelName)
{
	std::vector<Channel>::iterator it = find(_channels.begin(), _channels.end(), channelName);

	(void)client;
	if (it == _channels.end())
	{
		return (NULL);
	}
	return &(*it);
}


/* findClientInChannel: find client in channel
 * 1. Find client by iterating through channel->get_clients_in_channel()
 * 2. If client does not exist, send error message to client and return NULL
 * else return client
 */
/* Client	*Server::findClientInChannel(Client &client, Channel *channel, const std::string	&nickname)
{
	Client	*match = channel->find_client(nickname);

	if (!match)
	{
		sendErrorMessage(client.get_client_fd(), "Error: " + nickname 
			+ " does not exist\r\n");
    	return (NULL);
	}
	return match;
} */


Client	*Server::find_client(Client &client, const std::string& nickname)
{
	(void)client;

	std::vector<Client>::iterator it = find(_clients.begin(), _clients.end(), nickname);

	if (it == _clients.end())
		return (NULL);
	return &(*it);
}

/* kickClientFromChannel: kick client from channel
 * 1. Remove client from channel->get_clients_in_channel()
 * 2. Send message to client
 */
int Server::kickClientFromChannel(Channel *channel, Client *client, const std::string &reason)
{
	std::string message;
	
	// Remove client from channel
	channel->remove_client(*(client));
	// add client to banned list
	channel->add_client_to_banned_vector(*(client));
	// if client is in operator_channel, remove from operator_channel
	if (channel->find_clients_operator_channel(*client))
		channel->remove_client_from_clients_operator_vector(*client);
	// Send message to client
    if (reason.empty())
    {
        message = client->get_nickname() 
			+ " has been kicked from the channel. Reason: This is Sparta!\r\n";
    }
	else
	{
		message = client->get_nickname() 
			+ " has been kicked from the channel. Reason: " + reason + "\r\n";
	}
	// Send message to client send success message to client
	if (sendSuccessMessage(client->get_client_fd(), message) == -1)
		return (1);
    return (0);
}

/* cmd_topic: set topic of channel
 * 1. Parse input into channel name and topic
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Set topic of channel
 */
int Server::cmd_topic(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string topic;
	
	// Parse input
	iss >> channel_to_find;
	std::getline(iss, topic);

	// if no channel name is given
	if (channel_to_find.empty() || strIsWhitespace(channel_to_find))
	{
		std::string error = "Error[TOPIC]: Usage: topic <channel> [topic]\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = "Error[TOPIC]: " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	// Set topic of channel
	if (topic.empty() || strIsWhitespace(topic))
	{
		std::string message;
		// send the topic to client
		if (channel->get_topic().empty())
			message = "Topic of " + channel->get_name() + " is not set\r\n";
		else
			message = "Topic of " + channel->get_name() + " is: " + channel->get_topic() + "\r\n";
		sendSuccessMessage(client.get_client_fd(), message);
		return (0);
	}
	// check if topic_mode is true
	if (channel->get_topic_mode() == true && channel->find_clients_operator_channel(client))
	{
		std::string success = "Success[TOPIC]: topic changed to " + topic + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		channel->set_topic(topic);
	}
	else if (!channel->find_clients_operator_channel(client))
	{
		// Check if client is administrator
		std::string error = "Error[TOPIC]: " + client.get_nickname() + " is not an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}	
	return (0);
}

/* cmd_invite: invite client to channel (INVITE <nickname> <channel>)
 * 1. Parse input into channel name and nickname
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Find the nickname in the channel
 * 5. Invite the user
 */
int Server::cmd_invite(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string nickname;
	
	// Parse input
	iss >> nickname;
	iss >> channel_to_find;

	std::cout << "nickname: " << nickname << std::endl; // debug
	std::cout << "channel_to_find: " << channel_to_find << std::endl; // debug
	// if nickname is empty or channel is empty
	if (nickname.empty() || channel_to_find.empty())
	{
		std::string message = "Error[INVITE]: INVITE <nickname> <channel>\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string message = "Error[INVITE]: channel " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find if Client is in _clients_operator_channel
	if (!channel->find_clients_operator_channel(client))
	{
		std::string message = "Error[INVITE]: You (" + client.get_nickname() + ") are not an operator in channel " + channel->get_name() + "\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// find if nickname is belongs to client that is already on _clients_invited_to_channel
	if(channel->find_clients_invited_to_channel_by_nickname(nickname))
	{
		std::string message = "Error[INVITE]: " + nickname + " is already invited to the channel" + channel->get_name() + "\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find the client to invite
	Client *client_to_invite = find_client(client, nickname);
	if (!client_to_invite)
	{
		std::string message = "Error[INVITE]: user with nickname " + nickname + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Invite the user
	channel->add_client_to_clients_invited_vector(*client_to_invite);
	// Send message to client
	std::string message = "[INVITE] " + client.get_nickname() + " has invited you to join " + channel->get_name() + "\r\n";
	sendSuccessMessage(client_to_invite->get_client_fd(), message);
	return (0);
}

int	Server::password_checker(std::string password)
{
	// Check if password is between 3 and 12 characters
	if (password.length() < 3 || password.length() > 12)
	{
		std::cerr << RED << "ERROR: " << RESET 
			<< "Password must be between 3 and 12 characters" << std::endl;
		return (1);
	}
	// Check if password contains non-printable characters
	for (size_t i = 0; i < password.length(); i++)
	{
		if (!isprint(password[i]))
		{
			std::cerr << RED << "ERROR: " << RESET 
				<< "Password must not contain non-printable characters" << std::endl;
			return (1);
		}
	}
	return (0);
}
/* password_checker: check if password is valid
 * 1. Check if password is between 3 and 12 characters
 * 2. Check if password contains non-printable characters
 */
int	Server::password_checker(std::string password, int fd)
{
	// Check if password is between 3 and 12 characters
	if (password.length() < 3 || password.length() > 12)
	{
		std::string error = "Error. Password must be between 3 and 12 characters\r\n";
		sendErrorMessage(fd, error);
		return (1);
	}
	// Check if password contains non-printable characters
	for (size_t i = 0; i < password.length(); i++)
	{
		if (!isprint(password[i]))
		{
			std::string error = "Error. Password must not contain non-printable characters\r\n";
			sendErrorMessage(fd, error);
			return (1);
		}
	}
	return (0);
}
