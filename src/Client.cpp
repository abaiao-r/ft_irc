/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:34:09 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/10 12:25:40 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Commands.hpp"
#include "colours.hpp"

Client::Client() {} // Constructor

Client::~Client() {} // Destructor

std::map<int, std::string> clientBuffers;  // FD -> Accumulated commands

bool hasAllCommands(const std::string& buffer)
{
	return (buffer.find("CAPS") != std::string::npos &&
			buffer.find("NICK") != std::string::npos &&
			buffer.find("USER") != std::string::npos &&
			buffer.find("PASS") != std::string::npos);
}

void Client::handle_client(int server_fd, const std::string &password, char ** /* av */)
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
			std::cerr << RED << "Error: poll failed" << RESET << std::endl;
			break;
		}
		// Check if the server socket is ready to read
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
						std::cerr << RED << "Error: Cannot accept client" << RESET << std::endl;
						continue;
					}
					// -3 because the first client always starts on 4th socket
					std::cout << "\033[33mClient number " << client_fd - 3 << " \033[32mconnected\033[0m" << std::endl;
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
						std::cerr << RED << "Error: User not found for fd " << RESET << clients[i].fd << std::endl;
						close(clients[i].fd);
						clients.erase(clients.begin() + i);
						i--;
						continue;
					}

					if (!user->is_registered)
					{
						char buffer[1024];
						int bytesReceived = recv(clients[i].fd, buffer, sizeof(buffer) - 1, 0);
						if (bytesReceived <= 0) 
						{
							std::cerr << RED << "Error reading from client or client disconnected." << RESET << std::endl;
							close(clients[i].fd);
							clients.erase(clients.begin() + i);
							i--;
							continue;
						}
						buffer[bytesReceived] = '\0';
						std::string initialCommand = std::string(buffer);
						// DEBUG std::cout << "Received:" << initialCommand << std::endl;
						if (initialCommand.substr(0, 3) == "CAP")
							clientBuffers[clients[i].fd] = initialCommand; // Store CAP command
						if (clientBuffers.count(clients[i].fd))
						{
							// If we have a stored CAP command for this client, process both messages
							if (!handle_hexchat(clientBuffers[clients[i].fd], initialCommand, *user, password, clients[i].fd))
							{
								std::cerr << RED << "Error: User failed Hexchat authentication." << RESET << std::endl;
								close(clients[i].fd);
								clients.erase(clients.begin() + i);
								i--;
								continue;
							}
							//clientBuffers.erase(clients[i].fd);
						}
						else
						{
							//if initialCommand has a \n or \r\n, remove it
							if(initialCommand.find("\n") != std::string::npos)
								initialCommand.erase(initialCommand.find("\n"));
							if (!authenticate_user(clients[i].fd, initialCommand, password, *user))
							{
								std::cerr << RED << "Error: User failed authentication." << RESET << std::endl;
								close(clients[i].fd);
								clients.erase(clients.begin() + i);
								i--;
								continue;
							}
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
}

bool operator==(const User& lhs, const User& rhs)
{
	return lhs.fd == rhs.fd;
}