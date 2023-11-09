/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 12:30:20 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/09 13:43:20 by abaiao-r         ###   ########.fr       */
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
		int					get_mode() const;
		void				set_mode(int mode);
		unsigned int 		get_channel_limit(void) const;
		void				set_channel_limit(unsigned channel_limit);
		bool 				get_channel_invite_only(void) const;
		void				set_channel_invite_only(bool channel_invite_only);
		std::vector<Client>	&get_clients_in_channel();
		std::vector<Client> &get_clients_banned(void);
		std::vector<Client> &get_clients_operator_channel(void);
		std::vector<Client> &get_clients_invited_to_channel(void);
		void 				add_client_to_clients_operator_vector(Client &client);
		void				add_client(Client &client);
		Client				*find_banned_client(const std::string &client_banned);
		Client				*find_clients_operator_channel(const std::string &clients_operator_channel);
		Client 				*find_clients_invited_to_channel(const std::string &clients_invited_to_channel);
		Client				*find_client(const std::string &nickname);
		void				message(Client &client, std::string msg);
};
