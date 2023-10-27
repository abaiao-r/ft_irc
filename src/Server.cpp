/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:59:20 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/27 19:21:39 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Default constructor */
Server::Server() : _port(0), _server_fd(-1)
{
    std::cout << CYAN << "Default constructor Server called" << RESET 
        << std::endl;
    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET; // IPv4
    _address.sin_addr.s_addr = INADDR_ANY; // Any available network interface
    _address.sin_port = htons(_port); // Port
}
/* Parameter constructor */
Server::Server(int port) : _port(port), _server_fd(-1)
{
    std::cout << CYAN << "Default constructor Server called" << RESET 
        << std::endl;
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
    close_server();
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
bool Server::start_listening(void)
{
    if (listen(_server_fd, 5) == -1)
    {
        std::cerr << RED <<"Error:" << RESET << " Cannot listen socket"
            << std::endl;
        close_server();
        return (false);
    }
    std::cout << GREEN << "Server listening on port " << _port << RESET
        << std::endl;
    return (true);
}

/* bind_socket: Binds the socket to the address and port number
** bind() assigns the address specified by addr to the socket referred to by
** the file descriptor server_fd. addrlen specifies the size, in bytes, of the
** address structure pointed to by addr. Traditionally, this operation is
** called “assigning a name to a socket”. It is normally necessary to assign a
** local address using bind() before a SOCK_STREAM socket may receive
** connections.
*/
bool Server::bind_socket(void)
{
    if (bind(_server_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
    {
        std::cerr << RED <<"Error:" << RESET << " Cannot bind socket" 
            << std::endl;
        close_server();
        return (false);
    }
    return (true);
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
bool Server::set_socket_options(void)
{
    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, 
        sizeof(opt)) == -1) 
    {
        std::cerr << RED <<"Error:" << RESET << " Cannot set socket options" 
            << std::endl;
        close_server();
        return (false);
    }
    return (true);
}

/* create_socket: Creates the socket
** Note: socket() returns a file descriptor (int). The parameters are:
** 1. Domain: AF_INET (IPv4) or AF_INET6 (IPv6)
** 2. Type: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
** 3. Protocol: 0 (IP)
** Returns true if the socket is created successfully, false otherwise
*/
bool Server::create_socket(void)
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
    {
        std::cerr << RED <<"Error:" << RESET << " Cannot create socket" 
            << std::endl;
        return (false);
    }
    return (true);
}

/* init_server: Initializes the server
** 1. Creates the socket
** 2. Sets the socket options
** 3. Binds the socket
** 4. Starts listening
** Returns true if all steps are successful, false otherwise
*/
bool Server::init_server(void)
{
    if (!create_socket()) 
        return (false);
    if (!set_socket_options()) 
        return (false);
    if (!bind_socket()) 
        return (false);
    if (!start_listening())
        return (false);
    return (true);
}

/* listen_server: Accepts incoming connections
** accept() extracts the first connection request on the queue of pending
** connections for the listening socket, server_fd, creates a new connected
** socket, and returns a new file descriptor referring to that socket. The
** newly created socket is not in the listening state. The original socket
** server_fd is unaffected by this call.
*/
void Server::listen_server(void)
{
    socklen_t addrlen = sizeof(_address);
    int new_socket = accept(_server_fd, (struct sockaddr*)&_address, &addrlen);
    if (new_socket == -1)
    {
        std::cerr << RED <<"Error:" << RESET << " Cannot accept connection" 
            << std::endl;
        close_server();
        return ;
    }
    std::cout << GREEN << "New connection accepted" << RESET << std::endl;
}

/* close_server: Closes the socket */
void Server::close_server(void)
{
    if (_server_fd != -1)
        close(_server_fd);
    _server_fd = -1;
}
