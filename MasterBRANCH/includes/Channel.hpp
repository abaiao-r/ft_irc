/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-per <joao-per@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 14:52:25 by joao-per          #+#    #+#             */
/*   Updated: 2023/11/09 17:34:42 by joao-per         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Ft_irc.hpp"
# include "User.hpp"

class Channel
{
<<<<<<< HEAD:includes/Channel.hpp
	public:
		bool					is_private;
		bool					is_invite_only;
		bool					is_topic_settable;
		long unsigned int		limit;
		std::string name;
		std::vector<User> users_in_channel;
		std::string topic;
		std::string password;
		std::vector<std::string> ban_list;
		std::vector<std::string> invite_list;
		std::vector<std::string> operator_list;

		User admin;
=======
    public:
        std::string name;
        User admin; 
        std::vector<User> users_in_channel;
        std::string topic;

>>>>>>> irc2:MasterBRANCH/includes/Channel.hpp
};

#endif