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

#include "Ft_irc.hpp"
#include "User.hpp"
#include "Commands.hpp"
#include "utils.hpp"
#include "Client.hpp"
#include "colours.hpp"
#include <vector>
#include <algorithm>

int init_server(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        std::cerr << "Error: Cannot create socket" << std::endl;
        return (-1);
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Error: Cannot set socket options" << std::endl;
        close(server_fd);
        return (-1);
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
    {
        std::cerr << "Error: Cannot bind socket" << std::endl;
        close(server_fd);
        return (-1);
    }

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


