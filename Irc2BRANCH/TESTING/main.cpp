/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 10:20:34 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 17:19:43 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerUtils.hpp"

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage: ircserv <port> <password>\n";
		return 1;
	}
	try
	{
		if (!isStrOnlyDigits(av[1]))
			throw std::runtime_error("Port must be am integer");
		int port = safe_atoi(av[1]);
		if (port < 1024 || port > 49151)
			throw std::runtime_error("Port must be between 1024 and 49151");
		Server	irc(port, av[2]);
	}
	catch(const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
