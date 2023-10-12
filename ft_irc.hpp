#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <iostream>
#include <cstring>  // For memset
#include <fcntl.h>  // For fcntl
#include <netinet/in.h>  // For sockaddr_in, htons
#include <sys/socket.h>  // For socket, bind, listen, accept
#include <unistd.h>  // For close
#include <poll.h>  // For poll
#include <cstdlib>  // For atoi
#include <string>

struct User
{
	std::string nickname;
	std::string username;
	std::string realname;
	bool has_authenticated;
	bool is_registered;

	User() : has_authenticated(false), is_registered(false) {}
};

bool handle_pass(User& user, const std::string& message, const std::string& server_password);
bool handle_nick(User& user, const std::string& message);
bool handle_user(User& user, const std::string& message);


#endif
