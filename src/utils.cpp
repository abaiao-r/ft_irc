/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 23:24:43 by andrefranci       #+#    #+#             */
/*   Updated: 2023/10/25 16:13:09 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"


/* safe_atoi: Converts a string to an integer.
 * Returns: The integer value of the string.
 * Throws: std::out_of_range if the input string is not a valid integer.
 * How does it check for overflow and underflow?
 * 1. It checks for overflow by comparing the result to INT_MAX / 10. If the
 *   result is greater than INT_MAX / 10, then multiplying the result by 10
 *  will cause an overflow.
 * 2. If the result is equal to INT_MAX / 10, then multiplying the result by
 *  10 will cause an overflow if the next digit is greater than INT_MAX % 10.
 * 3. It checks for underflow by comparing the result to INT_MIN / 10. If the
 *  result is less than INT_MIN / 10, then multiplying the result by 10
 * will cause an underflow.
 * 4. If the result is equal to INT_MIN / 10, then multiplying the result by
 * 10 will cause an underflow if the next digit is greater than -(INT_MIN % 10).
 * 5. The function also checks for overflow and underflow by comparing the
 * result to INT_MAX and INT_MIN, respectively.
 */
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
			throw std::out_of_range("Integer overfloww or underflow "
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