/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 08:29:50 by gacorrei          #+#    #+#             */
/*   Updated: 2023/11/30 09:02:25 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* parseArgsListCommand: /list #channel1,#channel2,#channel3
** 1. parse input string into vector of strings by comma
** 2. return vector of strings
*/
std::vector<std::string> Server::parseArgsListCommand(std::string input)
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
int Server::cmdListWithArg(Client &client, std::string input)
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
			sendSuccessMessage(client.get_client_fd(), msg2);
		}
		else
		{
			std::string msg2 = ":localhost " + ERR_NOSUCHCHANNEL + " " 
				+ client.get_nickname() + " " + *it + " :No such channel\r\n";
			sendErrorMessage(client.get_client_fd(), msg2);
		}
	}
	return (0);
}

/* cmdListNoArgs: /list
** 1. send RPL_LIST message for each channel
*/
int Server::cmdListNoArgs(Client &client)
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
		sendSuccessMessage(client.get_client_fd(), msg2);
	}
		return (0);
}

/*cmd_list: /list #channel1,#channel2,#channel3 or /list
** 1. message RPL_LISTSTART
** 2. if input is empty, list all channels
** 3. else, list channels that match input
** 4. message RPL_LISTEND
*/
int Server::cmd_list(Client &client, std::string input)
{
	std::string msg = ":localhost " + RPL_LISTSTART + " " 
		+ client.get_nickname() + " :Channel list\r\n";
	sendSuccessMessage(client.get_client_fd(), msg);
	// if input is empty, list all channels
	if (input.empty() && _channels.size() > 0)
	{
		if (cmdListNoArgs(client) == 1)
			return (1);
	}
	// else, list channels that match input
	else if (!input.empty() && _channels.size() > 0)
	{
		if (cmdListWithArg(client, input) == 1)
			return (1);
	}
	std::string msg2 = ":localhost " + RPL_LISTEND + " " 
		+ client.get_nickname() + " :End of /LIST\r\n";
	sendSuccessMessage(client.get_client_fd(), msg2);
	return (0);
}

/* sendWhoReplyMessages: send RPL_WHOREPLY message for each client in channel
** 1. iterate through clients in channel
** 2. send RPL_WHOREPLY message for each client
** 3. send RPL_ENDOFWHO message
*/
void Server::sendWhoReplyMessages(Client &client, Channel &channel)
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
        sendSuccessMessage(client.get_client_fd(), msg);
    }

    msg = ":localhost " + RPL_ENDOFWHO + " " + client.get_nickname() + " " +
          channel.get_name() + " :End of WHO list\r\n";
    sendSuccessMessage(client.get_client_fd(), msg);
}

/* cmd_who: /who #channel1
** 1. if input is empty,send error message
** 2. if channel doesn't exist, send ERR_NOSUCHCHANNEL message
** 3. else, send RPL_WHOREPLY message for each client in channel
** 4. send RPL_ENDOFWHO message
*/
int Server::cmd_who(Client &client, std::string input)
{
    if (input.empty())
    {
        std::string error = "Error[MODE]: No argument provided\r\n";
        sendErrorMessage(client.get_client_fd(), error);
        return (1);
    }

    Channel *channel = findChannel(client, input);

    if (!channel)
    {
        std::string error = ":localhost " + ERR_NOSUCHCHANNEL +
            " : Error[MODE]: Channel " + input + " does not exist\r\n";
        sendErrorMessage(client.get_client_fd(), error);
        return (1);
    }
    sendWhoReplyMessages(client, *channel);
    return (0);
}

/* handleModeMinusL: MODE <channel> -l
** 1. if channel limit is already 0 (unlimited), send error message
** 2. else, set channel limit to 0 (unlimited)
*/
int Server::handleModeMinusL(Channel *channel, int fd)
{
	std::string message;
	// Expecting format: MODE <channel> -l
	// check if channel limit is already 0
	if (channel->get_channel_limit() == 0)
	{
		message = "Error[MODE -l]: Channel " + channel->get_name() 
			+ " is not limited\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// set channel limit
	channel->set_channel_limit(0);
	message = "Success[MODE -l]: Channel " + channel->get_name() 
		+ " is no longer limited\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModePlusL: MODE <channel> +l <limit>
** 1. if channel is already limited, send error message
** 2. if no limit is specified, send error message
** 3. if limit is not a number or not in integer range, send error message
** 4. else, set channel limit to argument converted to int
*/
int Server::handleModePlusL(Channel *channel, std::string argument, int fd)
{
	// Expecting format: MODE <channel> +l <limit>
	// check if channel is already limited
	if (channel->get_channel_limit() != 0)
	{
		std::string message = "Error[MODE +l]: Channel " + channel->get_name() 
			+ " is already limited\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	if (argument.empty())
	{
		std::string message = "Error[MODE +l]: No limit specified\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	if (isStrOnlyDigits(argument) == false)
	{
		std::string message = "Error[MODE +l]: " + argument 
			+ " is not a valid number\r\n";
		sendErrorMessage(fd, message);
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
		sendErrorMessage(fd, message);
		return (1);
	}
	std::string message = "Success[MODE +l]: Channel " 
		+ channel->get_name() + " is now limited to " + argument + " users\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModeMinusT: MODE <channel> -t
** 1. if channel is already topic protected, send error message
** 2. else, set channel to topic protected
*/
int Server::handleModeMinusT(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> -t
	// check if channel is already topic protected
	if (channel->get_topic_mode() == false)
	{
		message = "Error[MODE -t]: Channel " + channel->get_name() 
			+ " is not topic protected\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// set channel to topic protected
	channel->set_topic_mode(false);
	message = "Success[MODE -t]: Channel " + channel->get_name() 
		+ " is no longer topic protected\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModePlusT: MODE <channel> +t
** 1. if channel is already topic protected, send error message
** 2. else, set channel to topic protected
*/
int Server::handleModePlusT(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +t
	// check if channel is already topic protected
	if (channel->get_topic_mode() == true)
	{
		message = "Error[MODE +t]: Channel " + channel->get_name() 
			+ " is already topic protected\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// set channel to topic protected
	channel->set_topic_mode(true);
	message = "Success[MODE +t]: Channel " + channel->get_name() 
		+ " is now topic protected\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModeMinusI: MODE <channel> -i
** 1. if channel is already invite only, send error message
** 2. else, set channel to invite only
*/
int Server::handleModeMinusI(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> -i
	// check if channel is already invite only
	if (channel->get_channel_invite_only() == false)
	{
		message = "Error[MODE -i]: Channel " + channel->get_name() 
			+ " is not invite only\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// set channel to invite only
	channel->set_channel_invite_only(false);
	message = "Success[MODE -i]: Channel " + channel->get_name() 
		+ " is no longer invite only\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModePlusI: MODE <channel> +i
** 1. if channel is already invite only, send error message
** 2. else, set channel to invite only
*/
int Server::handleModePlusI(Channel *channel, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +i
	// check if channel is already invite only
	if (channel->get_channel_invite_only() == true)
	{
		message = "Error[MODE +i]: Channel " + channel->get_name() 
			+ " is already invite only\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}

	// set channel to invite only
	channel->set_channel_invite_only(true);
	message = "Success[MODE +i]: Channel " + channel->get_name() 
		+ " is now invite only\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModeMinusK: MODE <channel> -k
** 1. if channel doesn't have a password, send error message
** 2. else, remove password of channel
*/
int Server::handleModeMinusK(Channel *channel, int fd)
{
	std::string message;
	
	// Expecting format: MODE <channel> -k
	// check if channel is already password protected
	if (channel->get_password().empty())
	{
		message = "Error[MODE -k]: Channel " + channel->get_name() 
			+ " is not password protected\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// remove password of channel
	channel->set_password("");
	message = "Success[MODE -k]: Channel " + channel->get_name() 
		+ " is no longer password protected\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModePlusK: MODE <channel> +k <password>
** 1. if channel is already password protected, send error message
** 2. if password is invalid, send error message
** 3. else, change password of channel
*/
int Server::handleModePlusK(Channel *channel, std::string argument, int fd)
{
	std::string message;

	// Expecting format: MODE <channel> +k <password>
	// check if channel is already password protected
	if (!channel->get_password().empty())
	{
		message = "Error[MODE +k]: Channel " + channel->get_name() 
			+ " is already password protected\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// check if password is valid
	if (password_checker(argument, fd))
		return (1);
	// change password of channel
	channel->set_password(argument);
	message = ":localhost " + RPL_CHANNELMODEIS + " " 
		+ channel->get_name() + " +k is now password protected\r\n";
	sendSuccessMessage(fd, message);
	return (0);
}

/* handleModeMinusO: MODE <channel> -o <nickname>
** 1. if client is not in channel, send error message
** 2. if client is not an operator, send error message
** 3. if client is the last operator, send error message
** 4. else, remove client from operators vector
*/
int Server::handleModeMinusO(Client &client, Channel *channel, std::string argument, int fd)
{
	std::string message;
	
	// Expecting format: MODE <channel> -o <nickname>
	// look if argument(client to become operator) is in the channel
	if (!channel->find_client(argument, "clients"))
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE -o]: " 
			+ argument + " is not in the channel " + channel->get_name() + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// look for nickname in clients operator_channel
	if (!channel->find_client(argument, "operators"))
	{
		message = "Error[MODE -o]: " + argument 
			+ " is not an operator in channel " + channel->get_name() + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// if client is the last operator in the channel, send error message
	if (channel->get_clients_operator_channel().size() == 1)
	{
		message = "Error[MODE -o]: " + argument 
			+ " is the last operator in channel " + channel->get_name() + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// remove nickname from clients operator_channel
	// find client by nickname
	Client *client_to_remove = find_client(client, argument);
	if (!client_to_remove)
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE -o]: Client " 
			+ argument + " does not exist\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	channel->remove_client_from_clients_operator_vector(*client_to_remove);
	message = "Success[MODE -o]: " + argument 
		+ " is no longer an operator in channel " + channel->get_name() + "\r\n";
	sendSuccessMessage(fd, message);
	sendSuccessMessage(client_to_remove->get_client_fd(), message);
	sendChannelUserListMessage(channel, argument);
	return (0);
}

/* handleModePlusO: MODE <channel> +o <nickname>
** 1. if client is not in channel, send error message
** 2. if client is already an operator, send error message
** 3. else, add client to operators vector
*/
int Server::handleModePlusO(Client &client, Channel *channel, 
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
		sendErrorMessage(fd, message);
		return (1);
	}
	// look for nickname in clients operator_channel
	if (channel->find_client(argument, "operators"))
	{
		message = "Error[MODE +o]: " + argument 
			+ " is already an admin in channel " + channel->get_name() + "\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	// add nickname to clients operator_channel
	// find client by nickname
	Client *client_to_add = find_client(client, argument);
	if (!client_to_add)
	{
		message = ":localhost " + ERR_NOSUCHNICK + " : Error[MODE +o]: Client " 
			+ argument + " does not exist\r\n";
		sendErrorMessage(fd, message);
		return (1);
	}
	channel->add_client_to_clients_operator_vector(*client_to_add);
	// send success message to client and channel with reply 381
	message =  ":localhost " + RPL_YOUREOPER + " " + argument + " : [MODE +o]" 
		+ argument + " is now an operator in channel " + channel->get_name() 
		+ "\r\n";
	sendSuccessMessage(fd, message);
	sendSuccessMessage(client_to_add->get_client_fd(), message);
	// send channel user list to all clients in channel
	sendChannelUserListMessage(channel, argument);
	return (0);
}

/* handleMode: MODE <channel> [+|-][o|k|i|t|l] <argument>
** 1. call the appropriate function based on the mode
** 2. return 2 if mode is invalid
*/
int  Server::handleMode(Client &client, Channel *channel, const std::string &mode, const std::string &argument, int fd)
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
int Server::sendOperatorPrivilegeError(int fd, const std::string &channel)
{
    std::string message = ":localhost " + ERR_CHANOPRIVSNEEDED +
        " : Error[MODE]: You are not an operator in channel " + channel + "\r\n";
    return (sendErrorMessage(fd, message));
}

/* sendChannelModeInfo: send RPL_CHANNELMODEIS message
** 1. send RPL_CHANNELMODEIS message
*/
int Server::sendChannelModeInfo(int fd, Channel *channel, 
	const std::string &channelName)
{
    std::string message = ":localhost " + RPL_CHANNELMODEIS + " " + channelName 
		+ " " + channelName + ": " + channel->get_mode() + "\r\n";
    return (sendSuccessMessage(fd, message));
}

int Server::sendChannelNotFoundError(int fd, const std::string &channel)
{
    std::string message = ":localhost " + ERR_NOSUCHCHANNEL 
		+ " : Error[MODE]: Channel " + channel + " does not exist\r\n";
    return (sendErrorMessage(fd, message));
}

// Debug delete later
void Server::printDebugInfo(const std::string &channel, const std::string &mode, const std::string &argument)
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
int Server::cmd_mode(Client &client, std::string input)
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
		sendErrorMessage(fd, error);
		return (1);
	}
	return (0);
}

void	Server::cmd_pass(Client &client, std::string input)
{
	int	fd = client.get_client_fd();

	if (client.get_authenticated())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[PASS]: You are already authenticated\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (pass_validation(input))
	{
		std::string success = "Success[PASS]: You are now authenticated\r\n";
		sendSuccessMessage(fd, success);
		client.set_authenticated(true);
		return;
	}
	if (client.pass_counter(0, 0) == 2)
	{
		std::string error = ":localhost " + ERR_PASSWDMISMATCH 
			+ " : Error[PASS]: Too many wrong attempts, disconnecting\r\n";
		sendErrorMessage(fd, error);
		disconnect_client(fd);
	}
	std::string error = ":localhost " + ERR_PASSWDMISMATCH 
		+ " : Error[PASS]: Wrong password\r\n";
	sendErrorMessage(fd, error);
	client.pass_counter(1, 0);
}

void	Server::cmd_user(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	std::stringstream	ss(input);
	std::string			username;

	ss >> username;
	if (client.get_registered())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[USER]: You are already registered\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_authenticated())
	{
		std::string error = "Error[USER]: You are not authenticated. Input password first\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_username().empty())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[USER]: You already have a username\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (name_validation(username))
	{
		client.set_username(username);
		std::string success = "Success[USER]: Username set to " 
			+ username + "\r\n";
		sendSuccessMessage(fd, success);
		if (!client.get_nickname().empty())
		{
			success = "Success[USER]: You are now registered\r\n";
			sendSuccessMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[USER]: Username can't have spaces or symbols (except [-_[]{}\\|]), can't start with '#' or ':' and must be between 3 and 10 characters long\r\n";
		sendErrorMessage(fd, error);
	}
}

void	Server::cmd_nick(Client &client, std::string input)
{
	int	fd = client.get_client_fd();
	int	test;

	if (client.get_registered())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[NICK]: You are already registered\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_authenticated())
	{
		std::string error = "Error[NICK]: You are not authenticated. Input password first\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	if (!client.get_nickname().empty())
	{
		std::string error = ":localhost " + ERR_ALREADYREGISTERED 
			+ " : Error[NICK]: You already have a nickname\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	test = nick_validation(input);
	if (!test)
	{
		client.set_nickname(input);
		std::string success = "Success[NICK]: Nickname set to " + input + "\r\n";
		sendSuccessMessage(fd, success);
		if (!client.get_username().empty())
		{
			success = "Success[NICK]: You are now registered\r\n";
			sendSuccessMessage(fd, success);
			client.set_registered(true);
		}
		return;
	}
	else if (test == 1)
	{
		std::string error = ":localhost " + ERR_ERRONEUSNICKNAME 
			+ " : Error[NICK]: Nickname can't have spaces or symbols (except [-_[]{}\\|]), can't start with '#' or ':' and must be between 3 and 10 characters long\r\n";
		sendErrorMessage(fd, error);
	}
	else
	{
		std::string error = ":localhost " + ERR_NICKNAMEINUSE 
			+ " : Error[NICK]: Nickname already in use, choose another\r\n";
		sendErrorMessage(fd, error);
	}
}

/* checkBanned
 * 1. Checks if the client is banned from the channel
 * 2. If it is, sends an error message and returns true
 * 3. If it isn't, returns false
 */
bool Server::checkBanned(Client &client, const std::string &input_channel_name, 
	CH_IT &it, int fd)
{
	if (it->find_client(client.get_nickname(), "banned"))
	{
		std::string message = ":localhost " + ERR_BANNEDFROMCHAN +
			" : Error[JOIN]: You (" + client.get_nickname()
			+ ") are banned from channel " + input_channel_name + "\r\n";
		sendErrorMessage(fd, message);
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
bool Server::checkPassword(const std::string &input_channel_name,
	const std::string &input_password, CH_IT &it, int fd)
{
	if (!it->get_password().empty())
	{
		if (it->get_password() != input_password)
		{
			std::string message = "Error[JOIN]: Wrong password for channel " +
								  input_channel_name + "\r\n";
			sendErrorMessage(fd, message);
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
bool Server::checkInviteOnly(Client &client, 
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
			sendErrorMessage(fd, message);
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
bool Server::checkChannelFull(const std::string &input_channel_name, CH_IT &it, 
	int fd)
{
	if (it->get_clients_in_channel().size() >= it->get_channel_limit() 
		&& it->get_channel_limit() != 0)
	{
		std::string message = ":localhost " + ERR_CHANNELISFULL +
			" : Error[JOIN]: Channel " + input_channel_name + " is full\r\n";
		sendErrorMessage(fd, message);
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
bool Server::validateJoinConditions(Client &client, int fd,
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
bool Server::checkIfClientAlreadyInChannel(Client &client, 
	const std::string &input_channel_name, CH_IT &it, int fd)
{
	std::vector<Client> in_channel = it->get_clients_in_channel();

	if (find(in_channel.begin(), in_channel.end(), 
		client.get_client_fd()) != in_channel.end())
	{
		std::string message = "Error[JOIN]: " + client.get_nickname() 
			+ " is already in channel " + input_channel_name + "\r\n";
		sendErrorMessage(fd, message);
		return (false);
	}
	return (true);
}
/* createAndJoinChannel
** 1. Checks if the channel exists
** 2. If it doesn't, creates the channel and adds the client to it
** 3. If it does, adds the client to the channel
*/
bool Server::createAndJoinChannel(Client &client, 
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
bool Server::checkClientRegistration(Client &client, int fd)
{
	if (!client.get_registered())
	{
		std::string message = "Error[JOIN]: You must be registered to join a channel\r\n";
		sendErrorMessage(fd, message);
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
bool Server::validateJoinPreconditions(Client &client, int fd, 
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
int Server::cmd_join(Client &client, std::string input)
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


void	Server::cmd_privmsg(Client &client, std::string input)
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
		sendErrorMessage(fd, error);
		return;
	}
	s >> dest;
	getline(s, msg);
	msg.erase(0, msg.find_first_not_of(" \t\n\r\f\v"));
	if (msg[0] != ':')
	{
		error = "Error[PRIVMSG]: Usage: /PRIVMSG <destination> :<message>\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	msg = msg.substr(1, msg.length() - 1);
	if (dest[0] == '#')
	{
		ch_test = findChannel(client, dest);
		if (!ch_test)
		{
			error = ":localhost " + ERR_NOSUCHSERVER + " : Error. Could not find destination\r\n";
			sendErrorMessage(fd, error);
			return;
		}
		// check if client is in channel
		if (find(ch_test->get_clients_in_channel().begin(), ch_test->get_clients_in_channel().end(), client.get_client_fd()) == ch_test->get_clients_in_channel().end())
		{
			error = ":localhost " + ERR_CANNOTSENDTOCHAN + " : Error[PRIVMSG]: You (" + client.get_nickname() + ") are not in channel " + dest + "\r\n";
			sendErrorMessage(fd, error);
			return;
		}
		// check if client is banned
		if (ch_test->find_client(client.get_nickname(), "banned"))
		{
			error = ":localhost " + ERR_CANNOTSENDTOCHAN + " : Error[PRIVMSG]: You (" + client.get_nickname() + ") are banned from channel " + dest + "\r\n";
			sendErrorMessage(fd, error);
			return;
		}
		ch_test->message(client, msg);
		if (_Clippy.big_brother(*ch_test, client, msg))
			kickClientFromChannel(ch_test, &ch_test->get_operator(), &client, "Profanity");
		return;
	}
	c_test = find_client(client, dest);
	if (!c_test)
	{
		error = ":localhost " + ERR_NOSUCHSERVER + " : Error. Could not find destination\r\n";
		sendErrorMessage(fd, error);
		return;
	}
	fd = c_test->get_client_fd();
	std::string	final_msg = ":" + client.get_nickname() + "!"
		+ client.get_username() + "@" + "localhost" + " PRIVMSG "
		+ c_test->get_nickname() + " :" + msg + "\r\n";
	sendSuccessMessage(fd, final_msg);
}

/* kickClientFromChannel: KICK <channel> <nickname> [<reason>]
** 1. remove client from channel
** 2. add client to banned list
** 3. if client is in operator_channel, remove from operator_channel
** 4. send message to client
*/
int Server::kickClientFromChannel(Channel *channel, Client *client, 
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
		sendSuccessMessage(client_to_kick->get_client_fd(), message);
		sendSuccessMessage(client->get_client_fd(), message);
		sendChannelUserListMessage(channel, client->get_nickname());
		return (0);
	}
	else
	{
		message = ":" + client->get_nickname() + " KICK " + channel->get_name()
			+ " " + client_to_kick->get_nickname() + " :" + reason + "\r\n";
		sendSuccessMessage(client_to_kick->get_client_fd(), message);
		sendSuccessMessage(client->get_client_fd(), message);
		sendChannelUserListMessage(channel, client->get_nickname());
		return (0);
	}
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
int Server::performChecks(Client &client, const std::string &channel_to_find, 
	std::string &nickname, Channel *&channel, Client *&client_to_kick)
{
	if (nickname.empty())
	{
		std::string error = ":localhost " + ERR_NEEDMOREPARAMS 
			+ " : Error[KICK]: Usage: KICK <channel> <nickname> [<reason>]\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	if (nickname[0] == ':')
		nickname.erase(0, 1);
	// if nickname is equal to client nickname
	if (nickname == client.get_nickname())
	{
		std::string error = ":localhost " + ERR_CANNOTSENDTOCHAN 
			+ " : Error[KICK]: You can't kick yourself\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the channel
	channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = ":localhost " + ERR_NOSUCHCHANNEL 
			+ " : Error[KICK]: Channel " + channel_to_find 
			+ " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find if Client is in vector of clients operator_channel
	std::string client_that_kicked = client.get_nickname();
	if (!channel->find_client(client_that_kicked, "operators"))
	{
		std::string error = ":localhost " + ERR_CHANOPRIVSNEEDED 
			+ " : Error[KICK]: You are not an operator in channel " 
			+ channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// find if nickname is in operator_channel
	if (channel->find_client(nickname, "operators"))
	{
		std::string error = "Error[KICK]: " + nickname 
			+ " is an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (1);
	}
	// Find the client to kick in the channel
	client_to_kick = channel->find_client(nickname, "clients");
	if(!client_to_kick)
	{
		std::string error = ":localhost " + ERR_USERNOTINCHANNEL 
			+ " : Error[KICK]: " + nickname + " is not in channel " 
			+ channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
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
void Server::parseKickCommand(std::istringstream &iss, 
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
int Server::cmd_kick(Client &client, std::string input)
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

/* cmd_topic: set topic of channel
 * 1. Parse input into channel name and topic
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Set topic of channel
 */
int Server::cmd_topic(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string topic;
	
	// Parse input
	iss >> channel_to_find;
	// skip all kind of leading whitespaces
	iss >> std::ws;
	//skip : skip only the first ':'
	
	if (iss.peek() == ':')
		iss.ignore();
	//iss.ignore(1, ':'); //different from the above, this will ignore all ':' 
	std::getline(iss, topic);

	// if no channel name is given
	if (channel_to_find.empty() || strIsWhitespace(channel_to_find))
	{
		std::string error = "Error[TOPIC]: Usage: topic <channel> [topic]\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string error = "Error[TOPIC]: " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	// if client does not belong to channel
	std::string client_nickname = client.get_nickname();
	if (!channel->find_client(client_nickname, "clients"))
	{
		std::string error = "Error[TOPIC]: You (" + client.get_nickname() + ") are not in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}
	// Set topic of channel
	if (topic.empty() || strIsWhitespace(topic))
	{
		std::string message;
		// send the topic to client
		if (channel->get_topic().empty())
			message = "Topic of " + channel->get_name() + " is not set\r\n";
		else
			message = "Topic of " + channel->get_name() + " is: " + channel->get_topic() + "\r\n";
		sendSuccessMessage(client.get_client_fd(), message);
		return (0);
	}
	// check if topic_mode is true
	if (channel->get_topic_mode() == true && channel->find_client(client_nickname, "operators"))
	{
		std::string success = "Success[TOPIC]: topic changed to " + topic + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		channel->set_topic(topic);
	}// add else if for when client is operator but topic_mode is false
	else if (channel->get_topic_mode() == false && channel->find_client(client_nickname, "clients"))
	{
		std::string success = "Success[TOPIC]: topic changed to " + topic + "\r\n";
		sendSuccessMessage(client.get_client_fd(), success);
		channel->set_topic(topic);
	}
	else if (!channel->find_client(client_nickname, "operators"))
	{
		// Check if client is administrator
		std::string error = "Error[TOPIC]: " + client.get_nickname() + " is not an operator in channel " + channel_to_find + "\r\n";
		sendErrorMessage(client.get_client_fd(), error);
		return (-1);
	}	
	return (0);
}

/* cmd_invite: invite client to channel (INVITE <nickname> <channel>)
 * 1. Parse input into channel name and nickname
 * 2. Check if client is administrator
 * 3. Find the channel
 * 4. Find the nickname in the channel
 * 5. Invite the user
 */
int Server::cmd_invite(Client &client, std::string input)
{
	std::istringstream iss(input);
	std::string channel_to_find;
	std::string nickname;
	
	// Parse input
	iss >> nickname;
	iss >> channel_to_find;

	std::cout << "nickname: " << nickname << std::endl; // debug
	std::cout << "channel_to_find: " << channel_to_find << std::endl; // debug
	// if nickname is empty or channel is empty
	if (nickname.empty() || channel_to_find.empty())
	{
		std::string message = "Error[INVITE]: INVITE <nickname> <channel>\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find the channel
	Channel *channel = findChannel(client, channel_to_find);
	if (!channel)
	{
		std::string message = "Error[INVITE]: channel " + channel_to_find + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find if Client is in _clients_operator_channel
	std::string client_nickname = client.get_nickname();
	if (!channel->find_client(client_nickname, "operators"))
	{
		std::string message = "Error[INVITE]: You (" + client.get_nickname() + ") are not an operator in channel " + channel->get_name() + "\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// find if nickname is belongs to client that is already on _clients_invited_to_channel
	if(channel->find_client(nickname, "invited"))
	{
		std::string message = "Error[INVITE]: " + nickname + " is already invited to the channel" + channel->get_name() + "\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Find the client to invite
	Client *client_to_invite = find_client(client, nickname);
	if (!client_to_invite)
	{
		std::string message = "Error[INVITE]: user with nickname " + nickname + " does not exist\r\n";
		sendErrorMessage(client.get_client_fd(), message);
		return (1);
	}
	// Invite the user
	channel->add_client_to_clients_invited_vector(*client_to_invite);
	// Send message to client
	std::string message = "[INVITE] " + client.get_nickname() + " has invited you to join " + channel->get_name() + "\r\n";
	sendSuccessMessage(client_to_invite->get_client_fd(), message);
	return (0);
}

/* sendSuccessMessage: send success message to client
 * 1. Send success message to client
 */
int Server::sendSuccessMessage(int client_fd, const std::string	&successMessage)
{
	if (send(client_fd, successMessage.c_str(), successMessage.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}

/* sendErrorMessage: send error message to client
 * 1. Send error message to client
 */
int Server::sendErrorMessage(int client_fd, const std::string	&errorMessage)
{
	if (send(client_fd, errorMessage.c_str(), errorMessage.size(), MSG_NOSIGNAL) == -1)
	{
		std::cerr << RED << "Error: " << RESET << "send() failed" << std::endl;
		return (-1);
	}
	return (0);
}
