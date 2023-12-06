/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 10:45:16 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 18:17:37 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCommands.hpp"

ServerCommands::ServerCommands()
	: _Clippy("Clippy")
{
	std::cout << CYAN << "Default constructor ServerCommands called" << RESET 
		<< std::endl;
}

ServerCommands::~ServerCommands()
{
	std::cout << RED << "Destructor ServerCommands called" << RESET 
		<< std::endl;
	C_IT end = _clients.end();
	for (C_IT it = _clients.begin(); it != end; it++)
		close(it->get_client_fd());
}

ServerCommands::ServerCommands(const ServerCommands &copy)
	: _Clippy("Clippy")
{
	std::cout << CYAN << "Copy constructor ServerCommands called" << RESET 
		<< std::endl;
	*this = copy;
}

ServerCommands &ServerCommands::operator=(const ServerCommands &copy)
{
	(void)copy;
	std::cout << CYAN << "Assignment operator ServerCommands called" << RESET 
		<< std::endl;
	return (*this);
}

/* parseLoginLine:
 * 1. Extract the command and its arguments from the line.
 * 2. Store the command and its arguments in the map.
 * 3. Remove \r\n from the input.
 */
void ServerCommands::parseLoginLine(const std::string &line, std::map<std::string, std::string> &cmds)
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
void ServerCommands::login(Client &client, const std::string &buffer)
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
	if (cmds.find("PASS") == cmds.end()
		&& cmds.find("USER") == cmds.end()
		&& cmds.find("NICK") == cmds.end()
		&& cmds.find("CAP") == cmds.end())
		sendMessage(client.get_client_fd(), "ERROR :Login before using commands\r\n");
	{
		if (client.get_registered())
			client.set_authenticated(true);
	}

	// Clear the commands and input from the map.
	cmds.clear();
}


int	ServerCommands::client_cmds(Client &client)
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

int	ServerCommands::choose_cmd(Client &client, std::string in)
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

/* parseArgsListCommand: /list #channel1,#channel2,#channel3
** 1. parse input string into vector of strings by comma
** 2. return vector of strings
*/
std::vector<std::string> ServerCommands::parseArgsListCommand(std::string input)
{
	std::vector<std::string> channels_to_list;
	std::stringstream ss(input);
	std::string token;
	
	while (std::getline(ss, token, ','))
	{
		if (token[0] == '#')
			channels_to_list.push_back(token);
	}
	return (channels_to_list);
}

/* cmdListWithArg: /list #channel1,#channel2,#channel3
** 1. parse input string into vector of strings by comma
** 3. if it exists, send RPL_LIST message
** 4. if it doesn't exist, send ERR_NOSUCHCHANNEL message
*/
int ServerCommands::cmdListWithArg(Client &client, std::string input)
{	
	std::vector<std::string> channels_to_list = parseArgsListCommand(input);
	
	for (std::vector<std::string>::iterator it = channels_to_list.begin(); 
		it != channels_to_list.end(); it++)
	{	
		Channel *channel = findChannel(client, *it);
		if (channel != NULL)
		{
			int num_clients_in_channel = channel->get_clients_in_channel().size();
			// convert int to string
			std::string num_clients_in_channel_str 
				= static_cast<std::ostringstream*>(&(std::ostringstream() 
				<< num_clients_in_channel))->str();
			std::string msg2 = ":localhost " + RPL_LIST + " " 
				+ client.get_nickname() + " " + channel->get_name() 
				+ " " +	num_clients_in_channel_str + " :"
				+ channel->get_topic() + "\r\n";
			sendMessage(client.get_client_fd(), msg2);
		}
		else
		{
			std::string msg2 = ":localhost " + ERR_NOSUCHCHANNEL + " " 
				+ client.get_nickname() + " " + *it + " :No such channel\r\n";
			sendMessage(client.get_client_fd(), msg2);
		}
	}
	return (0);
}

/* cmdListNoArgs: /list
** 1. send RPL_LIST message for each channel
*/
int ServerCommands::cmdListNoArgs(Client &client)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); 
		it != _channels.end(); it++)
	{
		int num_clients_in_channel = it->get_clients_in_channel().size();
		// convert int to string
		std::string num_clients_in_channel_str 
			= static_cast<std::ostringstream*>(&(std::ostringstream() 
			<< num_clients_in_channel))->str();
		std::string msg2 = ":localhost " + RPL_LIST + " " 
			+ client.get_nickname() + " " + it->get_name() 
			+ " " +	num_clients_in_channel_str + " :"
			+ it->get_topic() + "\r\n";
		sendMessage(client.get_client_fd(), msg2);
	}
	return (0);
}

/*cmd_list: /list #channel1,#channel2,#channel3 or /list
** 1. message RPL_LISTSTART
** 2. if input is empty, list all channels
** 3. else, list channels that match input
** 4. message RPL_LISTEND
*/
int ServerCommands::cmd_list(Client &client, std::string input)
{
	std::string msg = ":localhost " + RPL_LISTSTART + " " 
		+ client.get_nickname() + " :Channel list\r\n";
	sendMessage(client.get_client_fd(), msg);
	// if input is empty, list all channels
	if (input.empty() && _channels.size() > 0)
		cmdListNoArgs(client);
	// else, list channels that match input
	else if (!input.empty() && _channels.size() > 0)
		cmdListWithArg(client, input);
	std::string msg2 = ":localhost " + RPL_LISTEND + " " 
		+ client.get_nickname() + " :End of /LIST\r\n";
	sendMessage(client.get_client_fd(), msg2);
	return (0);
}

/* sendWhoReplyMessages: send RPL_WHOREPLY message for each client in channel
** 1. iterate through clients in channel
** 2. send RPL_WHOREPLY message for each client
** 3. send RPL_ENDOFWHO message
*/
void ServerCommands::sendWhoReplyMessages(Client &client, Channel &channel)
{
	C_IT it = channel.get_clients_in_channel().begin();
	C_IT end = channel.get_clients_in_channel().end();
	std::string msg;

	for (; it != end; it++)
	{
		std::string nick = it->get_nickname();
		Client *curr = channel.find_client(nick, "operators");
		std::string opr = curr ? " H" : " G";
		std::string status = curr ? "@" : "+";
		msg = ":localhost " + RPL_WHOREPLY + " " + it->get_nickname() + " " +
			  channel.get_name() + " ft_irc " + client.get_nickname() + opr +
			  status + " :1 " + it->get_username() + "\r\n";
		sendMessage(client.get_client_fd(), msg);
	}

	msg = ":localhost " + RPL_ENDOFWHO + " " + client.get_nickname() + " " +
		  channel.get_name() + " :End of WHO list\r\n";
	sendMessage(client.get_client_fd(), msg);
}

/* cmd_who: /who #channel1
** 1. if input is empty,send error message
** 2. if channel doesn't exist, send ERR_NOSUCHCHANNEL message
** 3. else, send RPL_WHOREPLY message for each client in channel
** 4. send RPL_ENDOFWHO message
*/
int ServerCommands::cmd_who(Client &client, std::string input)
{
	if (input.empty())
	{
		std::string error = "Error[MODE]: No argument provided\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}

	Channel *channel = findChannel(client, input);

	if (!channel)
	{
		std::string error = ":localhost " + ERR_NOSUCHCHANNEL +
			" : Error[MODE]: Channel " + input + " does not exist\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	sendWhoReplyMessages(client, *channel);
	return (0);
}

/* handleModeMinusL: MODE <channel> -l
** 1. if channel limit is already 0 (unlimited), send error message
** 2. else, set channel limit to 0 (unlimited)
*/
int ServerCommands::handleModeMinusL(Channel *channel, int fd)
{
	std::string message;
	// Expecting format: MODE <channel> -l
	// check if channel limit is already 0
	if (channel->get_channel_limit() == 0)
	{
		message = "Error[MODE -l]: Channel " + channel->get_name() 
			+ " is not limited\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// set channel limit
	channel->set_channel_limit(0);
	message = "Success[MODE -l]: Channel " + channel->get_name() 
		+ " is no longer limited\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModePlusL: MODE <channel> +l <limit>
** 1. if channel is already limited, send error message
** 2. if no limit is specified, send error message
** 3. if limit is not a number or not in integer range, send error message
** 4. else, set channel limit to argument converted to int
*/
int ServerCommands::handleModePlusL(Channel *channel, std::string argument, int fd)
{
	// Expecting format: MODE <channel> +l <limit>
	// check if channel is already limited
	if (channel->get_channel_limit() != 0)
	{
		std::string message = "Error[MODE +l]: Channel " + channel->get_name() 
			+ " is already limited\r\n";
		sendMessage(fd, message);
		return (1);
	}
	if (argument.empty())
	{
		std::string message = "Error[MODE +l]: No limit specified\r\n";
		sendMessage(fd, message);
		return (1);
	}
	if (isStrOnlyDigits(argument) == false)
	{
		std::string message = "Error[MODE +l]: " + argument 
			+ " is not a valid number\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// set channel limit argument converted to int
	try
	{
		channel->set_channel_limit(safe_atoi(argument.c_str()));
	}
	catch (std::exception &e)
	{
		std::string message = "Error[MODE +l]: " + argument 
			+ " is a value out of integer range\r\n";
		sendMessage(fd, message);
		return (1);
	}
	std::string message = "Success[MODE +l]: Channel " 
		+ channel->get_name() + " is now limited to " + argument + " users\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModeMinusT: MODE <channel> -t
** 1. if channel is already topic protected, send error message
** 2. else, set channel to topic protected
*/
int ServerCommands::handleModeMinusT(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> -t
	// check if channel is already topic protected
	if (channel->get_topic_mode() == false)
	{
		message = "Error[MODE -t]: Channel " + channel->get_name() 
			+ " is not topic protected\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// set channel to topic protected
	channel->set_topic_mode(false);
	message = "Success[MODE -t]: Channel " + channel->get_name() 
		+ " is no longer topic protected\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModePlusT: MODE <channel> +t
** 1. if channel is already topic protected, send error message
** 2. else, set channel to topic protected
*/
int ServerCommands::handleModePlusT(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +t
	// check if channel is already topic protected
	if (channel->get_topic_mode() == true)
	{
		message = "Error[MODE +t]: Channel " + channel->get_name() 
			+ " is already topic protected\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// set channel to topic protected
	channel->set_topic_mode(true);
	message = "Success[MODE +t]: Channel " + channel->get_name() 
		+ " is now topic protected\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModeMinusI: MODE <channel> -i
** 1. if channel is already invite only, send error message
** 2. else, set channel to invite only
*/
int ServerCommands::handleModeMinusI(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> -i
	// check if channel is already invite only
	if (channel->get_channel_invite_only() == false)
	{
		message = "Error[MODE -i]: Channel " + channel->get_name() 
			+ " is not invite only\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// set channel to invite only
	channel->set_channel_invite_only(false);
	message = "Success[MODE -i]: Channel " + channel->get_name() 
		+ " is no longer invite only\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModePlusI: MODE <channel> +i
** 1. if channel is already invite only, send error message
** 2. else, set channel to invite only
*/
int ServerCommands::handleModePlusI(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +i
	// check if channel is already invite only
	if (channel->get_channel_invite_only() == true)
	{
		message = "Error[MODE +i]: Channel " + channel->get_name() 
			+ " is already invite only\r\n";
		sendMessage(fd, message);
		return (1);
	}

	// set channel to invite only
	channel->set_channel_invite_only(true);
	message = "Success[MODE +i]: Channel " + channel->get_name() 
		+ " is now invite only\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModeMinusK: MODE <channel> -k
** 1. if channel doesn't have a password, send error message
** 2. else, remove password of channel
*/
int ServerCommands::handleModeMinusK(Channel *channel, int fd)
{
	std::string message;
	
	// Expecting format: MODE <channel> -k
	// check if channel is already password protected
	if (channel->get_password().empty())
	{
		message = "Error[MODE -k]: Channel " + channel->get_name() 
			+ " is not password protected\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// remove password of channel
	channel->set_password("");
	message = "Success[MODE -k]: Channel " + channel->get_name() 
		+ " is no longer password protected\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModePlusK: MODE <channel> +k <password>
** 1. if channel is already password protected, send error message
** 2. if password is invalid, send error message
** 3. else, change password of channel
*/
int ServerCommands::handleModePlusK(Channel *channel, std::string argument, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +k <password>
	// check if channel is already password protected
	if (!channel->get_password().empty())
	{
		message = "Error[MODE +k]: Channel " + channel->get_name() 
			+ " is already password protected\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// check if password is valid
	if (password_checker(argument, fd))
		return (1);
	// change password of channel
	channel->set_password(argument);
	message = ":localhost " + RPL_CHANNELMODEIS + " " 
		+ channel->get_name() + " +k is now password protected\r\n";
	sendMessage(fd, message);
	return (0);
}

/* handleModeMinusO: MODE <channel> -o <nickname>
** 1. if client is not in channel, send error message
** 2. if client is not an operator, send error message
** 3. if client is the last operator, send error message
** 4. else, remove client from operators vector
*/
int ServerCommands::handleModeMinusO(Client &client, Channel *channel, std::string argument, int fd)
{
	std::string message;
	
	// Expecting format: MODE <channel> -o <nickname>
	// look if argument(client to become operator) is in the channel
	if (!channel->find_client(argument, "clients"))
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE -o]: " 
			+ argument + " is not in the channel " + channel->get_name() + "\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// look for nickname in clients operator_channel
	if (!channel->find_client(argument, "operators"))
	{
		message = "Error[MODE -o]: " + argument 
			+ " is not an operator in channel " + channel->get_name() + "\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// if client is the last operator in the channel, send error message
	if (channel->get_clients_operator_channel().size() == 1)
	{
		message = "Error[MODE -o]: " + argument 
			+ " is the last operator in channel " + channel->get_name() + "\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// remove nickname from clients operator_channel
	// find client by nickname
	Client *client_to_remove = find_client(client, argument);
	if (!client_to_remove)
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE -o]: Client " 
			+ argument + " does not exist\r\n";
		sendMessage(fd, message);
		return (1);
	}
	channel->remove_client_from_clients_operator_vector(*client_to_remove);
	message = "Success[MODE -o]: " + argument 
		+ " is no longer an operator in channel " + channel->get_name() + "\r\n";
	sendMessage(fd, message);
	sendMessage(client_to_remove->get_client_fd(), message);
	sendChannelUserListMessage(channel, argument);
	return (0);
}

/* handleModePlusO: MODE <channel> +o <nickname>
** 1. if client is not in channel, send error message
** 2. if client is already an operator, send error message
** 3. else, add client to operators vector
*/
int ServerCommands::handleModePlusO(Client &client, Channel *channel, 
	std::string argument, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +o <nickname>
	// look if argument(client to become operator) is in the channel
	if (!channel->find_client(argument, "clients"))
	{
		message = ":localhost " + ERR_NOSUCHNICK 
			+ " : Error[MODE +o]: " + argument + " is not in the channel " 
			+ channel->get_name() + "\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// look for nickname in clients operator_channel
	if (channel->find_client(argument, "operators"))
	{
		message = "Error[MODE +o]: " + argument 
			+ " is already an admin in channel " + channel->get_name() + "\r\n";
		sendMessage(fd, message);
		return (1);
	}
	// add nickname to clients operator_channel
	// find client by nickname
	Client *client_to_add = find_client(client, argument);
	if (!client_to_add)
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE +o]: Client " 
			+ argument + " does not exist\r\n";
		sendMessage(fd, message);
		return (1);
	}
	channel->add_client_to_clients_operator_vector(*client_to_add);
	// send success message to client and channel with reply 381
	message =  ":localhost " + RPL_YOUREOPER + " " + argument + " : [MODE +o]" 
		+ argument + " is now an operator in channel " + channel->get_name() 
		+ "\r\n";
	sendMessage(fd, message);
	sendMessage(client_to_add->get_client_fd(), message);
	// send channel user list to all clients in channel
	sendChannelUserListMessage(channel, argument);
	return (0);
}

/* handleMode: MODE <channel> [+|-][o|k|i|t|l] <argument>
** 1. call the appropriate function based on the mode
** 2. return 2 if mode is invalid
*/
int  ServerCommands::handleMode(Client &client, Channel *channel, 
	const std::string &mode, const std::string &argument, int fd)
{
	// Implement the logic for each mode
	if (mode == "+o")
		return (handleModePlusO(client, channel, argument, fd));
	else if (mode == "-o")
		return (handleModeMinusO(client, channel, argument, fd));
	else if (mode == "+k")
		return (handleModePlusK(channel, argument, fd));
	else if (mode == "-k")
		return (handleModeMinusK(channel, fd));
	else if (mode == "+i")
		return (handleModePlusI(channel, fd));
	else if (mode == "-i")
		return (handleModeMinusI(channel, fd));
	else if (mode == "+t")
		return (handleModePlusT(channel, fd));
	else if (mode == "-t")
		return (handleModeMinusT(channel, fd));
	else if (mode == "+l")
		return (handleModePlusL(channel, argument, fd));
	else if (mode == "-l")
		return (handleModeMinusL(channel, fd));
	else
		return (2);
}

/* sendOperatorPrivilegeError: send ERR_CHANOPRIVSNEEDED message
** 1. send ERR_CHANOPRIVSNEEDED message
*/
int ServerCommands::sendOperatorPrivilegeError(int fd, const std::string &channel)
{
	std::string message = ":localhost " + ERR_CHANOPRIVSNEEDED +
		" : Error[MODE]: You are not an operator in channel " + channel + "\r\n";
	return (sendMessage(fd, message));
}

/* sendChannelModeInfo: send RPL_CHANNELMODEIS message
** 1. send RPL_CHANNELMODEIS message
*/
int ServerCommands::sendChannelModeInfo(int fd, Channel *channel, 
	const std::string &channelName)
{
	std::string message = ":localhost " + RPL_CHANNELMODEIS + " " + channelName 
		+ " " + channelName + ": " + channel->get_mode() + "\r\n";
	return (sendMessage(fd, message));
}

int ServerCommands::sendChannelNotFoundError(int fd, const std::string &channel)
{
	std::string message = ":localhost " + ERR_NOSUCHCHANNEL 
		+ " : Error[MODE]: Channel " + channel + " does not exist\r\n";
	return (sendMessage(fd, message));
}

// Debug delete later
void ServerCommands::printDebugInfo(const std::string &channel, const std::string &mode, const std::string &argument)
{
	std::cout << BOLDYELLOW << "cmd_mode" << std::endl;
	std::cout << "channel_to_find: " << channel << "|" << std::endl;
	std::cout << "mode: " << mode << "|" << std::endl;
	std::cout << "argument: " << argument << "|" << std::endl;
	std::cout << RESET << std::endl;
}

/* cmd_mode: /mode #channel +o <nickname>
** 1. parse input string into channel, mode and argument
** 2. if mode is empty, send RPL_CHANNELMODEIS message
** 3. if client is not in the vector of clients in channel, send error message
** 4. if client is not an operator, send error message
** 5. if mode is invalid, send error message
** 6. else, call the appropriate function based on the mode
*/
int ServerCommands::cmd_mode(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string mode;
	std::string argument;
	std::string message;
	int fd = client.get_client_fd();

	// Parse input
	iss >> channel_to_find >> mode >> argument;

	// Debug
	printDebugInfo(channel_to_find, mode, argument);
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
		return (sendChannelNotFoundError(fd, channel_to_find));

	if (mode.empty())
		return (sendChannelModeInfo(fd, channel, channel_to_find));
	// Find if Client is in the vector of clients operator_channel
	std::string nickname = client.get_nickname();
	if (!channel->find_client(nickname, "operators"))
		return (sendOperatorPrivilegeError(fd, channel_to_find));
	// Handle mode based on the mode string
	if (handleMode(client, channel, mode, argument, fd) == 2)
	{
		std::string error = ":localhost " + ERR_UNKNOWNMODE 
			+ " : Error[MODE]: Unknown mode " + mode 
			+ " Usage: MODE <channel> [+|-][o|k|i|t|l] <argument>\r\n";
		sendMessage(fd, error);
		return (1);
	}
	return (0);
}

void	ServerCommands::cmd_pass(Client &client, std::string input)
{
	int	fd = client.get_client_fd();

	if (client.get_authenticated())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[PASS]: You are already authenticated\r\n";
		sendMessage(fd, error);
		return;
	}
	if (pass_validation(input))
	{
		std::string success = "Success[PASS]: You are now authenticated\r\n";
		sendMessage(fd, success);
		client.set_authenticated(true);
		return;
	}
	if (client.pass_counter(0, 0) == 2)
	{
		std::string error = ":localhost " + ERR_PASSWDMISMATCH 
			+ " : Error[PASS]: Too many wrong attempts, disconnecting\r\n";
		sendMessage(fd, error);
		disconnect_client(fd);
	}
	std::string error = ":localhost " + ERR_PASSWDMISMATCH 
		+ " : Error[PASS]: Wrong password\r\n";
	sendMessage(fd, error);
	client.pass_counter(1, 0);
}

void	ServerCommands::cmd_user(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	std::stringstream	ss(input);
	std::string			username;

	ss >> username;
	if (client.get_registered())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[USER]: You are already registered\r\n";
		sendMessage(fd, error);
		return;
	}
	if (!client.get_authenticated())
	{
		std::string error = "Error[USER]: You are not authenticated. Input password first\r\n";
		sendMessage(fd, error);
		return;
	}
	if (!client.get_username().empty())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[USER]: You already have a username\r\n";
		sendMessage(fd, error);
		return;
	}
	if (name_validation(username))
	{
		client.set_username(username);
		std::string success = "Success[USER]: Username set to " 
			+ username + "\r\n";
		sendMessage(fd, success);
		if (!client.get_nickname().empty())
		{
			success = "Success[USER]: You are now registered\r\n";
			sendMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[USER]: Username can't have spaces or symbols (except [-_[]{}\\|]), can't start with '#' or ':' and must be between 3 and 10 characters long\r\n";
		sendMessage(fd, error);
	}
}

void	ServerCommands::cmd_nick(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	int	test;

	if (client.get_registered())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[NICK]: You are already registered\r\n";
		sendMessage(fd, error);
		return;
	}
	if (!client.get_authenticated())
	{
		std::string error = "Error[NICK]: You are not authenticated. Input password first\r\n";
		sendMessage(fd, error);
		return;
	}
	if (!client.get_nickname().empty())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[NICK]: You already have a nickname\r\n";
		sendMessage(fd, error);
		return;
	}
	test = nick_validation(input);
	if (!test)
	{
		client.set_nickname(input);
		std::string success = "Success[NICK]: Nickname set to " + input + "\r\n";
		sendMessage(fd, success);
		if (!client.get_username().empty())
		{
			success = "Success[NICK]: You are now registered\r\n";
			sendMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else if (test == 1)
	{
		std::string error = ":localhost " + ERR_ERRONEUSNICKNAME 
			+ " : Error[NICK]: Nickname can't have spaces or symbols (except [-_[]{}\\|]), can't start with '#' or ':' and must be between 3 and 10 characters long\r\n";
		sendMessage(fd, error);
	}
	else
	{
		std::string error = ":localhost " + ERR_NICKNAMEINUSE 
			+ " : Error[NICK]: Nickname already in use, choose another\r\n";
		sendMessage(fd, error);
	}
}

/* checkBanned
 * 1. Checks if the client is banned from the channel
 * 2. If it is, sends an error message and returns true
 * 3. If it isn't, returns false
 */
bool ServerCommands::checkBanned(Client &client, const std::string &input_channel_name, 
	CH_IT &it, int fd)
{
	if (it->find_client(client.get_nickname(), "banned"))
	{
		std::string message = ":localhost " + ERR_BANNEDFROMCHAN +
			" : Error[JOIN]: You (" + client.get_nickname()
			+ ") are banned from channel " + input_channel_name + "\r\n";
		sendMessage(fd, message);
		return (true);
	}
	return false;
}

/* checkPassword
 * 1. Checks if the channel has a password
 * 2. if it does, checks if the password is correct
 * Returns true if password is correct or if there is no password, 
 * false otherwise
 */
bool ServerCommands::checkPassword(const std::string &input_channel_name,
	const std::string &input_password, CH_IT &it, int fd)
{
	if (!it->get_password().empty())
	{
		if (it->get_password() != input_password)
		{
			// numerric reply
			std::string message = ":localhost " + ERR_BADCHANNELKEY +
				" : Error[JOIN]: Wrong password for channel " 
				+ input_channel_name + "\r\n";
			sendMessage(fd, message);
			return (false);
		}
	}
	return (true);
}

/* checkInviteOnly
 * 1. Checks if the channel is invite only
 * 2. If it is, checks if the client is invited to the channel
 * 3. If it isn't, sends an error message and returns false
 * 4. If it is, returns true
 */
bool ServerCommands::checkInviteOnly(Client &client, 
	const std::string &input_channel_name, CH_IT &it, int fd)
{
	if (it->get_channel_invite_only())
	{
		if (find(it->get_clients_invited_to_channel().begin(),
			it->get_clients_invited_to_channel().end(),
			client.get_client_fd()) == it->get_clients_invited_to_channel().end())
		{
			std::string message = ":localhost " + ERR_INVITEONLYCHAN +
				" : Error[JOIN]: Channel " + input_channel_name 
				+ " is invite only\r\n";
			sendMessage(fd, message);
			return (false);
		}
	}
	return (true);
}

/* checkChannelFull
 * 1. Checks if the channel is full
 * 2. If it is, sends an error message and returns false
 * 3. If it isn't, returns true
 */
bool ServerCommands::checkChannelFull(const std::string &input_channel_name, CH_IT &it, 
	int fd)
{
	if (it->get_clients_in_channel().size() >= it->get_channel_limit() 
		&& it->get_channel_limit() != 0)
	{
		std::string message = ":localhost " + ERR_CHANNELISFULL +
			" : Error[JOIN]: Channel " + input_channel_name + " is full\r\n";
		sendMessage(fd, message);
		return (false);
	}
	return (true);
}

/* validateJoinConditions
** 1. Checks if the channel is full
** 2. Checks if the channel is invite only and if the client is invited
** 3. Checks if the channel has a password and if the password is correct
** 4. Checks if the client is banned from the channel
*/
bool ServerCommands::validateJoinConditions(Client &client, int fd,
	const std::string &input_channel_name, const std::string &input_password,
	CH_IT &it)
{
	if (!checkChannelFull(input_channel_name, it, fd))
		return (false);
	if (!checkInviteOnly(client, input_channel_name, it, fd))
		return (false);
	if (!checkPassword(input_channel_name, input_password, it, fd))
		return (false);
	if (checkBanned(client, input_channel_name, it, fd))
		return (false);
	return (true);
}

/* checkIfClientAlreadyInChannel
** 1. Checks if the client is already in the channel
** 2. If it is, sends an error message and returns false
** 3. If it isn't, returns true
*/
bool ServerCommands::checkIfClientAlreadyInChannel(Client &client, 
	const std::string &input_channel_name, CH_IT &it, int fd)
{
	std::vector<Client> in_channel = it->get_clients_in_channel();

	if (find(in_channel.begin(), in_channel.end(), 
		client.get_client_fd()) != in_channel.end())
	{
		// numeric reply
		std::string message = ":localhost " + ERR_USERONCHANNEL +
			" " + client.get_nickname() + " " + input_channel_name +
			" : Error[JOIN]: " + client.get_nickname() 
			+ " is already in channel " + input_channel_name + "\r\n";
		sendMessage(fd, message);
		return (false);
	}
	return (true);
}
/* createAndJoinChannel
** 1. Checks if the channel exists
** 2. If it doesn't, creates the channel and adds the client to it
** 3. If it does, adds the client to the channel
*/
bool ServerCommands::createAndJoinChannel(Client &client, 
	const std::string &input_channel_name, CH_IT &it, int fd)
{
	if (it == _channels.end())
	{
		if (channel_name_validation(fd, input_channel_name) == 1)
			return (false);
		// Create channel
		Channel new_channel(input_channel_name);
		_channels.push_back(new_channel);
		it = find(_channels.begin(), _channels.end(), input_channel_name);
		it->add_client(client);
		it->add_client_to_clients_operator_vector(client);
		join_messages(client, *it);
		client.set_is_admin(true);
		return (false);
	}
	return (true);
}

/* checkClientRegistration
** 1. Checks if the client is registered
** 2. If it isn't, sends an error message and returns false
** 3. If it is, returns true
*/
bool ServerCommands::checkClientRegistration(Client &client, int fd)
{
	if (!client.get_registered())
	{
		std::string message = ":localhost " + ERR_NOTREGISTERED +
			" : Error[JOIN]: You must be registered to join a channel\r\n";
		sendMessage(fd, message);
		return (false);
	}
	return (true);
}

/* validateJoinPreconditions
** 1. Checks if the client is registered
** 2. Checks if the channel exists
** 3. If it doesn't, creates the channel and adds the client to it
** 4. If it does, adds the client to the channel
** 5. Checks if the client is already in the channel
** 6. If it is, sends an error message and returns false
** 7. If it isn't, returns true
*/
bool ServerCommands::validateJoinPreconditions(Client &client, int fd, 
	const std::string &input_channel_name, CH_IT &it)
{
	if (!checkClientRegistration(client, fd))
		return (false);
	if (!createAndJoinChannel(client, input_channel_name, it, fd))
		return (false);
	if (!checkIfClientAlreadyInChannel(client, input_channel_name, it, fd))
		return (false);
	return (true);
}

/* cmd_join
** 1. Parses the input
** 2. Looks for the channel in the channels vector
** 3. Validates the preconditions for joining a channel:
**		- Client must be registered
**		- Channel must exist, if it doesn't, creates it
**		- Client must not already be in the channel
** 4. Validates the conditions for joining a channel:
**		- Channel must not be full
**		- If channel is invite only, client must be invited
**		- If channel has a password, client must provide the correct password
**		- Client must not be banned from the channel
** 5. Adds the client to the channel
** 6. Sends the JOIN message to the client
*/
int ServerCommands::cmd_join(Client &client, std::string input)
{
	int fd = client.get_client_fd();
	std::string message;
	std::string input_channel_name;
	std::string input_password;
	std::vector<Client> in_channel;  

	// Parsing input
	std::stringstream ss(input);
	ss >> input_channel_name;
	ss >> input_password;

	CH_IT it = find(_channels.begin(), _channels.end(), input_channel_name);

	if (!validateJoinPreconditions(client, fd, input_channel_name, it))
		return (1);
	if (!validateJoinConditions(client, fd, input_channel_name, input_password, 
		it))
		return (1);
	// Add client to channel
	it->add_client(client);
	join_messages(client, *it);
	return (0);
}


void	ServerCommands::cmd_privmsg(Client &client, std::string input)
{
	int					fd = client.get_client_fd();
	std::stringstream	s(input);
	std::string			dest, msg;
	std::string			error;
	Channel				*ch_test = NULL;
	Client				*c_test = NULL;

	if (!client.get_registered())
	{
		error = "Error[PRIVMSG]: You must be registered to send a message\r\n";
		sendMessage(fd, error);
		return;
	}
	s >> dest;
	getline(s, msg);
	msg.erase(0, msg.find_first_not_of(" \t\n\r\f\v"));
	if (msg[0] != ':')
	{
		error = "Error[PRIVMSG]: Usage: /PRIVMSG <destination> :<message>\r\n";
		sendMessage(fd, error);
		return;
	}
	msg = msg.substr(1, msg.length() - 1);
	if (dest[0] == '#')
	{
		ch_test = findChannel(client, dest);
		if (!ch_test)
		{
			error = ":localhost " + ERR_NOSUCHSERVER 
				+ " : Error. Could not find destination\r\n";
			sendMessage(fd, error);
			return;
		}
		// check if client is in channel
		if (find(ch_test->get_clients_in_channel().begin(), 
			ch_test->get_clients_in_channel().end(), 
				client.get_client_fd()) == ch_test->get_clients_in_channel().end())
		{
			error = ":localhost " + ERR_CANNOTSENDTOCHAN 
			+ " : Error[PRIVMSG]: You (" + client.get_nickname() 
			+ ") are not in channel " + dest + "\r\n";
			sendMessage(fd, error);
			return;
		}
		// check if client is banned
		if (ch_test->find_client(client.get_nickname(), "banned"))
		{
			error = ":localhost " + ERR_CANNOTSENDTOCHAN 
				+ " : Error[PRIVMSG]: You (" + client.get_nickname() 
					+ ") are banned from channel " + dest + "\r\n";
			sendMessage(fd, error);
			return;
		}
		ch_test->message(client, msg);
		if (_Clippy.big_brother(*ch_test, client, msg))
			kickClientFromChannel(ch_test, _Clippy, &client, "Profanity");
		return;
	}
	c_test = find_client(client, dest);
	if (!c_test)
	{
		error = ":localhost " + ERR_NOSUCHSERVER 
			+ " : Error. Could not find destination\r\n";
		sendMessage(fd, error);
		return;
	}
	fd = c_test->get_client_fd();
	std::string	final_msg = ":" + client.get_nickname() + "!"
		+ client.get_username() + "@" + "localhost" + " PRIVMSG "
		+ c_test->get_nickname() + " :" + msg + "\r\n";
	sendMessage(fd, final_msg);
}

/* getPartVector
** 1. Parses the input
** 2. Returns a vector containing the channel names and the reason
*/
std::vector<std::string>	ServerCommands::getPartVector(std::string input)
{
	std::stringstream			s1(input);
	std::string					channel_names;
	std::string					channel;
	std::string					reason;
	std::vector<std::string>	part_vector;

	s1 >> channel_names;
	if (!s1.eof())
		s1 >> reason;
	std::stringstream	s2(channel_names);
	while (!s2.eof())
	{
		std::getline(s2, channel, ',');
		if (!channel.empty())
			part_vector.push_back(channel);
	}
	if (!reason.empty())
	{
		reason.erase(0, 1);
		part_vector.push_back(reason);
	}
	return (part_vector);
}

/* cmd_part: PART <channel>{,<channel>} [<reason>]
** 1. Parses the input
** 2. Checks if the client is registered
** 3. Checks if the client is in the channel
** 4. Checks if the client is banned
** 5. Removes the client from the channel
** 6. Sends a message to the client
** 7. Sends a message to the channel
*/
void	ServerCommands::cmd_part(Client &client, std::string input)
{
	int							fd = client.get_client_fd();
	std::string 				nick = client.get_nickname();
	std::string 				msg;
	std::vector<std::string>	parts = getPartVector(input);

	if (parts.empty())
	{
		msg = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[PART]: Usage: /PART <channel> :<reason>\r\n";
		sendMessage(fd, msg);
		return;
	}

	std::string 				reason = parts.back();
	CH_IT 						ch_it;

	for (std::vector<std::string>::iterator	it1 = parts.begin(); 
		it1 < parts.end() - 1; it1++)
	{
		ch_it = find(_channels.begin(), _channels.end(), *it1);
		if (ch_it == _channels.end())
		{
			msg = ":localhost " + ERR_NOSUCHCHANNEL
				+ " : Error[PART]: Channel " + *it1 + " does not exist\r\n";
			sendMessage(fd, msg);
			continue;
		}
		if (!ch_it->find_client(nick, "clients"))
		{
			msg = ":localhost " + ERR_NOTONCHANNEL 
				+ " : Error[PART]: You (" + nick + ") are not in channel " + *it1 + "\r\n";
			sendMessage(fd, msg);
			continue;
		}
		msg = nick + " has left the channel: " + reason;
		ch_it->message(client, msg);
		ch_it->message(client, msg, "PART");
		msg = ":" + nick + "!" + client.get_username() + "@" + "localhost" 
			+ " PART " + *it1 + " :" + reason + "\r\n";
		sendMessage(fd, msg);
		msg = nick + " has quit the channel:";
		ch_it->message(client, msg, "QUIT");
		ch_it->remove_client(client);
		if (ch_it->find_client(nick, "operators"))
			ch_it->remove_client_from_clients_operator_vector(client);
		if (ch_it->get_clients_in_channel().size() == 0)
			_channels.erase(ch_it);
		ch_it->check_operator();
		sendChannelUserListMessage(ch_it.base(), client.get_nickname());
	}
}


/* kickClientFromChannel: KICK <channel> <nickname> [<reason>]
** 1. remove client from channel
** 2. add client to banned list
** 3. if client is in operator_channel, remove from operator_channel
** 4. send message to client
*/
int ServerCommands::kickClientFromChannel(Channel *channel, Client *client, 
	Client *client_to_kick, const std::string &reason)
{
	std::string message;
	
	// Remove client from channel
	channel->remove_client(*(client_to_kick));
	// add client_to_kick to banned list
	channel->add_client_to_banned_vector(*(client_to_kick));
	// if client is in operator_channel, remove from operator_channel
	std::string client_to_kick_nickname = client_to_kick->get_nickname();
	if (channel->find_client(client_to_kick_nickname, "operators"))
		channel->remove_client_from_clients_operator_vector(*client_to_kick);
	// Send message to client
	if (reason.empty())
	{
		message = ":" + client->get_nickname() + " KICK " + channel->get_name()
			+ " " + client_to_kick->get_nickname() + " :This is Sparta!\r\n";
		sendMessage(client_to_kick->get_client_fd(), message);
		sendMessage(client->get_client_fd(), message);
		sendChannelUserListMessage(channel, client->get_nickname());
		return (0);
	}
	else
	{
		message = ":" + client->get_nickname() + " KICK " + channel->get_name()
			+ " " + client_to_kick->get_nickname() + " :" + reason + "\r\n";
		sendMessage(client_to_kick->get_client_fd(), message);
		sendMessage(client->get_client_fd(), message);
		sendChannelUserListMessage(channel, client->get_nickname());
		return (0);
	}
}

/* kickClientFromChannel: KICK <channel> <nickname> [<reason>] (bot)
** Note: This function is used by the bot only
** 1. remove client from channel
** 2. add client to banned list
** 3. if client is in operator_channel, remove from operator_channel
** 4. send message to client
*/
int ServerCommands::kickClientFromChannel(Channel *channel, Bot &bot, 
	Client *client_to_kick, const std::string &reason)
{
	std::string message;
	
	// Remove client from channel
	channel->remove_client(*(client_to_kick));
	// add client_to_kick to banned list
	channel->add_client_to_banned_vector(*(client_to_kick));
	// if client is in operator_channel, remove from operator_channel
	std::string client_to_kick_nickname = client_to_kick->get_nickname();
	if (channel->find_client(client_to_kick_nickname, "operators"))
	{
		channel->remove_client_from_clients_operator_vector(*client_to_kick);
		channel->check_operator();
	}
	// Send message to client
	message = ":" + bot.get_name() + " KICK " + channel->get_name()
		+ " " + client_to_kick->get_nickname() + " :" + reason + "\r\n";
	sendMessage(client_to_kick->get_client_fd(), message);
	sendChannelUserListMessage(channel, bot.get_name());
	return (0);
}

/* performChecks: KICK <channel> <nickname> [<reason>]
** 1. if nickname is empty, send error message
** 2. if nickname is equal to client nickname, send error message
** 3. Find the channel
** 4. if client is not in vector of clients operator_channel, send error message
** 5. if nickname is in operator_channel, send error message
** 6. Find the client to kick in the channel
** 7. if client is not in vector of clients operator_channel, send error message
*/
int ServerCommands::performChecks(Client &client, const std::string &channel_to_find, 
	std::string &nickname, Channel *&channel, Client *&client_to_kick)
{
	if (nickname.empty())
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[KICK]: Usage: KICK <channel> <nickname> [<reason>]\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	if (nickname[0] == ':')
		nickname.erase(0, 1);
	// if nickname is equal to client nickname
	if (nickname == client.get_nickname())
	{
		std::string error = ":localhost " + ERR_CANNOTSENDTOCHAN 
			+ " : Error[KICK]: You can't kick yourself\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the channel
	channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = ":localhost " + ERR_NOSUCHCHANNEL 
			+ " : Error[KICK]: Channel " + channel_to_find 
			+ " does not exist\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find if Client is in vector of clients operator_channel
	std::string client_that_kicked = client.get_nickname();
	if (!channel->find_client(client_that_kicked, "operators"))
	{
		std::string error = ":localhost " + ERR_CHANOPRIVSNEEDED 
			+ " : Error[KICK]: You are not an operator in channel " 
			+ channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// find if nickname is in operator_channel
	if (channel->find_client(nickname, "operators"))
	{
		// numeric reply for hexchat
		std::string error = ":localhost " + ERR_CHANOPRIVSNEEDED 
			+ " : Error[KICK]: " + nickname 
			+ " is an operator in channel " + channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the client to kick in the channel
	client_to_kick = channel->find_client(nickname, "clients");
	if(!client_to_kick)
	{
		std::string error = ":localhost " + ERR_USERNOTINCHANNEL 
			+ " : Error[KICK]: " + nickname + " is not in channel " 
			+ channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	return (0);
}

/* parseKickCommand: KICK <channel> <nickname> [<reason>]
** 1. Skip until finding '#'
** 2. Parse channel name
** 3. Parse nickname
** 4. Parse reason
*/
void ServerCommands::parseKickCommand(std::istringstream &iss, 
	std::string &channel_to_find, std::string &nickname, std::string &reason)
{
	// Parse input
	// Skip until finding '#'
	while (iss.peek() != '#')
		iss.ignore();
	iss >> channel_to_find >> nickname;
	std::getline(iss, reason);
}

/* cmd_kick: kick client from channel (KICK <channel> <nickname> [<reason>])
 * 1. Parse input into channel name and nickname
 * 2. Perform checks, if any of the checks fails, return else continue
 * 3. Kick client from channel
 */
int ServerCommands::cmd_kick(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string nickname;
	std::string reason;

	// Parse input
	parseKickCommand(iss, channel_to_find, nickname, reason);

	Channel *channel = NULL;
	Client *client_to_kick = NULL;
	if (performChecks(client, channel_to_find, nickname, channel, client_to_kick))
		return (1);
	if (kickClientFromChannel(channel, &client, client_to_kick, reason) == -1)
		return (1);
	return (0);
}

int ServerCommands::handleTopicCommand(Client &client, Channel *&channel, 
	const std::string &topic)
{
	std::string message;
	std::string client_nickname = client.get_nickname();
	std::string channel_name = channel->get_name();

	if(topic.empty() || strIsWhitespace(topic))
	{
		if (channel->get_topic().empty())
			message = ":localhost " + RPL_NOTOPIC 
				+ " " + client_nickname + " " + channel_name 
				+ " :No topic is set\r\n";
		else
			message = ":localhost " + RPL_TOPIC 
				+ " " + client_nickname + " " + channel_name 
				+ " :" + channel->get_topic() + "\r\n";
		sendMessage(client.get_client_fd(), message);
		return (0);
	}
	if (channel->get_topic_mode() == true 
		&& channel->find_client(client_nickname, "operators"))
	{
		// RPL numeric for hexchat
		std::string success = ":localhost " + RPL_TOPIC 
			+ " " + client_nickname + " " + channel_name 
			+ " :" + topic + "\r\n";
		sendMessage(client.get_client_fd(), success);
		channel->set_topic(topic);
	}
	else if (channel->get_topic_mode() == false 
		&& channel->find_client(client_nickname, "clients"))
	{
		std::string success = ":localhost " + RPL_TOPIC 
			+ " " + client_nickname + " " + channel_name 
			+ " :" + topic + "\r\n";
		sendMessage(client.get_client_fd(), success);
		channel->set_topic(topic);
	}
	else if (!channel->find_client(client_nickname, "operators"))
	{
		std::string error = ":localhost " + ERR_CHANOPRIVSNEEDED 
			+ " : Error[TOPIC]: " + client_nickname 
			+ " is not an operator in channel " + channel_name + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (-1);
	}
	return (0);
}

int ServerCommands::handleTopicErrors(Client &client, const std::string &channel_to_find, 
	Channel *&channel)
{
	if (channel_to_find.empty() || strIsWhitespace(channel_to_find))
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[TOPIC]: Usage: TOPIC <channel> [<topic>]\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the channel
	channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = ":localhost " + ERR_NOSUCHCHANNEL 
			+ " : Error[TOPIC]: Channel " + channel_to_find 
			+ " does not exist\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	return (0);
}

/* cmd_topic: set topic of channel
 * 1. Parse input into channel name and topic
 ** first skip until finding '#' then parse channel name
 ** then skip all kind of leading whitespaces then parse topic(rest of input)
 */
void ServerCommands::parseTopicCommand(std::istringstream &iss, 
	std::string &channel_to_find, std::string &topic)
{
	// Parse input
	// Skip until finding '#'
	while (iss.peek() != '#')
		iss.ignore();
	iss >> channel_to_find;
	// skip all kind of leading whitespaces
	iss >> std::ws;
	//skip : skip only the first ':'
	if (iss.peek() == ':')
		iss.ignore();
	std::getline(iss, topic);
}

/* cmd_topic: set topic of channel
 * 1. Parse input into channel name and topic
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Set topic of channel
 */
int ServerCommands::cmd_topic(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string topic;
	
	// Parse input
	parseTopicCommand(iss, channel_to_find, topic);

	Channel *channel = NULL;
	if (handleTopicErrors(client, channel_to_find, channel) == 1)
		return (1);
	if (handleTopicCommand(client, channel, topic) == -1)
		return (-1);
	return (0);
}

/* handleInviteErrors: check if input is valid
 * 1. Check if input is empty
 * 2. Check if channel exists
 * 3. Check if client is in operator in channel
 * 4. Check if client is already invited
 * 5. Check if client is already in channel
 */
int ServerCommands::handleInviteErrors(Client &client, const std::string &nickname, 
	const std::string &channel_to_find, Channel *&channel)
{
	if (nickname.empty() || strIsWhitespace(nickname) 
		|| channel_to_find.empty() || strIsWhitespace(channel_to_find))
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[INVITE]: Usage: INVITE <nickname> <channel>\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the channel
	channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = ":localhost " + ERR_NOSUCHCHANNEL 
			+ " : Error[INVITE]: Channel " + channel_to_find 
			+ " does not exist\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// if client is not operator in channel
	if (!channel->find_client(client.get_nickname(), "operators"))
	{
		std::string error = ":localhost " + ERR_CHANOPRIVSNEEDED 
			+ " : Error[INVITE]: You (" + client.get_nickname()
			+ ") are not an operator in channel " + channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// find if client is already invited
	if (channel->find_client(nickname, "invited"))
	{
		std::string error = ":localhost " + ERR_USERONCHANNEL 
			+ " : Error[INVITE]: Client " + nickname 
			+ " is already invited to channel " + channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	// find if client is already in channel
	if (channel->find_client(nickname, "clients"))
	{
		std::string error = ":localhost " + ERR_USERONCHANNEL 
			+ " : Error[INVITE]: Client " + nickname 
			+ " is already in channel " + channel_to_find + "\r\n";
		sendMessage(client.get_client_fd(), error);
		return (1);
	}
	return (0);
}

/* cmd_invite: invite client to channel (INVITE <nickname> <channel>)
 * 1. Parse input into nickname and channel name
 * 2. Check if client is operator in channel
 * 3. Find the channel
 * 4. Find the client
 * 5. Invite client to channel
 */
int ServerCommands::cmd_invite(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string nickname;
	Channel *channel = NULL;

	// Parse input
	iss >> nickname;
	iss >> channel_to_find;
	if (handleInviteErrors(client, nickname, channel_to_find, channel) == 1)
		return (1);
	// Find the client to invite
	Client *client_to_invite = find_client(client, nickname);
	if (!client_to_invite)
	{
		std::string message = "Error[INVITE]: user with nickname " + nickname 
			+ " does not exist\r\n";
		sendMessage(client.get_client_fd(), message);
		return (1);
	}
	// Invite the client
	channel->add_client_to_clients_invited_vector(*client_to_invite);
	std::string message = "[INVITE] " + client.get_nickname() 
		+ " has invited you to join " + channel->get_name() + "\r\n";
	sendMessage(client_to_invite->get_client_fd(), message);
	return (0);
}

/*sendMessage: send message to client
 * 1. Send message to client if error occurs return -1
 */
int ServerCommands::sendMessage(int client_fd, const std::string &msg)
{
	if (send(client_fd, msg.c_str(), msg.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}

/* name_validation: check if name is valid
** 1. check if name is between 1 and 9 characters long
** 2. check if name starts with '#' or ':'
** 3. check if name has whitespaces
** 4. check if name has invalid characters
** 5. return true if name is valid
** 6. return false if name is invalid
*/
bool	ServerCommands::name_validation(std::string check)
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

/* nick_validation: check if nickname is valid
** 1. check if nickname is valid
** 2. check if nickname is already in use
** 3. return 0 if nickname is valid and not in use
** 4. return 1 if nickname is invalid
** 5. return 2 if nickname is already in use
*/
int	ServerCommands::nick_validation(std::string check)
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

/* name_compare: compare two strings
** 1. transform both strings to lowercase
** 2. compare strings
** 3. return 1 if strings are equal, 0 if not
*/
int	ServerCommands::name_compare(std::string check, std::string comp)
{
	std::cout << "Name to check is: " << check << " comparing against: " << comp << "\n";
	std::transform(check.begin(), check.end(), check.begin(), tolower);
	std::transform(comp.begin(), comp.end(), comp.begin(), tolower);
	std::cout << "After tolower: " << check << " and " << comp << "\n";
	if (check == comp)
		return 1;
	return 0;
}

/* disconnect_client: remove client from server
** 1. find client in _clients vector
** 2. remove client from all channels
** 3. close client's fd
** 4. remove client from _clients vector
*/
void	ServerCommands::disconnect_client(int fd)
{
	C_IT	end = _clients.end();
	C_IT	match = std::find(_clients.begin(), end, fd);

	if (match == end)
		throw(std::runtime_error("Error. Could not find client"));
	leave_all_rooms(*match);
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
void	ServerCommands::leave_all_rooms(Client &client)
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

/* channel_name_validation: check if channel name is valid
** 1. check if channel name starts with '#'
** 2. check if channel name has whitespaces
** 3. check if channel name is between 2 and 200 characters long
*/
int ServerCommands::channel_name_validation(int client_fd, std::string check)
{
	int	len = check.size();
	std::string message;

	if (check[0] != '#')
	{
		message = "Error[JOIN]: Channel name must start with '#'\r\n";
		sendMessage(client_fd, message);
		return (1);
	}
	for (int i = 1; i < len; i++)
	{
		if (!std::isalnum(check[i]) && (check[i] != '_') && (check[i] != '-'))
		{
			message = "Error[JOIN]: Channel name can only contain alphanumeric characters, '_' and '-'\r\n";
			sendMessage(client_fd, message);
			return (1);
		}
	}
	if (len < 2 || len > 200)
	{
		message = "Error[JOIN]: Channel name must be between 2 and 200 characters long\r\n";
		sendMessage(client_fd, message);
		return (1);
	}
	return (0);
}

/* is_client_admin: check if client is administrator
 * 1. Check if client is administrator
 * 2. Send error message if client is not administrator
 */
int ServerCommands::is_client_admin(Client &client) // Do we need this? Deprecated?
{
	if (!client.get_is_admin())
	{
		std::string error = "Error: " + client.get_nickname() 
			+ " is not administrator\r\n";
		sendMessage(client.get_client_fd(), error);
		return (-1);
	}
	return 0;
}

/* findChannel: find channel
 * 1. Find channel by iterating through _channels
 * 2. If channel does not exist, send error message to client and return NULL
 * else return channel
 */
Channel	*ServerCommands::findChannel(Client &client, const std::string	&channelName)
{
	std::vector<Channel>::iterator it = find(_channels.begin(), _channels.end(), channelName);

	(void)client;
	if (it == _channels.end())
		return NULL;
	return &(*it);
}

/* find_client: find client
 * 1. Find client by iterating through _clients
 * 2. If client does not exist, send error message to client and return NULL
 * else return client
 */
Client	*ServerCommands::find_client(Client &client, const std::string& nickname)
{
	(void)client;

	std::vector<Client>::iterator it = find(_clients.begin(), _clients.end(), nickname);

	if (it == _clients.end())
		return (NULL);
	return &(*it);
}

/* password_checker: check if password is valid
 * 1. Check if password is between 3 and 12 characters
 * 2. Check if password contains only non-printable characters
 */
int	ServerCommands::password_checker(std::string password, int fd)
{
	// Check if password is between 3 and 12 characters
	if (password.length() < 3 || password.length() > 12)
	{
		std::string error = "Error. Password must be between 3 and 12 characters\r\n";
		sendMessage(fd, error);
		return (1);
	}
	// Check if password contains non-printable characters
	for (size_t i = 0; i < password.length(); i++)
	{
		if (!isprint(password[i]))
		{
			std::string error = "Error. Password must not contain non-printable characters\r\n";
			sendMessage(fd, error);
			return (1);
		}
	}
	return (0);
}

/* get_users_string: get users string
 * 1. Get list of clients in channel
 * 2. Iterate through list and add clients to string
 * 3. Return string
 */
std::string	ServerCommands::get_users_string(Channel &channel)
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

/* join_messages: send messages to client and channel
 * 1. Send JOIN message to client
 * 2. Send RPL_TOPIC message to client
 * 3. Send RPL_NAMREPLY message to client
 * 4. Send RPL_ENDOFNAMES message to client
 * 5. Send JOIN message to channel
 */
void	ServerCommands::join_messages(Client &client, Channel &channel)
{
	std::string	message;
	std::string	name = channel.get_name();
	int			fd = client.get_client_fd();

	message = ":" + client.get_nickname() + "!" + client.get_username() + "@" 
		+ "localhost" + " JOIN " + name + "\r\n";
	sendMessage(fd, message);
	message = ":localhost " + RPL_TOPIC + " " + client.get_nickname() + " " 
		+ name + " " + channel.get_topic() + "\r\n";
	sendMessage(fd, message);
	message = ":localhost " + RPL_NAMREPLY + " " + client.get_nickname() + " = " 
		+ name + " :" + get_users_string(channel) + "\r\n";
	channel.info_message(message);
	message = ":localhost " + RPL_ENDOFNAMES + " " + client.get_nickname() + " " 
		+ name + " :End of NAMES list\r\n";
	channel.info_message(message);
}

/* sendChannelUserListMessage: send channel user list message
 * 1. Send message to client with list of users in channel
 * 2. Send message to client with end of names list
 */
void ServerCommands::sendChannelUserListMessage(Channel *channel, const std::string &argument)
{
	std::string userListMessage = ":localhost " + RPL_NAMREPLY + " " + argument
		+ " = " + channel->get_name() + " :" + get_users_string(*channel) 
		+ "\r\n";
	channel->info_message(userListMessage);

	std::string endOfNamesMessage = ":localhost " + RPL_ENDOFNAMES + " " 
		+ argument + " " + channel->get_name() + " :End of NAMES list\r\n";
	channel->info_message(endOfNamesMessage);
}
