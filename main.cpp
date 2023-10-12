#include "ft_irc.hpp"


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
		return (1);
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
	/* The `listen` function is
	used to make the server socket start listening for incoming client connections. */
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
					// Handle PASS authentication first
					int retry_count = 0;
					const int max_retries = 3;  // Example limit

					while (retry_count < max_retries && !user.has_authenticated)
					{
						ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
						buffer[n] = '\0';  // Null terminate the received message

						std::string message(buffer);
						
						if (message.find("PASS") != 0 || !handle_pass(user, message, av[2]))
						{
							send(client_fd, "ERROR: Invalid password. Please try again.\r\n", 46, MSG_NOSIGNAL);
							retry_count++;
							if (retry_count >= max_retries)
							{
								send(client_fd, "ERROR: Too many incorrect attempts. Closing connection.\r\n", 60, MSG_NOSIGNAL);
								close(client_fd);
								return (1);  // Or continue to wait for another client connection
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
