#include "Commands.hpp"
#include <map>
#include <vector>
#include <sys/socket.h>

std::map<std::string, std::vector<std::string> > user_messages;  // Messages associated with usernames

bool handle_join(User& user)
{
	// Expecting format: JOIN #channel
    // Check if user is already in the chat
    for (size_t i = 0; i < users.size(); i++)
	{
        if (users[i].nickname == user.nickname)
            return (false);
    }
    users.push_back(user);
    return true;
}


bool handle_msg(User& user, const std::string& message)
{
	// Expecting format: MSG <message>
	std::string actual_msg = message.substr(4); // Strip off the "MSG " part
	user_messages[user.nickname].push_back(actual_msg);

	// Broadcast this message to all users
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].fd != user.fd) // Don't send to the sender
		{
			std::string broadcast_msg = user.nickname + ": " + actual_msg + "\r\n";
			send(users[i].fd, broadcast_msg.c_str(), broadcast_msg.length(), MSG_NOSIGNAL);
		}
	}
	return true;
}


bool handle_privmsg(User& user, const std::string& message)
{
	// Expecting format: PRIVMSG <recipient> <message>
    size_t space_pos = message.find(' ', 8);  // Find space after "PRIVMSG "
    if (space_pos == std::string::npos)
        return (false);

    std::string recipient_nick = message.substr(8, space_pos - 8);
    std::string actual_msg = message.substr(space_pos + 1);

    for (size_t i = 0; i < users.size(); i++)
	{
        if (users[i].nickname == recipient_nick)
		{
            std::string pm_msg = "Private from " + user.nickname + ": " + actual_msg + "\r\n";
            send(users[i].fd, pm_msg.c_str(), pm_msg.length(), MSG_NOSIGNAL);
            return (true);
        }
    }
    return (false);
}
