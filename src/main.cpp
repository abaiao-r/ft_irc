/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:25 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/11 19:27:19 by joao-per         ###   ########.fr       */
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
		if (port < 6667 || port > 6697)
		{
			std::cerr << "Error: Invalid port. Availability: 6667 - 6697." << std::endl;
			throw std::exception();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	
	Server server(port);
	
	printWelcome();
	if (server.init_server() == -1) // not false but if equal to -1
		return (1);
	server.handle_client(server.get_server_fd(), av[2], av);
	
	close(server.get_server_fd());
	return (0);
}
