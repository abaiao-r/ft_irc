/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 09:39:14 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/04 11:22:49 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fcntl.h>
#include "Client.hpp"
#include "Channel.hpp"

#define C_IT std::vector<Client>::iterator
#define CH_IT std::vector<Channel>::iterator

int			unblock_socket(int fd);
int			password_checker(std::string password);
int			safe_atoi(const char *nptr);
bool 		isAlpha(const std::string& str);
bool 		strIsWhitespace(const std::string& str);
bool 		isStrOnlyDigits(const std::string& str);
std::string find_and_store_the_rest(std::string &str, std::string word_to_find);
