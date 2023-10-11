#include <iostream>
#include <cstring>  // For memset
#include <fcntl.h>  // For fcntl
#include <netinet/in.h>  // For sockaddr_in, htons
#include <sys/socket.h>  // For socket, bind, listen, accept
#include <unistd.h>  // For close
#include <poll.h>  // For poll
#include <cstdlib>  // For atoi

struct User
{
    std::string nickname;
    std::string username;
    std::string realname;
    bool is_registered;
};

bool handle_nick(User& user, const std::string& message)
{
    // Extract nickname from message
    // Format: NICK <nickname>
    size_t space_pos = message.find(' ');
    if(space_pos == std::string::npos || space_pos + 1 == message.size())
	{
        // Invalid message format
        return false;
    }
    std::string nickname = message.substr(space_pos + 1);

	//check if nickname already in user
    // If it is, return false
	if(user.nickname == nickname)
		return false;
	
    user.nickname = nickname;
    return true;
}


bool handle_user(User& user, const std::string& message)
{
    // Extract user details from message
    // Format: USER <username> <hostname> <servername> :<realname>
    size_t space_pos = message.find(' ');
    // Invalid message format
    if(space_pos == std::string::npos || space_pos + 1 == message.size())
        return false;
    std::string rest_of_message = message.substr(space_pos + 1);
    size_t colon_pos = rest_of_message.find(':');
    // Invalid message format
    if(colon_pos == std::string::npos)
        return (false);

    // Extracting and setting username and realname
    std::string username = rest_of_message.substr(0, rest_of_message.find(' '));
    std::string realname = rest_of_message.substr(colon_pos + 1);

    user.username = username;
    user.realname = realname;
    return (true);
}


int main(int ac, char **av)
{
	if(ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}

	const int port = std::atoi(av[1]);
	// Password is stored in av[2]
	// Need to do implementation to check password
	
	// Socket creation
	int server_fd;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1)
	{
		std::cerr << "Error: Cannot create socket" << std::endl;
		return 1;
	}

	// Socket options
	int opt = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error: Cannot set socket options" << std::endl;
		close(server_fd);
		return 1;
	}

	// Binding the socket
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		std::cerr << "Error: Cannot bind socket" << std::endl;
		close(server_fd);
		return (1);
	}

	// Listening
	if(listen(server_fd, 5) == -1)
	{
		std::cerr << "Error: Cannot listen on socket" << std::endl;
		close(server_fd);
		return (1);
	}
	
	// Polling setup
	pollfd pfd;
	pfd.fd = server_fd;
	pfd.events = POLLIN;
	
	std::cout << "Server is listening on port " << port << std::endl;

	while(1)
	{
		// Polling
		int poll_ret = poll(&pfd, 1, -1);
		if(poll_ret == -1)
		{
			std::cerr << "Error: poll failed" << std::endl;
			break ;
		}
		else if(poll_ret > 0)
		{
			// Checking if the `POLLIN` event has occurred on the file descriptor
			if(pfd.revents & POLLIN)
			{
				// Accepting client connection
				sockaddr_in client_address;
				socklen_t client_addrlen = sizeof(client_address);
				// `accept` is a blocking call and will wait until a client connects
				int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
				if(client_fd == -1)
					std::cerr << "Error: Cannot accept client" << std::endl;
				else
				{
					// Client connected
					std::cout << "Client connected" << std::endl;

					User user;
					user.is_registered = false;

					char buffer[512];
					while(!user.is_registered)
					{
						// Receive message from client
						ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
						if(n <= 0)
						{
							std::cerr << "Error: Failed to receive message or client disconnected" << std::endl;
							close(client_fd);
							break;
						}
						buffer[n] = '\0';  // Null terminate the received message

						std::string message(buffer);

						if(message.find("NICK") == 0)
						{
							// Send error message to client
							if(!handle_nick(user, message))
								send(client_fd, "ERROR :Nickname is already in use or not valid\r\n", 37, MSG_NOSIGNAL);
						}
						else if(message.find("USER") == 0)
						{
							// Send error message to client
							if(!handle_user(user, message))
								send(client_fd, "ERROR :Username is invalid\r\n", 29, MSG_NOSIGNAL);
						}

						// Check if user is now registered
						if(!user.nickname.empty() && !user.username.empty())
						{
							user.is_registered = true;

							// Send welcome messages...
							std::string welcome_message = "Welcome to the Internet Relay Network " + user.nickname + "!" + user.username + "@" + user.username + "\r\n";
							send(client_fd, welcome_message.c_str(), welcome_message.length(), MSG_NOSIGNAL);
							std::string your_host_message = "Your host is " + std::string(av[0]) + ", running version 1.0\r\n";
							send(client_fd, your_host_message.c_str(), your_host_message.length(), MSG_NOSIGNAL);
							std::string created_message = "This server was created abaiao-r and joao-per\r\n";
							send(client_fd, created_message.c_str(), created_message.length(), MSG_NOSIGNAL);
						}
					}
					close(client_fd);
				}
			}
		}
	}
	close(server_fd);
	return (0);
}
