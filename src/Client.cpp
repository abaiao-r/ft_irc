/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:34:09 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/02 16:11:47 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Commands.hpp"

/* Default constructor */
Client::Client(void)
{
	std::cout << CYAN << "Client: Default constructor called" << RESET 
		<< std::endl;
}

/* copy constructor: is private  so we can't copy */
Client::Client(const Client &src)
{
	std::cout << RED << "Client:copy constructor is private" << RESET
		<< std::endl;
	*this = src;
}

/* destructor */
Client::~Client(void)
{
	std::cout << RED << "Client: Destructor called" << RESET 
		<< std::endl;
}

/* assignment operator: is private so we can't copy */
Client &Client::operator=(const Client &src)
{
	std::cout << RED << "Client: Assignment operator is private" << RESET
		<< std::endl;
	if (this != &src)
		clients = src.clients;
	return (*this);
}

// Function to handle a new client connection
int Client::handleNewClient(int server_fd)
{
    sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);

    if (client_fd == -1)
	{
        std::cerr << "Error: Cannot accept client" << std::endl;
        return (-1);
    }

	// -3 because the first client always starts on 4th socket
    std::cout << "Client number " << client_fd - 3 << " connected" << std::endl;

    pollfd client_pfd;
    client_pfd.fd = client_fd;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;
    clients.push_back(client_pfd);

    User user;
    user.fd = client_fd;
    user.has_authenticated = false;
    user.is_registered = false;
    users.push_back(user);

	return (client_fd);
}

// Function to handle an authenticated client
int Client::handleAuthenticatedClient(int client_fd, const std::string &password)
{
    User* user = find_user_by_fd(client_fd);
	Commands commands;
    if (!user)
	{
       	std::cerr << RED << "Error: " << RESET << "User not found for fd " 
			<< client_fd << std::endl;
        //close(client_fd);
        // Remove the client from the clients vector
        //removeClient(client_fd);
        return (-1);
    }

    if (!user->has_authenticated)
	{
        if (!authenticate_user(client_fd, password, *user))
		{
			std::cerr << RED << "Error: " << RESET
				<< user << " failed to authenticate" << std::endl;
			return (-1);
        }
    }
    else
	{
        bool stillConnected = commands.handle_commands(client_fd, *user);
        if (!stillConnected)
		{
			// number of client is client fd - 3
			std::cout << "Client " << client_fd - 3 << " disconnected" 
				<< std::endl;
			return (-1);
        }
    }
	return (0);
}

// Function to handle client connections and authentication
void Client::handle_client(int server_fd, const std::string &password, char ** /* av */)
{
    pollfd server_pfd;
    server_pfd.fd = server_fd;
    server_pfd.events = POLLIN;
    clients.push_back(server_pfd);

    while (1)
	{
        int poll_ret = poll(&clients[0], clients.size(), -1);

        if (poll_ret == -1)
		{
            std::cerr << "Error: poll failed" << std::endl;
            break;
        }

        for (size_t i = 0; i < clients.size(); ++i)
		{
            if (clients[i].revents & POLLIN)
			{
                if (clients[i].fd == server_fd)
				{
					if (handleNewClient(server_fd) == -1)
						continue;
                } 
				else 
				{
                    if (handleAuthenticatedClient(clients[i].fd, password) == -1)
					{
						std::cerr << RED << "Error: " << RESET
							<< "handleAuthenticatedClient failed" << std::endl;
						close(clients[i].fd);
						clients.erase(clients.begin() + i);
						i--;
						continue;
					}
                }
            } 
			else if (clients[i].revents & (POLLHUP | POLLERR)) 
			{
                int client_fd = clients[i].fd;
                // Remove the client from the clients vector
                close(clients[i].fd);
                User* user = find_user_by_fd(client_fd);
                if (user)
				{
                    std::vector<User>::iterator it = std::find(users.begin(), users.end(), *user);
                    if (it != users.end())
                        users.erase(it);
                }
                close(client_fd);
            }
            clients[i].revents = 0;
        }
    }
}


/* void Client::handle_client(int server_fd, const std::string &password, char **  av)
{
	Commands commands;
	pollfd server_pfd;
	server_pfd.fd = server_fd;
	server_pfd.events = POLLIN;
	clients.push_back(server_pfd);
	
	while(1)
	{
		int poll_ret = poll(&clients[0], clients.size(), -1);
		
		if(poll_ret == -1)
		{
			std::cerr << "Error: poll failed" << std::endl;
			break;
		}
		
		for (size_t i = 0; i < clients.size(); ++i)
		{
			if (clients[i].revents & POLLIN)
			{
				if (clients[i].fd == server_fd)
				{
					sockaddr_in client_address;
					socklen_t client_addrlen = sizeof(client_address);
					int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
					
					if(client_fd == -1)
					{
						std::cerr << "Error: Cannot accept client" << std::endl;
						continue;
					}
					// -3 because the first client always starts on 4th socket
					std::cout << "Client number " << client_fd - 3 << " connected" << std::endl;
					pollfd client_pfd;
					client_pfd.fd = client_fd;
					client_pfd.events = POLLIN;
					client_pfd.revents = 0;
					clients.push_back(client_pfd);

					User user;
					user.fd = client_fd;
					user.has_authenticated = false;
					user.is_registered = false;
					users.push_back(user);
				}
				else
				{
					User* user = find_user_by_fd(clients[i].fd);
					if(!user)
					{
						std::cerr << "Error: User not found for fd " << clients[i].fd << std::endl;
						close(clients[i].fd);
						clients.erase(clients.begin() + i);
						i--;
						continue;
					}

					if (!user->has_authenticated)
					{
						if (!authenticate_user(clients[i].fd, password, *user))
						{
							std::cerr << "Error: User failed authentication." << std::endl;
							close(clients[i].fd);
							clients.erase(clients.begin() + i);
							i--;
							continue;
						}
					}
					else
					{
						bool stillConnected = commands.handle_commands(clients[i].fd, *user);
						if(!stillConnected)
						{
							close(clients[i].fd);
							clients.erase(clients.begin() + i);
							i--;
							continue;
						}
					}
				}
			}
			else if (clients[i].revents & (POLLHUP | POLLERR))
			{
				User* user = find_user_by_fd(clients[i].fd);
				if(user)
				{
					std::vector<User>::iterator it = std::find(users.begin(), users.end(), *user);
					if (it != users.end())
						users.erase(it);
				}

				close(clients[i].fd);
				clients.erase(clients.begin() + i);
				i--;
			}

			clients[i].revents = 0;
		}
	}
} */

bool operator==(const User& lhs, const User& rhs)
{
	return lhs.fd == rhs.fd;
}