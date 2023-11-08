/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:30:20 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/08 12:04:02 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"

class Client;
class Channel
{
	private:
		std::string			_name;
		std::string			_password;
		std::string			_topic;
		int					_mode;
		bool				_dm;
		unsigned int		_max_users;
		std::vector<Client>	_clients_in_channel;
		std::vector<Client>	_admins;
	public:
		Channel();
		Channel(Client admin, std::string name, std::string password, std::string topic);
		Channel(Client &user1, Client &user2);
		Channel(const Channel &copy);
		~Channel();

		Channel				&operator=(const Channel &copy);
		bool				operator==(std::string copy);
		std::string			get_name() const;
		std::string			get_topic() const;
		int					get_mode() const;
		std::vector<Client>	&get_clients_in_channel();
		bool				is_dm() const;
		void				set_name(std::string name);
		void				set_topic(std::string topic);
		void				set_mode(int mode);
		Client				*find_client(const std::string &nickname);
		void				message(Client &client, std::string msg);
};
