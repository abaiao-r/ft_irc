#include "User.hpp"
#include <vector>

std::vector<User> users;  // List of connected users

User::User() : is_registered(false), fd(-1) {}

User* find_user_by_fd(int fd)
{
	for (std::size_t i = 0; i < users.size(); i++)
	{
		if (users[i].fd == fd)
			return &users[i];
	}
	return NULL;
}
