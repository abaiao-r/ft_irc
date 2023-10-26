#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "User.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <map>
#include <sys/socket.h>
#include <poll.h>

class Client
{
    public:
        Client(); // Constructor
        ~Client(); // Destructor

        void handle_client(int server_fd, const std::string &password, char ** /* av */);
    private:
        std::vector<pollfd> clients;
};

#endif