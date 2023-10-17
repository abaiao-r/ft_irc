#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "User.hpp"
#include <string>
#include <map>
#include <vector>

bool handle_join(User& user);
bool handle_msg(User& user, const std::string& message);
bool handle_privmsg(User& user, const std::string& message);

#endif