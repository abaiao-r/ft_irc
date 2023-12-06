/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/11 19:30:40 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Commands.hpp"

std::map<int, std::string> clientBuffers;  // FD -> Accumulated commands

/* Default constructor */
Server::Server() : _port(0), _server_fd(-1)
{
	//std::cout << CYAN << "Default constructor Server called" << RESET << std::endl;
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET; // IPv4
	_address.sin_addr.s_addr = INADDR_ANY; // Any available network interface
	_address.sin_port = htons(_port); // Port
}

/* Parameter constructor */
Server::Server(int port) : _port(port), _server_fd(-1)
{
	//std::cout << CYAN << "Default constructor Server called" << RESET 	<< std::endl;
	/* `std::memset(&_address, 0, sizeof(_address));` is initializing the memory block of `_address` with
	zeros. It sets all the bytes in `_address` to zero, effectively initializing all the fields of the
	`sockaddr_in` structure to zero. This is commonly done before setting specific values to ensure
	that there are no garbage values in the structure. */
	std::memset(&_address, 0, sizeof(_address));
	//sets the address family of the socket to IPv4.
	_address.sin_family = AF_INET;
	//the socket accepts connections from any available network interface on the machine.
	_address.sin_addr.s_addr = INADDR_ANY;
	//htons()converts the port number from host byte order to network byte order, which is the byte order used by the network protocol. This ensures that the port number is stored in the correct byte order for communication over the network.
	_address.sin_port = htons(_port);
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
	close(_server_fd);
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
	return (*this);
}

/* get_port: Returns the port number */
int Server::get_port(void) const
{
	return (_port);
}

/* get_server_fd: Returns the server file descriptor */
int Server::get_server_fd(void) const
{
	return (_server_fd);
}

/* get_address: Returns the server address */
sockaddr_in Server::get_address(void) const
{
	return (_address);
}

int Server::start_listening(void)
{
	if (listen(_server_fd, 5) == -1)
	{
		std::cerr << RED <<"Error:" << RESET << " Cannot listen socket"
			<< std::endl;
		close(_server_fd);
		return (-1);
	}
	std::cout << GREEN << "Server is now listening on port: " << BOLDGREEN << _port << RESET
		<< std::endl;
	return (0);
}

int Server::bind_socket(void)
{

	//pheraps should start the sockaddr struct here. 
	if (bind(_server_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
	{
		std::cerr << RED <<"Error:" << RESET << " Cannot bind socket" 
			<< std::endl;
		close(_server_fd);
		return (-1);
	}
	return (0);
}

int Server::set_socket_options(void) // use int
{
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, 
		sizeof(opt)) == -1) 
	{
		std::cerr << RED <<"Error:" << RESET << " Cannot set socket options" 
			<< std::endl;
		close (_server_fd);
		return (-1);
	}
	return (0);
}

int Server::create_socket(void) // use int
{
	_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_server_fd == -1)
	{
		std::cerr << RED <<"Error:" << RESET << " Cannot create socket" 
			<< std::endl;
		return (-1);
	}
	return (0);
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

int Server::init_server(void) // use int
{
	if (create_socket() == -1)
		return (-1);
	if (unblock_socket(_server_fd) == -1)
		return (-1);
	if (set_socket_options() == -1)
		return (-1);
	if (bind_socket() == -1) 
		return (-1);
	if (start_listening() == -1)
		return (-1);
	create_epoll();
	return (0);
}

int Server::make_socket_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << RED << "Error getting flags for fd " << fd << RESET << std::endl;
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        std::cerr << RED << "Error setting fd " << fd << " to non-blocking" << RESET << std::endl;
        return -1;
    }

    return 0; // Successfully set to non-blocking
}

// This function adds a file descriptor to the epoll instance.
int Server::add_fd_to_epoll(int epoll_fd, int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET; // Read operation | Edge Triggered behavior

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        std::cerr << RED << "Error adding fd " << fd << " to epoll" << RESET << std::endl;
        return -1;
    }

    return 0; // Successfully added fd to epoll
}

void Server::handle_client(int server_fd, const std::string &password, char ** /* av */)
{
    Commands commands;
    int count = 0;

    // Assuming _epoll_fd is initialized and the server_fd has been added to it
    // and _events is an array of epoll_event with MAX_EVENTS size.

	(void)password;
	

    while (1)
    {
        count = epoll_wait(_epoll_fd, _events, 5, -1);
        if (count == -1)
        {
            if (errno == EINTR)
            {
                // Interrupted by a signal, continue to wait
                continue;
            }
            std::cerr << RED << "Error in epoll_wait: " << strerror(errno) << RESET << std::endl;
            break;
        }

        for (int i = 0; i < count; i++)
        {
            if (_events[i].events & (EPOLLERR | EPOLLHUP))
            {
                std::cerr << RED << "Epoll error on fd: " << _events[i].data.fd << RESET << std::endl;
                close(_events[i].data.fd);
                // Handle client disconnection
                continue;
            }
            else if (_events[i].data.fd == server_fd)
            {
                // Handle new connections
                sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(client_address);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
                if (client_fd == -1)
                {
                    std::cerr << RED << "Error: Cannot accept client" << RESET << std::endl;
                    continue;
                }
                make_socket_non_blocking(client_fd); // You need to implement this
                add_fd_to_epoll(_epoll_fd, client_fd); // You need to implement this

                User user;
                user.fd = client_fd;
                user.has_authenticated = false;
                user.is_registered = false;
                users.push_back(user);
				std::cout << GREEN << "New client connected with fd: " << client_fd << RESET << std::endl;
            }
            else
            {
                // Handle data from clients
                User* user = find_user_by_fd(_events[i].data.fd);
                if (!user)
                {
                    std::cerr << RED << "Error: User not found for fd " << RESET << _events[i].data.fd << std::endl;
                    close(_events[i].data.fd);
                    continue;
                }

                if (!user->is_registered)
                {
                    // Authenticate user
                    // The authenticate_user function should now start reading from the start of the buffer
                    // since we're now inside the epoll loop and the data is ready to be read
                   /*  if (!authenticate_user(_events[i].data.fd, password, *user))
                    {
                        std::cerr << RED << "Error: User failed authentication." << RESET << std::endl;
                        close(_events[i].data.fd);
                        // Erase the user from the users list
                        // ...
                        continue;
                    } */
                }
                else
                {
                    // User is already authenticated, handle their commands
                    bool stillConnected = commands.handle_commands(_events[i].data.fd, *user);
                    if (!stillConnected)
                    {
                        close(_events[i].data.fd);
                        // Erase the user from the users list
                        // ...
                        continue;
                    }
                }
        }
    }
}
}