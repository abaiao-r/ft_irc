#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "User.hpp"
#include <string>
#include <vector>
#include <cstddef>
#include <map>

void handle_client(int server_fd, const std::string &password, char ** /* av */);

#endif