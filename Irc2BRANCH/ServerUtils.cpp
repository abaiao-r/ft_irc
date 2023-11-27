/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 08:29:56 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/27 17:28:26 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::pass_validation(std::string check) const
{
	return (check == _password);
}

bool	Server::name_validation(std::string check)
{
	int	len = check.size();

	if (len > MAX_LEN || len < MIN_LEN || check[0] == '#' || check[0] == ':')
		return false;
	for (int i = 0; i < len; i++)
		if (std::isspace(check[i]) || (!std::isalnum(check[i])
			&& (check[i] != '_') && (check[i] != '-') && (check[i] != '[')
			&& (check[i] != ']') && (check[i] != '{') && (check[i] != '}')
			&& (check[i] != '\\') && (check[i] != '|')))
			return false;
	return true;
}

int	Server::nick_validation(std::string check)
{
	if (!name_validation(check))
		return 1;

	C_IT	it = _clients.begin();

	for (; it != _clients.end(); it++)
	{
		if (name_compare(check, it->get_nickname()))
			return 2;
	}
	return 0;
}

int	Server::name_compare(std::string check, std::string comp)
{
	std::cout << "Name to check is: " << check << " comparing against: " << comp << "\n";
	std::transform(check.begin(), check.end(), check.begin(), tolower);
	std::transform(comp.begin(), comp.end(), comp.begin(), tolower);
	std::cout << "After tolower: " << check << " and " << comp << "\n";
	if (check == comp)
		return 1;
	return 0;
}

void	Server::disconnect_client(int fd)
{
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	leave_all_rooms(fd);
	close(fd);
	_clients.erase(match);
}

void	Server::leave_all_rooms(int fd)
{
	CH_IT	it = _channels.begin();
	Client	remove = *find(_clients.begin(), _clients.end(), fd).base();

	for (; it < _channels.end(); it++)
	{
		it->remove_client(remove);
		if (it->get_clients_in_channel().size() == 0)
			_channels.erase(it);
	}
}

void	Server::signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << std::endl;
		_loop_state = 0;
	}
}

/* channel_name_validation: check if channel name is valid
** 1. check if channel name starts with '#'
** 2. check if channel name has whitespaces
** 3. check if channel name is between 2 and 200 characters long
*/
int Server::channel_name_validation(int client_fd, std::string check)
{
	int	len = check.size();
	std::string message;

	if (check[0] != '#')
	{
		message = "Error[JOIN]: Channel name must start with '#'\r\n";
		sendErrorMessage(client_fd, message);
		return (1);
	}
	for (int i = 1; i < len; i++)
	{
		if (!std::isalnum(check[i]) && (check[i] != '_') && (check[i] != '-'))
		{
			message = "Error[JOIN]: Channel name can only contain alphanumeric characters, '_' and '-'\r\n";
			sendErrorMessage(client_fd, message);
			return (1);
		}
	}
	if (len < 2 || len > 200)
	{
		message = "Error[JOIN]: Channel name must be between 2 and 200 characters long\r\n";
		sendErrorMessage(client_fd, message);
		return (1);
	}
	return (0);
}

/* is_client_admin: check if client is administrator
 * 1. Check if client is administrator
 * 2. Send error message if client is not administrator
 */
int Server::is_client_admin(Client &client)
{
	if (!client.get_is_admin())
	{
		std::string error = "Error: " + client.get_nickname() 
			+ " is not administrator\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	return 0;
}

/* findChannel: find channel
 * 1. Find channel by iterating through _channels
 * 2. If channel does not exist, send error message to client and return NULL
 * else return channel
 */
Channel	*Server::findChannel(Client &client, const std::string	&channelName)
{
	std::vector<Channel>::iterator it = find(_channels.begin(), _channels.end(), channelName);

	(void)client;
	if (it == _channels.end())
	{
		return (NULL);
	}
	return &(*it);
}

Client	*Server::find_client(Client &client, const std::string& nickname)
{
	(void)client;

	std::vector<Client>::iterator it = find(_clients.begin(), _clients.end(), nickname);

	if (it == _clients.end())
		return (NULL);
	return &(*it);
}

/* kickClientFromChannel: kick client from channel
 * 1. Remove client from channel->get_clients_in_channel()
 * 2. Send message to client
 */
int Server::kickClientFromChannel(Channel *channel, Client *client, Client *client_to_kick, const std::string &reason)
{
	std::string message;
	
	// Remove client from channel
	channel->remove_client(*(client_to_kick));
	// add client_to_kick to banned list
	channel->add_client_to_banned_vector(*(client_to_kick));
	// if client is in operator_channel, remove from operator_channel
	std::string client_to_kick_nickname = client_to_kick->get_nickname();
	if (channel->find_clients_operator_channel(client_to_kick_nickname))
		channel->remove_client_from_clients_operator_vector(*client_to_kick);
	// Send message to client
	if (reason.empty())
	{
		// Sending message: :asd KICK #tyu andrebaiao :bye
		message = ":" + client->get_nickname() + " KICK " + channel->get_name()
			+ " " + client_to_kick->get_nickname() + " :This is Sparta!\r\n";
		if (sendSuccessMessage(client_to_kick->get_client_fd(), message) == -1)
			return (1);
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		// Sending message: :localhost 353 andrebaiao = #tyu :list of clients_in_channel
		message = ":localhost " + RPL_NAMREPLY + " " + client_to_kick->get_nickname()
			+ " = " + channel->get_name() + " :" + get_users_string(*channel) + "\r\n";
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		//Sending message: :localhost 366 andrebaiao #tyu :End of NAMES list
		message = ":localhost " + RPL_ENDOFNAMES + " " + client->get_nickname() 
			+ " " + channel->get_name() + " :End of NAMES list\r\n";
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		return (0);
	}
	else
	{
		// Sending message: :asd KICK #tyu andrebaiao :bye
		message = ":" + client->get_nickname() + " KICK " + channel->get_name()
			+ " " + client_to_kick->get_nickname() + " :" + reason + "\r\n";
		if (sendSuccessMessage(client_to_kick->get_client_fd(), message) == -1)
			return (1);
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		// Sending message: :localhost 353 andrebaiao = #tyu :list of clients_in_channel
		message = ":localhost " + RPL_NAMREPLY + " " + client_to_kick->get_nickname()
			+ " = " + channel->get_name() + " :" + get_users_string(*channel) + "\r\n";
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		//Sending message: :localhost 366 andrebaiao #tyu :End of NAMES list
		message = ":localhost " + RPL_ENDOFNAMES + " " + client->get_nickname() 
			+ " " + channel->get_name() + " :End of NAMES list\r\n";
		if (sendSuccessMessage(client->get_client_fd(), message) == -1)
			return (1);
		return (0);
		
	}
	// Send message to client send success message to client
	if (sendSuccessMessage(client->get_client_fd(), message) == -1)
		return (1);
	return (0);
}

int	Server::password_checker(std::string password)
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
/* password_checker: check if password is valid
 * 1. Check if password is between 3 and 12 characters
 * 2. Check if password contains only non-printable characters
 */
int	Server::password_checker(std::string password, int fd)
{
	// Check if password is between 3 and 12 characters
	if (password.length() < 3 || password.length() > 12)
	{
		std::string error = "Error. Password must be between 3 and 12 characters\r\n";
		sendErrorMessage(fd, error);
		return (1);
	}
	// Check if password contains non-printable characters
	for (size_t i = 0; i < password.length(); i++)
	{
		if (!isprint(password[i]))
		{
			std::string error = "Error. Password must not contain non-printable characters\r\n";
			sendErrorMessage(fd, error);
			return (1);
		}
	}
	return (0);
}

std::string	Server::get_users_string(Channel &channel)
{
	std::string			ret;
	std::vector<Client>	list = channel.get_clients_in_channel();
	C_IT	it = list.begin();

	for (; it != list.end(); it++)
	{
		std::string	nickname = it->get_nickname();
		if (!channel.find_clients_operator_channel(nickname))
			ret += "%" + it->get_nickname();
		else
			ret += "@" + it->get_nickname();
		if (it + 1 != list.end())
			ret += " ";
	}
	if (ret.empty())
		ret = " ";
	return ret;
}

void	Server::join_messages(Client &client, Channel &channel)
{
	std::string	message;
	std::string	name = channel.get_name();
	int			fd = client.get_client_fd();

	message = ":" + client.get_nickname() + "!" + client.get_username() + "@" + "localhost" + " JOIN " + name + "\r\n";
	sendSuccessMessage(fd, message);
	message = ":localhost " + RPL_TOPIC + " " + client.get_nickname() + " " + name + " " + channel.get_topic() + "\r\n";
	sendSuccessMessage(fd, message);
	message = ":localhost " + RPL_NAMREPLY + " " + client.get_nickname() + " = " + name + " :" + get_users_string(channel) + "\r\n";
	channel.info_message(message);
	message = ":localhost " + RPL_ENDOFNAMES + " " + client.get_nickname() + " " + name + " :End of NAMES list\r\n";
	channel.info_message(message);
}
