#include <iostream>
#include <cstring>  // For memset
#include <fcntl.h>  // For fcntl
#include <netinet/in.h>  // For sockaddr_in, htons
#include <sys/socket.h>  // For socket, bind, listen, accept
#include <unistd.h>  // For close
#include <poll.h>  // For poll
#include <cstdlib>  // For atoi

int main(int ac, char **av)
{
	if(ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	
	const int port = std::atoi(av[1]);
	// Password is stored in av[2]
	
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
		return 1;
	}

	// Listening
	if(listen(server_fd, 5) == -1)
	{
		std::cerr << "Error: Cannot listen on socket" << std::endl;
		close(server_fd);
		return 1;
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
					std::cout << "Client connected" << std::endl;
					// Add client communication

					close(client_fd);
				}
			}
		}
	}
	
	close(server_fd);
	return (0);
}
