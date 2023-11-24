/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gacorrei <gacorrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 15:58:00 by abaiao-r          #+#    #+#             */
/*   Updated: 2023/11/24 08:26:45 by gacorrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <map>
#include "colours.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "utils.hpp"

class Client;
class Channel;

#define C_IT std::vector<Client>::iterator
#define CH_IT std::vector<Channel>::iterator
#define SOCKLEN sizeof(struct sockaddr_in)
#define MAX_REQUESTS 5
#define MAX_EVENTS 1024
#define MAX_RETRIES 3
#define BUFFER_READ_SIZE 1024
#define MAX_LEN 10
#define MIN_LEN 3

// Numerics
// https://modern.ircdocs.horse/#numerics
#define RPL_WELCOME (std::string)"001"
#define RPL_YOURHOST (std::string)"002"
#define RPL_CREATED (std::string)"003"
#define RPL_MYINFO (std::string)"004"
#define RPL_ISUPPORT (std::string)"005"
#define RPL_BOUNCE (std::string)"010"
#define RPL_STATSCOMMANDS (std::string)"212"
#define RPL_ENDOFSTATS (std::string)"219"
#define RPL_UMODEIS (std::string)"221"
#define RPL_STATSUPTIME (std::string)"242"
#define RPL_LUSERCLIENT (std::string)"251"
#define RPL_LUSEROP (std::string)"252"
#define RPL_LUSERUNKNOWN (std::string)"253"
#define RPL_LUSERCHANNELS (std::string)"254"
#define RPL_LUSERME (std::string)"255"
#define RPL_ADMINME (std::string)"256"
#define RPL_ADMINLOC1 (std::string)"257"
#define RPL_ADMINLOC2 (std::string)"258"
#define RPL_ADMINEMAIL (std::string)"259"
#define RPL_TRYAGAIN (std::string)"263"
#define RPL_LOCALUSERS (std::string)"265"
#define RPL_GLOBALUSERS (std::string)"266"
#define RPL_WHOISCERTFP (std::string)"276"
#define RPL_NONE (std::string)"300"
#define RPL_AWAY (std::string)"301"
#define RPL_USERHOST (std::string)"302"
#define RPL_UNAWAY (std::string)"305"
#define RPL_NOWAWAY (std::string)"306"
#define RPL_WHOISREGNICK (std::string)"307"
#define RPL_WHOISUSER (std::string)"311"
#define RPL_WHOISSERVER (std::string)"312"
#define RPL_WHOISOPERATOR (std::string)"313"
#define RPL_WHOWASUSER (std::string)"314"
#define RPL_ENDOFWHO (std::string)"315"
#define RPL_WHOISIDLE (std::string)"317"
#define RPL_ENDOFWHOIS (std::string)"318"
#define RPL_WHOISCHANNELS (std::string)"319"
#define RPL_WHOISSPECIAL (std::string)"320"
#define RPL_LISTSTART (std::string)"321"
#define RPL_LIST (std::string)"322"
#define RPL_LISTEND (std::string)"323"
#define RPL_CHANNELMODEIS (std::string)"324"
#define RPL_CREATIONTIME (std::string)"329"
#define RPL_WHOISACCOUNT (std::string)"330"
#define RPL_NOTOPIC (std::string)"331"
#define RPL_TOPIC (std::string)"332"
#define RPL_TOPICWHOTIME (std::string)"333"
#define RPL_INVITELIST (std::string)"336"
#define RPL_ENDOFINVITELIST (std::string)"337"
#define RPL_WHOISACTUALLY (std::string)"338"
#define RPL_INVITING (std::string)"341"
#define RPL_INVEXLIST (std::string)"346"
#define RPL_ENDOFINVEXLIST (std::string)"347"
#define RPL_EXCEPTLIST (std::string)"348"
#define RPL_ENDOFEXCEPTLIST (std::string)"349"
#define RPL_VERSION (std::string)"351"
#define RPL_WHOREPLY (std::string)"352"
#define RPL_NAMREPLY (std::string)"353"
#define RPL_LINKS (std::string)"364"
#define RPL_ENDOFLINKS (std::string)"365"
#define RPL_ENDOFNAMES (std::string)"366"
#define RPL_BANLIST (std::string)"367"
#define RPL_ENDOFBANLIST (std::string)"368"
#define RPL_ENDOFWHOWAS (std::string)"369"
#define RPL_INFO (std::string)"371"
#define RPL_MOTD (std::string)"372"
#define RPL_ENDOFINFO (std::string)"374"
#define RPL_MOTDSTART (std::string)"375"
#define RPL_ENDOFMOTD (std::string)"376"
#define RPL_WHOISHOST (std::string)"378"
#define RPL_WHOISMODES (std::string)"379"
#define RPL_YOUREOPER (std::string)"381"
#define RPL_REHASHING (std::string)"382"
#define RPL_TIME (std::string)"391"
#define ERR_UNKNOWNERROR (std::string)"400"
#define ERR_NOSUCHNICK (std::string)"401"
#define ERR_NOSUCHSERVER (std::string)"402"
#define ERR_NOSUCHCHANNEL (std::string)"403"
#define ERR_CANNOTSENDTOCHAN (std::string)"404"
#define ERR_TOOMANYCHANNELS (std::string)"405"
#define ERR_WASNOSUCHNICK (std::string)"406"
#define ERR_NOORIGIN (std::string)"409"
#define ERR_NORECIPIENT (std::string)"411"
#define ERR_NOTEXTTOSEND (std::string)"412"
#define ERR_INPUTTOOLONG (std::string)"417"
#define ERR_UNKNOWNCOMMAND (std::string)"421"
#define ERR_NOMOTD (std::string)"422"
#define ERR_NONICKNAMEGIVEN (std::string)"431"
#define ERR_ERRONEUSNICKNAME (std::string)"432"
#define ERR_NICKNAMEINUSE (std::string)"433"
#define ERR_NICKCOLLISION (std::string)"436"
#define ERR_USERNOTINCHANNEL (std::string)"441"
#define ERR_NOTONCHANNEL (std::string)"442"
#define ERR_USERONCHANNEL (std::string)"443"
#define ERR_NOTREGISTERED (std::string)"451"
#define ERR_NEEDMOREPARAMS (std::string)"461"
#define ERR_ALREADYREGISTERED (std::string)"462"
#define ERR_PASSWDMISMATCH (std::string)"464"
#define ERR_YOUREBANNEDCREEP (std::string)"465"
#define ERR_CHANNELISFULL (std::string)"471"
#define ERR_UNKNOWNMODE (std::string)"472"
#define ERR_INVITEONLYCHAN (std::string)"473"
#define ERR_BANNEDFROMCHAN (std::string)"474"
#define ERR_BADCHANNELKEY (std::string)"475"
#define ERR_BADCHANMASK (std::string)"476"
#define ERR_NOPRIVILEGES (std::string)"481"
#define ERR_CHANOPRIVSNEEDED (std::string)"482"
#define ERR_CANTKILLSERVER (std::string)"483"
#define ERR_NOOPERHOST (std::string)"491"
#define ERR_UMODEUNKNOWNFLAG (std::string)"501"
#define ERR_USERSDONTMATCH (std::string)"502"
#define ERR_HELPNOTFOUND (std::string)"524"
#define ERR_INVALIDKEY (std::string)"525"
#define RPL_STARTTLS (std::string)"670"
#define RPL_WHOISSECURE (std::string)"671"
#define ERR_STARTTLS (std::string)"691"
#define ERR_INVALIDMODEPARAM (std::string)"696"
#define RPL_HELPSTART (std::string)"704"
#define RPL_HELPTXT (std::string)"705"
#define RPL_ENDOFHELP (std::string)"706"
#define ERR_NOPRIVS (std::string)"723"
#define RPL_LOGGEDIN (std::string)"900"
#define RPL_LOGGEDOUT (std::string)"901"
#define ERR_NICKLOCKED (std::string)"902"
#define RPL_SASLSUCCESS (std::string)"903"
#define ERR_SASLFAIL (std::string)"904"
#define ERR_SASLTOOLONG (std::string)"905"
#define ERR_SASLABORTED (std::string)"906"
#define ERR_SASLALREADY (std::string)"907"
#define RPL_SASLMECHS (std::string)"908"
class Server
{
	private:
		int							_port;
		const std::string			_password;
		int							_server_fd;
		sockaddr_in					_address;
		int							_epoll_fd;
		epoll_event					_main_event;
		epoll_event					_events[MAX_EVENTS];
		std::vector<Client>			_clients;
		std::vector<Channel>		_channels;
		static int					_loop_state;

		// private because we don't want to allow copies of this class
		Server(void);
		Server(const Server &src);
		Server &operator=(const Server &src);


	public:
		Server(int port, std::string password);
		~Server();

		//Startup
		void		init_server(void);
		int			create_socket(void);
		int			unblock_socket(int fd);
		int			set_socket_options(void);
		int			bind_socket(void);
		int			start_listening(void);
		void		create_epoll();

		//Connection loop and login
		void		connection();
		void		client_connection();
		int 		client_cmds(Client &client);
		int			choose_cmd(Client &client, std::string in);
		void 		login(Client &client, const std::string &buffer);
		void 		parseLoginLine(const std::string &line, std::map<std::string, std::string> &cmds);

		//Commands and messages
		void		cmd_pass(Client &client, std::string input);
		void		cmd_user(Client &client, std::string input);
		void		cmd_nick(Client &client, std::string input);
		int			cmd_join(Client &client, std::string input);
		void		cmd_privmsg(Client &client, std::string input);
		int 		cmd_list(Client &client, std::string input);
		int 		cmd_kick(Client &client, std::string input);
		int 		cmd_invite(Client &client, std::string input);
		int			cmd_topic(Client &client, std::string input);
		int			cmd_mode(Client &client, std::string input);
		int			cmd_who(Client &client, std::string input);
		int			sendErrorMessage(int client_fd, const std::string& errorMessage);
		int 		sendSuccessMessage(int client_fd, const std::string	&successMessage);

		//Utils
		int			is_client_admin(Client &client);
		Channel		*findChannel(Client &client, const std::string& channelName);
		Client		*find_client(Client &client, const std::string& nickname);
		int 		kickClientFromChannel(Channel* channel, Client* client, Client *client_to_kick, const std::string& reason);
		int 		channel_name_validation(int client_fd, std::string check);
		bool		pass_validation(std::string check) const;
		bool		name_validation(std::string check);
		int			nick_validation(std::string check);
		int			name_compare(std::string check, std::string comp);
		int			password_checker(std::string password);
		int			password_checker(std::string password, int fd);
		std::string	get_users_string(Channel &channel);
		void		disconnect_client(int fd);
		void		leave_all_rooms(int fd);
		static void	signal_handler(int sig);
};
