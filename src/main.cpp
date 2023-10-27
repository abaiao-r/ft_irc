/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:25 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/25 18:30:21 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Client.hpp"
#include "utils.hpp"

int init_server(int port)
{
	/* The `socket()` function takes three arguments: the address domain (in this case `AF_INET` for IPv4), the type of socket
	(in this case `SOCK_STREAM` for a TCP socket), and the protocol (in this case `0` for the
	default protocol for the given address domain and socket type). */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1)
	{
		std::cerr << "Error: Cannot create socket" << std::endl;
		return (-1);
	}

	int opt = 1;
	/* The line `if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)` is
	setting a socket option to allow reusing the address. */
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error: Cannot set socket options" << std::endl;
		close(server_fd);
		return (-1);
	}

	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	/* `address.sin_family = AF_INET;` sets the address family of the socket to IPv4. */
	address.sin_family = AF_INET;
	/* the socket will accept connections from any available network interface on the machine. */
	address.sin_addr.s_addr = INADDR_ANY;
	/* `address.sin_port = htons(port);` is setting the port number of the server socket. The `htons()`
	function is used to convert the port number from host byte order to network byte order, which is
	the byte order used by the network protocol. This ensures that the port number is stored in the
	correct byte order for communication over the network. */
	address.sin_port = htons(port);
	/* it is binding the server socket `server_fd` to the IP address `INADDR_ANY` and the
	port number specified in the `address` structure. */
	if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		std::cerr << "Error: Cannot bind socket" << std::endl;
		close(server_fd);
		return (-1);
	}

	/* The `listen(server_fd, 5)` function is used to make the server socket `server_fd` start
	listening for incoming connections. The second argument `5` specifies the maximum number of
	pending connections that can be queued up before the server socket starts rejecting new
	connections. */
	if(listen(server_fd, 5) == -1)
	{
		std::cerr << "Error: Cannot listen on socket" << std::endl;
		close(server_fd);
		return (-1);
	}

	std::cout << "Server is listening on port " << port << std::endl;
	return server_fd;
}

int main(int ac, char **av)
{
	Client clients;
	if(ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return (1);
	}
	// Parse the port
	int port;
	try
	{
		port = safe_atoi(av[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	int server_fd = init_server(port);
	if(server_fd == -1)
		return (1);
	clients.handle_client(server_fd, av[2], av);
	
	close(server_fd);
	return (0);
}
