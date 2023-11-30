/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:30:20 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/30 12:52:55 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "Client.hpp"
#include "colours.hpp"

#define STRING_VEC std::vector<std::string>

class Client;

class Channel
{
	private:
		std::string			_name;
		std::string			_password;
		std::string			_topic;
		bool 				_topic_mode;
		unsigned int 		_channel_limit;
		bool				_channel_invite_only;
		std::vector<Client>	_clients_in_channel;
		// clients banned from channel
		std::vector<Client>	_clients_banned;
		// clients that are operators of the channel
		std::vector<Client> _clients_operator_channel;
		// clients on the invite vector
		std::vector<Client> _clients_invited_to_channel;
		std::string	_vectors[4];

	public:
		Channel();
		Channel(std::string name);
		Channel(std::string name, std::string password);
		Channel(const Channel &copy);
		~Channel();

		Channel				&operator=(const Channel &copy);
		bool				operator==(std::string copy);

		std::string			get_name() const;
		void				set_name(std::string name);

		std::string			get_password(void) const;
		void				set_password(std::string password);

		std::string			get_topic() const;
		void				set_topic(std::string topic);

		bool 				get_topic_mode() const;
		void				set_topic_mode(bool topic_mode);

		std::string			get_mode() const;
		void				set_mode(std::string mode);

		unsigned int 		get_channel_limit(void) const;
		void				set_channel_limit(unsigned channel_limit);

		bool 				get_channel_invite_only(void) const;
		void				set_channel_invite_only(bool channel_invite_only);

		std::vector<Client>	&get_clients_in_channel(void);
		void				add_client(Client &client);
		void				remove_client(Client &client);

		std::vector<Client> &get_clients_banned(void);
		void 				add_client_to_banned_vector(Client &client);
		void 				remove_client_from_banned_vector(Client &client);

		std::vector<Client> &get_clients_operator_channel(void);
		Client				&get_operator();
		void				check_operator();
		void 				add_client_to_clients_operator_vector(Client &client);
		void 				remove_client_from_clients_operator_vector(Client &client);


		std::vector<Client> &get_clients_invited_to_channel(void);
		void 				add_client_to_clients_invited_vector(Client &client);
		void 				remove_client_from_clients_invited_vector(Client &client);

		Client				*find_client(std::string nickname, std::string vector);
		void				message(Client &client, std::string msg);
		void				message(Client &client, std::string msg, std::string code);
		void				info_message(std::string msg);
		int 				sendSuccessMessage(int client_fd, const std::string	&successMessage);
};
