/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:38:53 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/28 16:48:08 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <climits>

int	safe_atoi(const char *nptr);
bool isAlpha(const std::string& str);
bool strIsWhitespace(const std::string& str);
std::string find_and_store_the_rest(std::string &str, std::string word_to_find);
bool isStrOnlyDigits(const std::string& str);

#endif
