/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abaiao-r <abaiao-r@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 08:29:38 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/30 20:01:19 by abaiao-r         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::connection()
{
	int		count;
	int		client_fd;
	C_IT	match;

	while (_loop_state)
	{
		// for debugging Print the number of clients connected, the number of channels,the clients connected,  the number of clients in each channel, the clients in each channel, the operations in each channel, the invites in each channel, the bans in each channel,
		std::cout << "-----------SERVER INFO-----------\n";
		// number of clients connected to the server
		std::cout << "There are " << _clients.size() << " clients connected\n";
		// clients connected to the server
		if (_clients.size() > 0)
		{
			std::cout << "\nClients connected to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				std::cout << it->get_nickname() << "\n";
			}
			std::cout << "\nClients authenticated to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				if (it->get_authenticated() == true)
					std::cout << it->get_nickname() << "\n";
			}
			std::cout << "\nClients registered to the server:\n";
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			{
				if (it->get_registered())
					std::cout << it->get_nickname() << "\n";
			}
		}
		// number of channels
		std::cout << "\nThere are " << _channels.size() << " channels\n";
		// names of the channels
		if (_channels.size() > 0)
		{
			std::cout << "\nChannels:\n";
			for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				std::cout << it->get_name() << "\n";
			}
		}
		// clients in each channel, operations in each channel, invites in each channel, bans in each channel
		if (_channels.size() > 0)
		{
			for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				std::cout << "\nChannel " << it->get_name() << " has " << it->get_clients_in_channel().size() << " clients\n";
				std::cout << "\nClients in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_in_channel().begin(); it2 != it->get_clients_in_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nOperators in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_operator_channel().begin(); it2 != it->get_clients_operator_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nInvites in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_invited_to_channel().begin(); it2 != it->get_clients_invited_to_channel().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
				std::cout << "\nBans in channel " << it->get_name() << ":\n";
				for (std::vector<Client>::iterator it2 = it->get_clients_banned().begin(); it2 != it->get_clients_banned().end(); it2++)
				{
					std::cout << it2->get_nickname() << "\n";
				}
			}
		}
		std::cout << "---------------------------------\n";
		// end of debugging
		count = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
		if (count == -1)
		{
			if (!_loop_state)
				break;
			throw(std::runtime_error("Error in epoll_wait"));
		}
		for (int i = 0; i < count; i++)
		{
			if (_events[i].data.fd == _server_fd)
			{
				client_connection();
				client_fd = _clients.back().get_client_fd();
				_main_event.events = EPOLLIN;
				_main_event.data.fd = client_fd;
				if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_main_event) == -1)
					throw(std::runtime_error("Error in epoll_ctl"));
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
					disconnect_client(*match);
					for (CH_IT it = _channels.begin(); it != _channels.end(); it++)
						it->check_operator();
					continue;
				}
				client_cmds(*match);
			}
		}
	}
}

void	Server::client_connection()
{
	Client		client;
	socklen_t	len = SOCKLEN;
	int			client_fd = accept(_server_fd, (struct sockaddr*)&client._client_addr, &len);

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

/* parseLoginLine:
 * 1. Extract the command and its arguments from the line.
 * 2. Store the command and its arguments in the map.
 * 3. Remove \r\n from the input.
 */
void Server::parseLoginLine(const std::string &line, std::map<std::string, std::string> &cmds)
{
	std::string cmd = line;
	std::string input;

	// Skip empty lines.
	if (cmd.empty())
		return;

	// Extract the command and its arguments from the line.
	std::stringstream s(cmd);
	s >> cmd;
	std::getline(s, input);
	input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));

	// Store the command and its arguments in the map.
	cmds[cmd] = input;

	// Remove \r\n from the input.
	if (cmds[cmd].find("\r") != std::string::npos)
		cmds[cmd].erase(cmds[cmd].find("\r"), 2);

	// Remove leading and trailing whitespace from the input.
	cmds[cmd].erase(0, cmds[cmd].find_first_not_of(" \t\n\r\f\v"));
	cmds[cmd].erase(cmds[cmd].find_last_not_of(" \t\n\r\f\v") + 1);

	// Debug print: Display the command and its arguments.
	std::cout << YELLOW << "cmd:" << cmd << "| input:" << input << "|"<< RESET << std::endl;
	std::cout << ORANGE << "cmd:" << cmd << " input:" << input << RESET << std::endl;
}


/*login:
** 1. Parse the buffer into a map of commands and their arguments.
** 2. Execute the commands in the map (PASS, USER, NICK if they exist).
** 3. Clear the commands and input from the map.
*/
void Server::login(Client &client, const std::string &buffer)
{
 std::map<std::string, std::string> cmds;
	std::stringstream s(buffer);

	// Iterate through lines in the buffer (split by \n) and parse each command.
	std::string line;
	while (std::getline(s, line, '\n'))
	{
		parseLoginLine(line, cmds);
	}

	// Execute the commands in the map (PASS, USER, NICK if they exist). 
	// First look for PASS, then USER, then NICK.
	if (cmds.find("PASS") != cmds.end())
		cmd_pass(client, cmds["PASS"]);
	if (cmds.find("NICK") != cmds.end())
		cmd_nick(client, cmds["NICK"]);
	if (cmds.find("USER") != cmds.end())
		cmd_user(client, cmds["USER"]);

	// Clear the commands and input from the map.
	cmds.clear();
}


int	Server::client_cmds(Client &client)
{
	char		buffer[BUFFER_READ_SIZE];
	int			fd = client.get_client_fd();
	ssize_t		n = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (n == -1)
	{
		disconnect_client(fd);
		throw(std::runtime_error("Error. Failed in rcv."));
	}
	else if (n == 0) // recv returns 0 if client disconnects
	{
		disconnect_client(fd);
		return 0;
	}
	buffer[n] = 0;
	std::cout << MAGENTA << "Received:" << buffer << RESET << ".\n";
	if (n > 0 && buffer[n - 1] == '\n')
	{
		buffer[n - 1] = 0;
		client.add_to_cmd(static_cast<std::string>(buffer));
		strncpy(buffer, client.get_cmd().c_str(), BUFFER_READ_SIZE);
		client.clear_cmd();
	}
	else if (n > 0 && buffer[n - 1] != '\n')
	{
		client.add_to_cmd(static_cast<std::string>(buffer));
		return 0;
	}
	if (n > 1 && buffer[n - 2] == '\r')
		buffer[n - 2] = 0;

	// if client is not registered, check for login commands
	if (!client.get_authenticated() || !client.get_registered())
	{
		login(client, buffer);
		if (client.get_registered() && client.get_authenticated())
		{
			std::string success = ":localhost " + RPL_WELCOME + " " + client.get_nickname() 
				+ " :Welcome to the Internet Relay Network\r\n";
			sendMessage(client.get_client_fd(), success);
			std::string success2 = ":localhost " + RPL_ISUPPORT + " " + client.get_nickname() 
				+ " :CHANTYPES=#\r\n";
			sendMessage(client.get_client_fd(), success2);
			std::string success3 = ":localhost " + RPL_ISUPPORT + " " + client.get_nickname() 
				+ " :CHANMODES=i,t,k,o,l\r\n";
			sendMessage(client.get_client_fd(), success3);
		}
		return (0);
	}
	return choose_cmd(client, static_cast<std::string>(buffer));
}

int	Server::choose_cmd(Client &client, std::string in)
{
	int					fd = client.get_client_fd();
	std::stringstream	s(in);
	std::string			cmd;
	std::string			input;
	std::string			remaining;

	s >> cmd;
	std::getline(s, input);
	input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
	if (cmd == "PASS")
		cmd_pass(client, input);
	else if (cmd == "USER")
		cmd_user(client, input);
	else if (cmd == "NICK")
		cmd_nick(client, input);
	else if (cmd == "JOIN")
		cmd_join(client, input);
	else if (cmd == "PRIVMSG")
		cmd_privmsg(client, input);
	else if (cmd == "PART")
		cmd_part(client, input);
	else if (cmd == "KICK")
		cmd_kick(client, input);
	else if (cmd == "INVITE")
		cmd_invite(client, input);
	else if (cmd == "TOPIC")
		cmd_topic(client, input);
	else if (cmd == "MODE")
		cmd_mode(client, input);
	else if (cmd == "WHO")
		cmd_who(client, input);
	else if (cmd == "LIST")
		cmd_list(client, input);
	else if (cmd == "EXIT")
		disconnect_client(client.get_client_fd());
	else if (cmd == "HALP")
		_Clippy.cmd_help(client);
	else
	{
		sendMessage(fd, "Error. Command not found\r\n");
		return (-1);
	}
	if (!s.eof())
	{
		std::getline(s, remaining);
		choose_cmd(client, remaining);
	}
	return (0);
}
