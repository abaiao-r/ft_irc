/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_user.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:34:03 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/09 14:50:13 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLE_USER_HPP
# define HANDLE_USER_HPP

# include "User.hpp"
# include "Ft_irc.hpp"

bool	handle_pass(User& user, const std::string& message, const std::string& server_password);
bool	handle_nick(User& user, const std::string& message);
bool	handle_user(User& user, const std::string& message);


#endif
