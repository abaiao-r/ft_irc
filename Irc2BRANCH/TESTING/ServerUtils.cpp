/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 09:39:52 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/04 13:24:30 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerUtils.hpp"
#include <limits.h>

int	unblock_socket(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "Error when setting O_NONBLOCK flag\n";
		return -1;
	}
	return 0;
}

/* password_checker: check if password is valid
 * 1. Check if password is between 3 and 12 characters
 * 2. Check if password contains non-printable characters
 */
int	password_checker(std::string password)
{
	// Check if password is between 3 and 12 characters
	if (password.length() < 3 || password.length() > 12)
	{
		std::cerr << RED << "ERROR: " << RESET 
			<< "Password must be between 3 and 12 characters" << std::endl;
		return (1);
	}
	// Check if password contains non-printable characters
	for (size_t i = 0; i < password.length(); i++)
	{
		if (!isprint(password[i]))
		{
			std::cerr << RED << "ERROR: " << RESET 
				<< "Password must not contain non-printable characters" << std::endl;
			return (1);
		}
	}
	return (0);
}

int	safe_atoi(const char *nptr)
{
	int	i = 0;
	int	sign = 1;
	int	result = 0;

	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (nptr[i] == '+')
		i++;
	while (nptr[i] != '\0' && nptr[i] >= '0' && nptr[i] <= '9')
	{
		if (result > INT_MAX / 10 || (result == INT_MAX / 10 && (nptr[i]
					- '0') > (sign == 1 ? INT_MAX % 10 : -(INT_MIN % 10))))
		{
			throw std::out_of_range("Integer overflow or underflow "
					+ std::string(nptr));
		}
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}

bool isAlpha(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!::isalpha(*it))
			return (false);
	}
	return (true);
}

bool strIsWhitespace(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!::isspace(*it))
			return (false);
	}
	return (true);
}


/* find_and_store_the_rest:
** 1. Find the word_to_find in str.
** 2. If found, store the rest of the string in rest.
** If not found, rest is empty. str == "".
** 3. Return rest.
*/
std::string find_and_store_the_rest(std::string &str, std::string word_to_find)
{
	std::string rest;
	size_t pos = str.find(word_to_find);

	if (pos != std::string::npos)
	{
		rest = str.substr(pos + word_to_find.length());
	}
	else
	{
		throw std::invalid_argument("Word not found in string.");
	}
	return (rest);
}

/*isStrOnlyDigits:
** 1. Check if str is only digits.
** 2. If yes, return true.
** 3. If not, return false.
*/
bool isStrOnlyDigits(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!std::isdigit(*it))
			return (false);
	}
	return (true);
}
