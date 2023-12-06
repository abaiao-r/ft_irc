/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 23:25:19 by andrefranci       #+#    #+#             */
/*   Updated: 2023/11/10 13:59:33 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <climits>
# include <iostream>

int		safe_atoi(const char *nptr);
bool 	isAlpha(const std::string& str);
void    printWelcome();

#endif