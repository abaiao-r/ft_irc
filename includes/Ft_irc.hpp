/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 14:51:19 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/10/18 14:51:21 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

bool handle_pass(User& user, const std::string& message, const std::string& server_password);
bool handle_nick(User& user, const std::string& message);
bool handle_user(User& user, const std::string& message);


#endif
