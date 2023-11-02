#include "User.hpp"
#include "Handle_user.hpp"

std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		tokens.push_back(token);
	}

	tokens.push_back(s.substr(pos_start));
	return tokens;
}

bool authenticate_hexchat(int client_fd, const std::string& message, const std::string& password, User &user)
{
    std::vector<std::string> commands = split(message, "\r\n");

    bool found_pass = false, found_nick = false, found_user = false;

    for(size_t i = 0; i < commands.size(); ++i)
	{
		std::cout << "size:" << commands.size() << std::endl;
        if(commands[i].find("PASS") == 0)
		{
            found_pass = true;
			std::cout << "Command was|" << commands[i] << "|" << std::endl;
            if (!handle_pass(user, commands[i], password))
			{
                send(client_fd, "ERROR: Invalid password. Disconnecting.\r\n", 40, MSG_NOSIGNAL);
                close(client_fd);
                return (false);
            }
			std::cout << "SUCCESS: Password accepted!" << std::endl;
        }
		else if(commands[i].find("NICK") == 0)
		{
            found_nick = true;
			std::cout << "Command was: " << commands[i] << std::endl;
            if (!handle_nick(user, commands[i]))
			{
                send(client_fd, "ERROR: Nickname is invalid or already in use. Disconnecting.\r\n", 63, MSG_NOSIGNAL);
                close(client_fd);
                return (false);
            }
			std::cout << "SUCCESS: Nickname accepted!" << std::endl;
        }
		else if(commands[i].find("USER") == 0)
		{
            found_user = true;
			std::cout << "Command was: " << commands[i] << std::endl;
            if (!handle_user(user, commands[i]))
			{
                send(client_fd, "ERROR: invalid name. Usage: USER <name> <admin>. Disconnecting.\r\n", 68, MSG_NOSIGNAL);
                close(client_fd);
                return (false);
            }
			std::cout << "SUCCESS: User accepted!" << std::endl;
        }
		std::cout << "Command was: " << commands[i] << std::endl;
    }

    // Check if all necessary commands are present
    if(!found_pass || !found_nick || !found_user)
	{
        send(client_fd, "ERROR: Missing PASS, NICK or USER command. Disconnecting.\r\n", 59, MSG_NOSIGNAL);
        close(client_fd);
        return (false);
    }

    // If all commands are processed successfully
    if(user.has_authenticated && user.is_registered)
	{
        send(client_fd, "SUCCESS: You are fully authenticated!\r\n", 40, MSG_NOSIGNAL);
        return (true);
    }

    return (false);
}


