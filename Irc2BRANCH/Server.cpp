/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/10 10:13:07 by gacorrei         ###   ########.fr       */
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

	while (_loop_state)
	{
		//ONLY FOR TESTING!!! DELETE AFTER
		std::cout << "There are " << _clients.size() << " clients connected\n";
		// number of channels
		std::cout << "There are " << _channels.size() << " channels\n";
		// number of clients in each channel
		if (_channels.size() > 0)
		{
			for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				std::cout << "Channel " << it->get_name() << " has " << it->get_clients_in_channel().size() << " clients\n";
			}
		}
		//
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
	buffer[n] = 0;
	if (n > 0 && buffer[n - 1] == '\n')
		buffer[n - 1] = 0;
	if (n > 0 && buffer[n - 2] == '\r')
		buffer[n - 2] = 0;

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
	// else if (cmd == "MODE")
	// 	cmd_mode(client, input);
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
		send(fd, "You are already authenticated\r\n", 32, MSG_NOSIGNAL);
		return;
	}
	if (pass_validation(input))
	{
		send(fd, "Success!\r\n", 11, MSG_NOSIGNAL);
		client.set_authenticated(true);
		return;
	}
	if (client.pass_counter(0, 0) == 2)
	{
		send(fd, "Too many wrong attempts, disconnecting\r\n", 41, MSG_NOSIGNAL);
		disconnect_client(fd);
	}
	send(fd, "Wrong password\r\n", 17, MSG_NOSIGNAL);
	client.pass_counter(1, 0);
}

void	Server::cmd_user(Client &client, std::string input)
{
	int	fd = client.get_client_fd();

	if (client.get_registered())
	{
		send(fd, "You are already registered\r\n", 29, MSG_NOSIGNAL);
		return;
	}
	if (!client.get_username().empty())
	{
		send(fd, "Username already set\r\n", 23, MSG_NOSIGNAL);
		return;
	}
	if (name_validation(input))
	{
		client.set_username(input);
		send(fd, "Username set\r\n", 15, MSG_NOSIGNAL);
		if (!client.get_nickname().empty())
		{
			send(fd, "Successfully registered\r\n", 26, MSG_NOSIGNAL);
			client.set_registered(true);
		}
		return;
	}
	else
		send(fd, "Name can't have spaces or symbols (except '_' and '-') \
		and must be between 3 and 10 characters long\r\n", 104, MSG_NOSIGNAL);
}

void	Server::cmd_nick(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	int	test;

	if (client.get_registered())
	{
		send(fd, "You are already registered\r\n", 29, MSG_NOSIGNAL);
		return;
	}
	if (!client.get_nickname().empty())
	{
		send(fd, "Nickname already set\r\n", 23, MSG_NOSIGNAL);
		return;
	}
	test = nick_validation(input);
	if (!test)
	{
		client.set_nickname(input);
		send(fd, "Nickname set\r\n", 15, MSG_NOSIGNAL);
		if (!client.get_username().empty())
		{
			send(fd, "Successfully registered\r\n", 26, MSG_NOSIGNAL);
			client.set_registered(true);
		}
		return;
	}
	else if (test == 1)
		send(fd, "Name can't have spaces or symbols (except '_' and '-') \
		and must be between 3 and 10 characters long\r\n", 104, MSG_NOSIGNAL);
	else
		send(fd, "Nickname already in use, choose another\r\n", 42, MSG_NOSIGNAL);
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

	std::cout << "check: " << check << std::endl;
	if (check[0] != '#')
	{
		message = "Channel name must start with '#'\r\n";
		sendErrorMessage(client_fd, message);
		return (1);
	}
	for (int i = 1; i < len; i++)
	{
		if (!std::isalnum(check[i]) && (check[i] != '_') && (check[i] != '-'))
		{
			message = "Channel name can only have alphanumeric characters, \
			'_' and '-'\r\n";
			sendErrorMessage(client_fd, message);
			return (1);
		}
	}
	if (len < 2 || len > 200)
	{
		message = "Channel name must be between 2 and 200 characters long\r\n";
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
		message = "You must be registered to join a channel\r\n";
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
		if (input_password.empty() || strIsWhitespace(input_password) == true)
		{
			Channel new_channel(input_channel_name);
			_channels.push_back(new_channel);
		}
		else
		{
			Channel new_channel(input_channel_name, input_password);
			_channels.push_back(new_channel);
		}
		// add client to channel
		it = find(_channels.begin(), _channels.end(), input_channel_name);
		
		it->add_client(client);
		//set client as operator
		it->add_client_to_clients_operator_vector(client);
		message = client.get_nickname() + " has created the channel " + input_channel_name + "\r\n";
		sendSuccessMessage(fd, message);
		//send message to all clients in channel?
		return (0);
	}
	in_channel = it->get_clients_in_channel();
	// check if client is already in channel
	if (find(in_channel.begin(), in_channel.end(), client.get_client_fd()) != in_channel.end())
	{
		message = client.get_nickname() + " is already in channel " + input_channel_name + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// check if channel is full, or invite only or it needs a password to do
	// checks if it needs a password and if the password is correct
	if (it->get_clients_in_channel().size() >= it->get_channel_limit())
	{
		message = "Channel " + input_channel_name + " is full\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	if (it->get_channel_invite_only() == true)
	{
		// check if not client is on invite list
		if (find(it->get_clients_invited_to_channel().begin(), it->get_clients_invited_to_channel().end(), client.get_client_fd()) == it->get_clients_invited_to_channel().end())
		{
			message = "Channel " + input_channel_name + " is invite only\r\n";
			sendErrorMessage(fd, message);
			return (1);
		}
	}
	if (!it->get_password().empty())
	{
		if (it->get_password() != input_password)
		{
			message = "Wrong password for channel " + input_channel_name + "\r\n";
			sendErrorMessage(fd, message);
			return (1);
		}
	}
	// add client to channel
	it->add_client(client);
	message = client.get_nickname() + " has joined channel " + input + "\r\n";
	if (send(fd, message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << "Error sending message to client in cmd_join 3" << fd 
			<< std::endl;
		return (-1);
	}
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
		send(fd, "Error. Can't use commands before registering\r\n", 47, MSG_NOSIGNAL);
		return;
	}
	s >> dest;
	getline(s, msg);
	if (msg[0] != ':')
	{
		send(fd, "Error. Wrong message format (must start with ':')\r\n", 52, MSG_NOSIGNAL);
		return;
	}
	if (dest[0] == '#')
	{
		ch_test = findChannel(client, dest);
		if (!ch_test)
		{
			send(fd, "Error. Could not find destination\r\n", 36, MSG_NOSIGNAL);
			return;
		}
		ch_test->message(client, msg.substr(1, msg.length() - 1));
	}
	c_test = find_client(client, dest);
	if (!c_test)
	{
		send(fd, "Error. Could not find destination\r\n", 36, MSG_NOSIGNAL);
		return;
	}
	fd = c_test->get_client_fd();
	std::string	final_msg = ":" + client.get_nickname() + "!"
	+ client.get_username() + "@" + "localhost" + " PRIVMSG "
	+ c_test->get_nickname() + " :" + msg + "\r\n";
	send(fd, final_msg.c_str(), final_msg.length(), MSG_NOSIGNAL);
}

/* cmd_kick: kick a user from a channel
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
    iss >> channel_to_find >> nickname;
    std::getline(iss, reason);
    // Check if client is administrator
	if (is_client_admin(client) == 0) // change to check if it is channel operator
		return (1);
    // Find the channel
    Channel *channel = findChannel(client, channel_to_find);
    if (!channel)
        return (1);
    // Find the nickname in the channel
    Client *client_to_kick = findClientInChannel(client, channel, nickname);
    if (!client_to_kick)
		return (1);
    // Kick the user
    // int client_to_kick_fd = client_to_kick->get_client_fd();
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
		std::cerr << GREEN << "Error: " << RESET << "send() failed" << std::endl;
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

	if (it == _channels.end())
	{
		sendErrorMessage(client.get_client_fd(), "Error: " + channelName
			+ " does not exist\r\n");
		return NULL;
	}
	return &(*it);
}


/* findClientInChannel: find client in channel
 * 1. Find client by iterating through channel->get_clients_in_channel()
 * 2. If client does not exist, send error message to client and return NULL
 * else return client
 */
Client	*Server::findClientInChannel(Client &client, Channel *channel, const std::string	&nickname)
{
	Client	*match = channel->find_client(nickname);

	if (!match)
	{
		sendErrorMessage(client.get_client_fd(), "Error: " + nickname 
			+ " does not exist\r\n");
    	return (NULL);
	}
	return match;
}


Client	*Server::find_client(Client &client, const std::string& nickname)
{
	C_IT	match = find(_clients.begin(), _clients.end(), nickname);

	if (match != _clients.end())
	{
		sendErrorMessage(client.get_client_fd(), "Error: " + nickname 
			+ " does not exist\r\n");
    	return (NULL);
	}
	return match.base();
}

/* kickClientFromChannel: kick client from channel
 * 1. Remove client from channel->get_clients_in_channel()
 * 2. Send message to client
 */
int Server::kickClientFromChannel(Channel *channel, Client *client, const std::string &reason)
{
	std::string message;
	
    channel->get_clients_in_channel().erase(std::remove(channel->get_clients_in_channel().begin(), channel->get_clients_in_channel().end(), *client), channel->get_clients_in_channel().end());
    if (reason.empty())
    {
        message = client->get_nickname() 
			+ " has been kicked from the channel. Reason: This is Sparta!";
    }
	else
	{
		message = client->get_nickname() 
			+ " has been kicked from the channel. Reason: " + reason;
	}
    if (send(client->get_client_fd(), message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
    {
        std::cerr << "Error: send() failed" << std::endl;
        return (-1);
    }
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
	// Check if client is administrator
	if (is_client_admin(client) == 0)
		return (-1);
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
		return (-1);
	// Set topic of channel
	if (topic.empty() || strIsWhitespace(topic))
	{
		// send the topic to client
		std::string message = "Topic: " + channel->get_topic() + "\r\n";
		if (send(client.get_client_fd(), message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
		{
			std::cerr << "Error: send() failed" << std::endl;
			return (-1);
		}
		return (0);
	}
	channel->set_topic(topic);
	return (0);
}

/* cmd_invite: invite client to channel
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
	// Check if client is administrator
	if (is_client_admin(client) == 0)
		return (-1);
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
		return (-1);
	// Find the nickname in the channel
	Client *client_to_invite = find_client(client, nickname);
	if (!client_to_invite)
		return (-1);
	// Invite the user
	int client_to_invite_fd = client_to_invite->get_client_fd();
	std::string message = client.get_nickname() + " has invited you to join " + channel->get_name() + "\r\n";
	if (send(client_to_invite_fd, message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << "Error: send() failed" << std::endl;
		return (-1);
	}
	return (0);
}
