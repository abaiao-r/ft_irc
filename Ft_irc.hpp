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
#include "User.hpp"

/* struct User
{
	std::string nickname;
	std::string username;
	std::string realname;
	bool has_authenticated;
	bool is_registered;
	bool user_registered;	// Indicates if USER command has been set successfully
	bool nick_registered;	// Indicates if NICK command has been set successfully
	int fd;					// file descriptor associated with this user
	

	User() : user_registered(false), nick_registered(false) {}
}; */


bool handle_pass(User& user, const std::string& message, const std::string& server_password);
bool handle_nick(User& user, const std::string& message);
bool handle_user(User& user, const std::string& message);


#endif
