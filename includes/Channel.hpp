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

};

#endif