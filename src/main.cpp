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
#include "colours.hpp"
#include <vector>
#include <algorithm>

bool operator==(const User& lhs, const User& rhs)
{
    return lhs.fd == rhs.fd;
}

int init_server(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        std::cerr << "Error: Cannot create socket" << std::endl;
        return -1;
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Error: Cannot set socket options" << std::endl;
        close(server_fd);
        return -1;
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
        return -1;
    }

    if(listen(server_fd, 5) == -1)
    {
        std::cerr << "Error: Cannot listen on socket" << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "Server is listening on port " << port << std::endl;
    return server_fd;
}
bool authenticate_user(int client_fd, const std::string& password, User &user)
{
    char buffer[512];
    // Handle PASS authentication first
    int retry_count = 0;
    const int max_retries = 3;  // Example limit
    while (retry_count < max_retries && !user.has_authenticated)
    {
		std::cout << "Waiting for PASS message" << std::endl;
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        buffer[n] = '\0';  // Null terminate the received message

        std::string message(buffer);
        
        if (message.find("PASS") != 0 || !handle_pass(user, message, password))
        {
            send(client_fd, "ERROR: Invalid password. Please try again.\r\n", 46, MSG_NOSIGNAL);
            retry_count++;
            if (retry_count >= max_retries)
            {
                send(client_fd, "ERROR: Too many incorrect attempts. Closing connection.\r\n", 60, MSG_NOSIGNAL);
                close(client_fd);
                return false;
            }
        }
    }
	while(!user.is_registered)
	{
		// Receive message from client
		ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if(n <= 0)
		{
			std::cerr << "Error: Failed to receive message or client disconnected" << std::endl;
			close(client_fd);
			break ;
		}
		buffer[n] = '\0';  // Null terminate the received message

		std::string message(buffer);

		if(message.find("NICK") == 0)
		{
			if(!handle_nick(user, message))
				send(client_fd, "ERROR: Nickname is invalid or already in use\r\n", 46, MSG_NOSIGNAL);
			else if(user.user_registered && user.nick_registered)
				send(client_fd, "SUCCESS: You are fully authenticated!\r\n", 40, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Nickname set successfully!\r\n", 37, MSG_NOSIGNAL);
		}
		else if(message.find("USER") == 0)
		{
			if(!handle_user(user, message))
				send(client_fd, "ERROR: Realname is invalid\r\n", 30, MSG_NOSIGNAL);
			else if(user.user_registered && user.nick_registered)
				send(client_fd, "SUCCESS: You are fully authenticated!\r\n", 40, MSG_NOSIGNAL);
			else
				send(client_fd, "SUCCESS: Realname set successfully!\r\n", 37, MSG_NOSIGNAL);
		}


		// Check if user is now registered
		if(!user.nickname.empty() && !user.realname.empty())
		{
			user.is_registered = true;

			// Send welcome messages...
			std::string welcome_message = "Welcome to the Internet Relay Network " + user.nickname + "!" + user.username + "@" + user.username + "\r\n";
			send(client_fd, welcome_message.c_str(), welcome_message.length(), MSG_NOSIGNAL);
			std::string your_host_message = "Your host is running on version 0.1\r\n";
			send(client_fd, your_host_message.c_str(), your_host_message.length(), MSG_NOSIGNAL);
			std::string created_message = "This server was created abaiao-r, joao-per and gacorrei \r\n";
			send(client_fd, created_message.c_str(), created_message.length(), MSG_NOSIGNAL);
		}
	}
	std::cout << "User Registered" << std::endl;
	std::cout << user.has_authenticated << std::endl;
    return user.has_authenticated;
}

void handle_commands(int client_fd, User &user)
{
    char buffer[512];
    bool client_connected = true;
    while(client_connected)
    {
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if(n <= 0)
        {
            std::cerr << "Error: Failed to receive message or client disconnected" << std::endl;
            client_connected = false;
            break;
        }
        buffer[n] = '\0';
        std::string message(buffer);
        std::cout << "Received message: " << message << std::endl;
        if(message.find("JOIN") == 0)
            handle_join(user);  // Assuming handle_join is previously defined
        else if(message.find("MSG") == 0)
            handle_msg(user, message);  // Assuming handle_msg is previously defined
        else if(message.find("PRIVMSG") == 0)
            handle_privmsg(user, message);  // Assuming handle_privmsg is previously defined
        // Add more command handlers as needed
    }
}

std::vector<pollfd> clients;
void handle_client(int server_fd, const std::string &password, char ** /* av */)
{
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

                    std::cout << "Client connected" << std::endl;
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
                        handle_commands(clients[i].fd, *user);
                    }
                }
            }
            else if (clients[i].revents & (POLLHUP | POLLERR))
            {
                User* user = find_user_by_fd(clients[i].fd);
                if(user)
                {
                    std::vector<User>::iterator it = std::find(users.begin(), users.end(), *user);
                    if (it != users.end()) users.erase(it);
                }

                close(clients[i].fd);
                clients.erase(clients.begin() + i);
                i--;
            }

            clients[i].revents = 0;
        }
    }
}



int main(int ac, char **av)
{
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
    {
        return 1;
    }
    
    handle_client(server_fd, av[2], av);
    
    close(server_fd);
    return 0;
}


