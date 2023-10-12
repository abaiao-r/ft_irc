#include "ft_irc.hpp"

bool handle_nick(User& user, const std::string& message)
{
    // Extract nickname from message
    // Format: NICK <nickname>
    size_t space_pos = message.find(' ');
    if(space_pos == std::string::npos || space_pos + 1 == message.size())
	{
        // Invalid message format
        return false;
    }
    std::string nickname = message.substr(space_pos + 1);

	//check if nickname already in user
    // If it is, return false
	if(user.nickname == nickname)
		return false;
	
    user.nickname = nickname;
    return true;
}

bool handle_user(User& user, const std::string& message)
{
    // Extract user details from message
    // Format: USER <username> <hostname> <servername> :<realname>
    size_t space_pos = message.find(' ');
    // Invalid message format
    if(space_pos == std::string::npos || space_pos + 1 == message.size())
        return false;
    std::string rest_of_message = message.substr(space_pos + 1);
    size_t colon_pos = rest_of_message.find(':');
    // Invalid message format
    if(colon_pos == std::string::npos)
        return (false);

    // Extracting and setting username and realname
    std::string username = rest_of_message.substr(0, rest_of_message.find(' '));
    std::string realname = rest_of_message.substr(colon_pos + 1);

    user.username = username;
    user.realname = realname;
    return (true);
}

bool handle_pass(User& user, const std::string& message, const std::string& server_password)
{
    // Extract password from message
    // Format: PASS <password>
    size_t space_pos = message.find(' ');
    // Invalid message format
    if(space_pos == std::string::npos || space_pos + 1 == message.size())
        return false;
	
    std::string provided_password = message.substr(space_pos + 1);
	provided_password.resize(provided_password.size() - 1); //Delete new line

    if(provided_password.compare(server_password) == 0)
	{
		std::cout << "Passwords match!" << std::endl;
        user.has_authenticated = true;
        return (true);
    }
    return (false);
}