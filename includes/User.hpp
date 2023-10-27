/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:51:29 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/18 14:51:31 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "Ft_irc.hpp"
#include <string>
#include <vector>
#include <cstddef>

struct User
{
	std::string nickname;
	std::string username;
	std::string realname;
	bool is_admin;
	bool has_authenticated;
	bool is_registered;
	bool user_registered;
	bool nick_registered;
	int fd;

	User();
};

User* find_user_by_fd(int fd);
extern std::vector<User> users; // List of connected users
bool authenticate_user(int client_fd, const std::string& password, User &user);

#endif
