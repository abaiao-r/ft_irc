/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 10:45:28 by gacorrei          #+#    #+#             */
/*   Updated: 2023/12/06 13:50:06 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <map>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "ServerUtils.hpp"
#include "Numerics.hpp"

#define BUFFER_READ_SIZE 1024
#define MAX_LEN 10
#define MIN_LEN 3

class ServerCommands
{
	private:
		ServerCommands(const ServerCommands &copy);
		ServerCommands &operator=(const ServerCommands &copy);
	public:
		std::vector<Client>		_clients;
		std::vector<Channel>	_channels;
		Bot						_Clippy;
		ServerCommands();
		virtual			~ServerCommands();
		int 			client_cmds(Client &client);
		int				choose_cmd(Client &client, std::string in);
		void 			login(Client &client, const std::string &buffer);
		void 			parseLoginLine(const std::string &line, std::map<std::string, std::string> &cmds);
		//Commands and messages
		void			cmd_pass(Client &client, std::string input);
		void			cmd_user(Client &client, std::string input);
		void			cmd_nick(Client &client, std::string input);
		/* JOIN funtions*/
		bool 			checkBanned(Client &client, const std::string &input_channel_name, CH_IT &it, int fd);
		bool			checkPassword(const std::string &input_channel_name, const std::string &input_password, CH_IT &it, int fd);
		bool			checkInviteOnly(Client &client, const std::string &input_channel_name, CH_IT &it, int fd);
		bool			checkChannelFull(const std::string &input_channel_name, CH_IT &it, int fd);
		bool			validateJoinConditions(Client &client, int fd, const std::string &input_channel_name, const std::string &input_password, CH_IT &it);
		bool			checkIfClientAlreadyInChannel(Client &client, const std::string &input_channel_name, CH_IT &it, int fd);
		bool			createAndJoinChannel(Client &client, const std::string &input_channel_name, CH_IT &it, int fd);
		bool			checkClientRegistration(Client &client, int fd);
		bool			validateJoinPreconditions(Client &client, int fd, const std::string &input_channel_name, CH_IT &it);
		int				cmd_join(Client &client, std::string input);
		/*PRIVMSG funtions*/
		void			cmd_privmsg(Client &client, std::string input);
		/*LIST funtions*/
		std::vector<std::string>	parseArgsListCommand(std::string input);
		int 			cmdListNoArgs(Client &client);
		int 			cmdListWithArg(Client &client, std::string input);
		int 			cmd_list(Client &client, std::string input);
		/* WHO funtions*/
		void 			sendWhoReplyMessages(Client &client, Channel &channel);
		int				cmd_who(Client &client, std::string input);
		/*KICK funtions*/
		int 			kickClientFromChannel(Channel* channel, Client* client, Client *client_to_kick, const std::string& reason);
		int				kickClientFromChannel(Channel *channel, Bot &bot, Client *client_to_kick, const std::string &reason);
		void 			parseKickCommand(std::istringstream &iss, std::string &channel_to_find, std::string &nickname, std::string &reason);
		int 			performChecks(Client &client, const std::string &channel_to_find, std::string &nickname, Channel *&channel, Client *&client_to_kick);
		int 			cmd_kick(Client &client, std::string input);
		/* INVITE funtions*/
		int				handleInviteErrors(Client &client, const std::string &channel_to_find, const std::string &nickname, Channel *&channel);
		int				cmd_invite(Client &client, std::string input);
		/*TOPIC funtions*/
		int 			handleTopicCommand(Client &client, Channel *&channel, const std::string &topic);
		int 			handleTopicErrors(Client &client, const std::string &channel_to_find, Channel *&channel);
		void 			parseTopicCommand(std::istringstream &iss, std::string &channel_to_find, std::string &topic);
		int				cmd_topic(Client &client, std::string input);
		/*mode funtions*/
		int 			handleModeMinusL(Channel *channel, int fd);
		int 			handleModePlusL(Channel *channel, std::string argument, int fd);
		int 			handleModeMinusT(Channel *channel, int fd);
		int 			handleModePlusT(Channel *channel, int fd);
		int 			handleModeMinusI(Channel *channel, int fd);
		int 			handleModePlusI(Channel *channel, int fd);
		int 			handleModeMinusK(Channel *channel, int fd);
		int 			handleModePlusK(Channel *channel, std::string argument, int fd);
		int 			handleModeMinusO(Client &client, Channel *channel, std::string argument, int fd);
		int 			handleModePlusO(Client &client, Channel *channel, std::string argument, int fd);
		int 			handleMode(Client &client, Channel *channel, const std::string &mode, const std::string &argument, int fd);
		int 			sendOperatorPrivilegeError(int fd, const std::string &channel);
		int 			sendChannelModeInfo(int fd, Channel *channel, const std::string &channelName);
		int 			sendChannelNotFoundError(int fd, const std::string &channel);
		void 			printDebugInfo(const std::string &channel, const std::string &mode, const std::string &argument);
		int				cmd_mode(Client &client, std::string input);
		/*PART funtions*/
		std::vector<std::string>	getPartVector(std::string input);
		void			cmd_part(Client &client, std::string input);
		//Utils
		int 			sendMessage(int client_fd, const std::string &msg);
		void			sendChannelUserListMessage(Channel *channel, const std::string &argument);
		int				is_client_admin(Client &client);
		Channel			*findChannel(Client &client, const std::string& channelName);
		Client			*find_client(Client &client, const std::string& nickname);
		int 			channel_name_validation(int client_fd, std::string check);
		virtual bool	pass_validation(std::string check) const = 0;
		bool			name_validation(std::string check);
		int				nick_validation(std::string check);
		int				name_compare(std::string check, std::string comp);
		int				password_checker(std::string password, int fd);
		std::string		get_users_string(Channel &channel);
		void			disconnect_client(int fd);
		void			leave_all_rooms(Client &client);
		void			join_messages(Client &client, Channel &channel);
};
