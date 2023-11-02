/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:25 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/02 13:04:20 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

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
	
	Server server(port);
	
	if (server.init_server() == -1) // not false but if equal to -1
		return (1);
	clients.handle_client(server.get_server_fd(), av[2], av);
	
	server.close_server();
	return (0);
}
