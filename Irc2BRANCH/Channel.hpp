/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:30:20 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/10 18:42:59 by abaiao-r         ###   ########.fr       */
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
		bool 				_topic_mode;
		int					_mode;
		unsigned int 		_channel_limit;
		bool				_channel_invite_only;
		std::vector<Client>	_clients_in_channel;
		// clients banned from channel
		std::vector<Client>	_clients_banned;
		// clients that are operators of the channel
		std::vector<Client> _clients_operator_channel;
		// clients on the invite vector
		std::vector<Client> _clients_invited_to_channel;

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

		int					get_mode() const;
		void				set_mode(int mode);

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
		void 				add_client_to_clients_operator_vector(Client &client);
		void 				remove_client_from_clients_operator_vector(Client &client);


		std::vector<Client> &get_clients_invited_to_channel(void);
		void 				add_client_to_clients_invited_vector(Client &client);
		void 				remove_client_from_clients_invited_vector(Client &client);

		Client				*find_client_in_channel_by_nickname(std::string &nickname_to_find);
		Client				*find_banned_client(const std::string &client_banned);
		Client				*find_banned_client_by_nickname(std::string &nickname_to_find);
		Client				*find_clients_operator_channel(Client &client);
		Client				*find_clients_operator_channel(std::string &nickname_to_find);
		Client 				*find_clients_invited_to_channel(const std::string &clients_invited_to_channel);
		Client 				*find_clients_invited_to_channel_by_nickname(std::string &nickname_to_find);
		Client				*find_client(Client &client);
		Client				*find_banned_client(Client &client);
		void				message(Client &client, std::string msg);
};
