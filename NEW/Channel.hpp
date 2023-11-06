/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:30:20 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/06 16:42:40 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"

class Client;
class Channel
{
	private:
		std::string			_name;
		//IS THIS NECESSARY?
		std::string			_password;
		std::string			_topic;
		int					_mode;
		bool				_dm;
		unsigned int		_max_users;
		std::vector<Client>	_clients_in_channel;
	public:
		Channel();
		Channel(std::string name, std::string password, std::string topic);
		Channel(Client &user1, Client &user2);
		Channel(Channel &copy);
		~Channel();
		Channel				&operator=(Channel &copy);
		bool				operator==(std::string copy);
		std::string			get_name() const;
		std::string			get_topic() const;
		int					get_mode() const;
		std::vector<Client>	&get_clients_in_channel();
		bool				is_dm() const;
		void				set_name(const Client &user, std::string name);
		void				set_topic(const Client &user, std::string topic);
		void				set_mode(const Client &user, int mode);
};
