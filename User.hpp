#ifndef USER_HPP
#define USER_HPP

#include "User.hpp"
#include <string>
#include <vector>
#include <cstddef>


struct User
{
	std::string nickname;
	std::string username;
	std::string realname;
	bool has_authenticated;
	bool is_registered;
	bool user_registered;
	bool nick_registered;
	int fd;

	User();
};

User* find_user_by_fd(int fd);
extern std::vector<User> users; // List of connected users

#endif
