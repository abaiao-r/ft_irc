#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Ft_irc.hpp"
#include "User.hpp"

class Channel
{
	public:
		std::string name;
		User admin; 
		std::vector<User> users_in_channel;
		std::string topic;
		std::string password;
		bool					is_private;
		bool					is_invite_only;
		bool					is_topic_settable;
		long unsigned int		limit;
		std::vector<std::string> ban_list;
		std::vector<std::string> invite_list;

};

#endif