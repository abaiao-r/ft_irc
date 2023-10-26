/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:51:11 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/18 14:51:13 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "User.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>

bool handle_join(User& user);
bool handle_msg(User& user, const std::string& message);
bool handle_privmsg(User& user, const std::string& message);
void handle_commands(int client_fd, User &user);

#endif