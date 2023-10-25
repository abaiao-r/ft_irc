/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:54:25 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/25 16:18:07 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ft_irc.hpp"
#include "User.hpp"
#include "Commands.hpp"
#include "utils.hpp"
#include "colours.hpp"

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

void handle_client(int server_fd, const std::string &password, char ** /* av */)
{
    pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    
    while(1)
    {
        int poll_ret = poll(&pfd, 1, -1);
        if(poll_ret == -1)
        {
            std::cerr << "Error: poll failed" << std::endl;
            break;
        }
        else if(poll_ret > 0 && (pfd.revents & POLLIN))
        {
            sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
            if(client_fd == -1)
            {
                std::cerr << "Error: Cannot accept client" << std::endl;
            }
            else
            {
                std::cout << "Client connected" << std::endl;
                User user;
                user.is_registered = false;
                
                authenticate_user(client_fd, password, user);
                if(user.is_registered)
                {
                    handle_commands(client_fd, user);
                }
                close(client_fd);
            }
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


