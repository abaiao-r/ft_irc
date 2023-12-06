/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 08:59:41 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 19:54:03 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConnection.hpp"

int	ServerConnection::_loop_state = 1;

ServerConnection::ServerConnection()
{
	std::signal(SIGINT, signal_handler);
	//std::cout << CYAN << "Default constructor ServerConnection called" << RESET 
	//	<< std::endl;
}

ServerConnection::~ServerConnection()
{
	//std::cout << RED << "Destructor ServerConnection called" << RESET 
	//	<< std::endl;
	close(_epoll_fd);
	std::signal(SIGINT, SIG_DFL);
}

ServerConnection::ServerConnection(const ServerConnection &copy)
	:ServerCommands()
{
	//std::cout << CYAN << "Copy constructor Server called" << RESET 
	//		<< std::endl;
	*this = copy;
}

ServerConnection &ServerConnection::operator=(const ServerConnection &copy)
{
	//std::cout << YELLOW << "Assignment operator Server called" << RESET 
	//	<< std::endl;
	(void)copy;
	return *this;
}

void	ServerConnection::create_epoll(int server_fd)
{
	memset(&_events, 0, sizeof(_events));
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw(std::runtime_error("Error when creating epoll"));
	memset(&_main_event, 0, sizeof(_main_event));
	add_epoll(server_fd);
}

void	ServerConnection::add_epoll(int fd)
{
	_main_event.events = EPOLLIN;
	_main_event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_main_event) == -1)
		throw(std::runtime_error("Error in epoll_ctl"));
}

void	ServerConnection::connection(int server_fd)
{
	int				count;
	int				client_fd;
	C_IT			match;

	while (_loop_state)
	{
		info_print();
		count = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
		if (count == -1)
		{
			if (!_loop_state)
				break;
			throw(std::runtime_error("Error in epoll_wait"));
		}
		for (int i = 0; i < count; i++)
		{
			if (_events[i].data.fd == server_fd)
			{
				client_connection(server_fd);
				client_fd = _clients.back().get_client_fd();
				add_epoll(client_fd);
			}
			else if ((_events[i].events & EPOLLERR)
				|| (_events[i].events & EPOLLHUP)
				|| (!(_events[i].events & EPOLLIN)))
				disconnect_client(_events[i].data.fd);
			else
			{
				client_fd = _events[i].data.fd;
				match = std::find(_clients.begin(), _clients.end(), client_fd);
				if (match == _clients.end())
				{
					std::cout << "Error. Could not find client\n";
					disconnect_client(match->get_client_fd());
					for (CH_IT it = _channels.begin(); it != _channels.end(); it++)
						it->check_operator();
					continue;
				}
				client_cmds(*match);
			}
		}
	}
}

void	ServerConnection::client_connection(int server_fd)
{
	Client		client;
	socklen_t	len = SOCKLEN;
	int			client_fd = accept(server_fd, (struct sockaddr*)&client._client_addr, &len);

	if (client_fd == -1)
		throw(std::runtime_error("Error. Could not accept client connection"));
	if (unblock_socket(client_fd))
	{
		close(client_fd);
		throw(std::runtime_error("Error when setting flags"));
	}
	client.set_client_fd(client_fd);
	_clients.push_back(client);
	_Clippy.greeting(client);
}

/* disconnect_client: remove client from server
** 1. find client in _clients vector
** 2. remove client from all channels
** 3. close client's fd
** 4. remove client from _clients vector
*/
void	ServerConnection::disconnect_client(int fd)
{
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	leave_all_rooms(*match);
	close(fd);
	_clients.erase(match);
}


/* disconnect_client: remove client from server
** 1. find client in _clients vector
** 2. remove client from all channels
** 3. close client's fd
** 4. remove client from _clients vector
*/
void	ServerConnection::disconnect_client(Client &client)
{
	int		fd = client.get_client_fd();
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	leave_all_rooms(client);
	close(fd);
	_clients.erase(match);
}

/* leave_all_rooms: remove client from all channels
** 1. iterate through all channels
** 2. remove client from channel
** 3. remove client from channel's clients operator vector
** 4. check if channel is empty
** 5. if channel is empty, remove channel
*/
void	ServerConnection::leave_all_rooms(Client &client)
{
	CH_IT	it = _channels.begin();
	Client	remove = *find(_clients.begin(), _clients.end(), client.get_client_fd()).base();

	for (; it < _channels.end(); it++)
	{
		it->remove_client(remove);
		it->remove_client_from_clients_operator_vector(remove);
		it->check_operator();
		sendChannelUserListMessage(it.base(), client.get_nickname());
		if (it->get_clients_in_channel().size() == 0)
			_channels.erase(it);
	}
}

/* sendChannelUserListMessage: send channel user list message
 * 1. Send message to client with list of users in channel
 * 2. Send message to client with end of names list
 */
void ServerConnection::sendChannelUserListMessage(Channel *channel, const std::string &argument)
{
	std::string userListMessage = ":localhost " + RPL_NAMREPLY + " " + argument
		+ " = " + channel->get_name() + " :" + get_users_string(*channel) 
		+ "\r\n";
	channel->info_message(userListMessage);

	std::string endOfNamesMessage = ":localhost " + RPL_ENDOFNAMES + " " 
		+ argument + " " + channel->get_name() + " :End of NAMES list\r\n";
	channel->info_message(endOfNamesMessage);
}

/* get_users_string: get users string
 * 1. Get list of clients in channel
 * 2. Iterate through list and add clients to string
 * 3. Return string
 */
std::string	ServerConnection::get_users_string(Channel &channel)
{
	std::string			ret;
	std::vector<Client>	list = channel.get_clients_in_channel();
	C_IT	it = list.begin();

	for (; it != list.end(); it++)
	{
		std::string	nickname = it->get_nickname();
		if (!channel.find_client(nickname, "operators"))
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

/* signal_handler: handle SIGINT signal
** 1. set loop state to 0
*/
void	ServerConnection::signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << std::endl;
		_loop_state = 0;
	}
}

void	ServerConnection::info_print()
{
	// for debugging Print the number of clients connected, the number of channels,the clients connected,  the number of clients in each channel, the clients in each channel, the operations in each channel, the invites in each channel, the bans in each channel,
	std::cout << "-----------SERVER INFO-----------\n";
	// number of clients connected to the server
	std::cout << "There are " << _clients.size() << " clients connected\n";
	// clients connected to the server
	if (_clients.size() > 0)
	{
		std::cout << "\nClients connected to the server:\n";
		for (C_IT it = _clients.begin(); it != _clients.end(); it++)
			std::cout << it->get_nickname() << "\n";
		std::cout << "\nClients authenticated to the server:\n";
		for (C_IT it = _clients.begin(); it != _clients.end(); it++)
			if (it->get_authenticated() == true)
				std::cout << it->get_nickname() << "\n";
		std::cout << "\nClients registered to the server:\n";
		for (C_IT it = _clients.begin(); it != _clients.end(); it++)
			if (it->get_registered())
				std::cout << it->get_nickname() << "\n";
	}
	// number of channels
	std::cout << "\nThere are " << _channels.size() << " channels\n";
	// names of the channels
	if (_channels.size() > 0)
	{
		std::cout << "\nChannels:\n";
		for (CH_IT it = _channels.begin(); it != _channels.end(); it++)
			std::cout << it->get_name() << "\n";
	}
	// clients in each channel, operations in each channel, invites in each channel, bans in each channel
	if (_channels.size() > 0)
	{
		for (CH_IT it = _channels.begin(); it != _channels.end(); it++)
		{
			std::cout << "\nChannel " << it->get_name() << " has " << it->get_clients_in_channel().size() << " clients\n";
			std::cout << "\nClients in channel " << it->get_name() << ":\n";
			for (C_IT it2 = it->get_clients_in_channel().begin(); it2 != it->get_clients_in_channel().end(); it2++)
				std::cout << it2->get_nickname() << "\n";
			std::cout << "\nOperators in channel " << it->get_name() << ":\n";
			for (C_IT it2 = it->get_clients_operator_channel().begin(); it2 != it->get_clients_operator_channel().end(); it2++)
				std::cout << it2->get_nickname() << "\n";
			std::cout << "\nInvites in channel " << it->get_name() << ":\n";
			for (C_IT it2 = it->get_clients_invited_to_channel().begin(); it2 != it->get_clients_invited_to_channel().end(); it2++)
				std::cout << it2->get_nickname() << "\n";
			std::cout << "\nBans in channel " << it->get_name() << ":\n";
			for (C_IT it2 = it->get_clients_banned().begin(); it2 != it->get_clients_banned().end(); it2++)
				std::cout << it2->get_nickname() << "\n";
		}
	}
	std::cout << "---------------------------------\n";
}
