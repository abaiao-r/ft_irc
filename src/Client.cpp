#include "Ft_irc.hpp"
#include "User.hpp"
#include "Commands.hpp"
#include "utils.hpp"
#include "Client.hpp"
#include "colours.hpp"
#include <vector>
#include <algorithm>

Client::Client() {} // Constructor

Client::~Client() {} // Destructor

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
                        commands.handle_commands(clients[i].fd, *user);
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