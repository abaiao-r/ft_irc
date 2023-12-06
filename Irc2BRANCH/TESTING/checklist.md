
checklist for ft_irc

initiliaze the server:

- write the name of the program without arguments
```
./ircserv
```
- write the name of the program with  only one argument
```
./ircserv 6697
```
- write the name of the program with  four arguments
```
./ircserv 6697 pass qwwq
```
- write the name of the program with  three arguments but port is not a number
```
./ircserv hello pass
```
- write the name of the program with  three arguments but port is not a integer
```
./ircserv 6697.5 pass
```
```
./ircserv 99999999999999999999999999999 pass
```
- write the name of the program with  three arguments but port in under 1024 or over 49151
```
./ircserv 1023 pass
```
```
./ircserv 49152 pass
```
- write the name of the program with the correct arguments
```
./ircserv 6697 pass
```

connection with the server:

- test the connection with the server with a wrong port
```
nc localhost 6698
```
- test the connection with the server with a right port
```
nc localhost 6697
```

After the connection with the server:

- test the control + c : the server must be closed
```
ctrl + c
```

commands:

PASS:

    Test Case 1: Already Authenticated
        Set client to be authenticated.
        Call cmd_pass with any input.
        Verify that an error message is sent indicating that the client is already authenticated.

    Test Case 2: Valid Password
        Set client to not be authenticated.
        Call cmd_pass with a valid password (meeting the specified validation criteria).
        Verify that a success message is sent indicating that the client is now authenticated.
        Verify that the client's authentication status is set to true.

    Test Case 3: Password Validation Failure
        Set client to not be authenticated.
        Call cmd_pass with an invalid password.
        Verify that an error message is sent indicating that the password is incorrect.
        Verify that the client's password attempt counter is incremented.

    Test Case 4: Too Many Wrong Attempts
        Set client to not be authenticated.
        Call cmd_pass with an incorrect password multiple times, exceeding the allowed attempts.
        Verify that an error message is sent indicating too many wrong attempts.
        Verify that the client is disconnected.

    Test Case 5: Password Validation Failure Counter Reset
        Set client to not be authenticated.
        Call cmd_pass with an incorrect password multiple times, but not exceeding the allowed attempts.
        Verify that the error message indicates a wrong password each time.
        Verify that the client's password attempt counter is incremented for each wrong attempt.

    Test Case 6: Combined Validation Failure
        Set client to be authenticated.
        Call cmd_pass with an invalid password.
        Verify that an error message is sent indicating that the client is already authenticated.
        Verify that the authentication status remains unchanged.

USER:
    Test Case 1: Already Registered
        Set client to be registered.
        Call cmd_user with any input.
        Verify that an error message is sent indicating that the client is already registered.

    Test Case 2: Not Authenticated
        Set client to be authenticated.
        Set client to not be registered.
        Call cmd_user with any input.
        Verify that an error message is sent indicating that the client is not authenticated.

    Test Case 3: Already Have Username
        Set client to not be registered.
        Set client to have a non-empty username.
        Call cmd_user with any input.
        Verify that an error message is sent indicating that the client already has a username.

    Test Case 4: Valid Username
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a username.
        Call cmd_user with a valid username (meeting the specified criteria).
        Verify that a success message is sent indicating that the username is set.
        Verify that if the client also has a nickname, a success message is sent indicating successful registration.

    Test Case 5: Invalid Username - Spaces and Symbols
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a username.
        Call cmd_user with a username containing spaces or symbols other than [-_[]{}\|].
        Verify that an error message is sent indicating that the username can't have spaces or invalid symbols.

    Test Case 6: Invalid Username - Length and Starting Characters
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a username.
        Call cmd_user with a username that doesn't meet the length and starting character criteria.
        Verify that an error message is sent indicating the specified criteria for a valid username.

    Test Case 7: Combined Validation Failure
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a username.
        Call cmd_user with a username that fails multiple validation criteria.
        Verify that an error message is sent indicating all the relevant validation failures.

NICK

    Test Case 1: Already Registered
        Set client to be registered.
        Call cmd_nick with any input.
        Verify that an error message is sent indicating that the client is already registered.

    Test Case 2: Not Authenticated
        Set client to not be authenticated.
        Set client to not be registered.
        Call cmd_nick with any input.
        Verify that an error message is sent indicating that the client is not authenticated.

    Test Case 3: Already Have Nickname
        Set client to not be registered.
        Set client to be authenticated.
        Set client to have a non-empty nickname.
        Call cmd_nick with any input.
        Verify that an error message is sent indicating that the client already has a nickname.

    Test Case 4: Valid Nickname
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a nickname.
        Call cmd_nick with a valid nickname (meeting the specified criteria).
        Verify that a success message is sent indicating that the nickname is set.
        Verify that if the client also has a username, a success message is sent indicating successful registration.

    Test Case 5: Invalid Nickname - Spaces and Symbols
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a nickname.
        Call cmd_nick with a nickname containing spaces or symbols other than [-_[]{}\|].
        Verify that an error message is sent indicating that the nickname can't have spaces or invalid symbols.

    Test Case 6: Invalid Nickname - Length and Starting Characters
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a nickname.
        Call cmd_nick with a nickname that doesn't meet the length and starting character criteria.
        Verify that an error message is sent indicating the specified criteria for a valid nickname.

    Test Case 7: Nickname In Use
        Set client to not be registered.
        Set client to be authenticated.
        Set client to not have a nickname.
        Call cmd_nick with a nickname that is already in use.
        Verify that an error message is sent indicating that the nickname is already in use.

    Test Case 8: Combined Validation Failure
        Set client to be registered.
        Set client to not be authenticated.
        Set client to have a non-empty nickname.
        Call cmd_nick with input that fails multiple validation criteria.
        Verify that an error message is sent indicating all relevant validation failures.

JOIN

    Test Case 1: Client Not Registered
        Set client to not be registered.
        Call cmd_join with any input.
        Verify that an error message is sent indicating that the client must be registered.

    Test Case 2: Channel Does Not Exist
        Set client to be registered.
        Call cmd_join with a channel name that doesn't exist.
        Verify that a new channel is created, and the client is added to it.
        Verify that the JOIN message is sent to the client.

    Test Case 3: Channel Exists
        Set client to be registered.
        Create a channel.
        Call cmd_join with the existing channel name.
        Verify that the client is added to the existing channel.
        Verify that the JOIN message is sent to the client.

    Test Case 4: Client Already in Channel
        Set client to be registered.
        Create a channel.
        Add the client to the channel.
        Call cmd_join with the same channel name.
        Verify that an error message is sent indicating that the client is already in the channel.

    Test Case 5: Channel Full
        Set client to be registered.
        Create a channel with a limited number of clients.
        Add the maximum number of clients to the channel.
        Call cmd_join with the same channel name.
        Verify that an error message is sent indicating that the channel is full.

    Test Case 6: Invite Only Channel - Client Not Invited
        Set client to be registered.
        Create an invite-only channel without inviting the client.
        Call cmd_join with the invite-only channel name.
        Verify that an error message is sent indicating that the channel is invite-only, and the client is not invited.

    Test Case 7: Invite Only Channel - Client Invited
        Set client to be registered.
        Create an invite-only channel and invite the client.
        Call cmd_join with the invite-only channel name.
        Verify that the client is added to the channel.
        Verify that the JOIN message is sent to the client.

    Test Case 8: Password-Protected Channel - Incorrect Password
        Set client to be registered.
        Create a password-protected channel with a password.
        Call cmd_join with the password-protected channel name and an incorrect password.
        Verify that an error message is sent indicating that the password is incorrect.

    Test Case 9: Password-Protected Channel - Correct Password
        Set client to be registered.
        Create a password-protected channel with a password.
        Call cmd_join with the password-protected channel name and the correct password.
        Verify that the client is added to the channel.
        Verify that the JOIN message is sent to the client.

    Test Case 10: Banned Client
        Set client to be registered.
        Create a channel.
        Ban the client from the channel.
        Call cmd_join with the channel name.
        Verify that an error message is sent indicating that the client is banned.

PRIVMSG

    Test Case 1: Not Registered
        Set client to not be registered.
        Call cmd_privmsg with any input.
        Verify that an error message is sent indicating that the client must be registered.

    Test Case 2: Invalid Usage
        Set client to be registered.
        Call cmd_privmsg with invalid usage (e.g., missing destination or message).
        Verify that an error message is sent indicating the correct usage.

    Test Case 3: Channel Destination - Channel Does Not Exist
        Set client to be registered.
        Call cmd_privmsg with a channel destination that does not exist.
        Verify that an error message is sent indicating that the destination channel could not be found.

    Test Case 4: Channel Destination - Client Not in Channel
        Set client to be registered.
        Create a channel.
        Call cmd_privmsg with a channel destination, but the client is not in the channel.
        Verify that an error message is sent indicating that the client is not in the channel.

    Test Case 5: Channel Destination - Client Banned
        Set client to be registered.
        Create a channel and add the client to it.
        Ban the client from the channel.
        Call cmd_privmsg with a channel destination.
        Verify that an error message is sent indicating that the client is banned from the channel.

    Test Case 6: Channel Destination - Message Sent Successfully
        Set client to be registered.
        Create a channel and add the client to it.
        Call cmd_privmsg with a channel destination and a valid message.
        Verify that the message is sent successfully to the channel.

    Test Case 7: Channel Destination - Profanity Check
        Set client to be registered.
        Create a channel and add the client to it.
        Call cmd_privmsg with a channel destination and a message containing profanity.
        Verify that the message is not sent, and the client is kicked from the channel.

    Test Case 8: Private Message to Non-Existent Client
        Set client to be registered.
        Call cmd_privmsg with a private message destination that does not exist.
        Verify that an error message is sent indicating that the destination client could not be found.

    Test Case 9: Private Message to Existing Client
        Set client to be registered.
        Create another client.
        Call cmd_privmsg with a private message destination (the other client) and a valid message.
        Verify that the private message is sent successfully.

PART

    Test Case 1: Missing Parameters
        Call cmd_part with an empty or insufficient input.
        Verify that an error message is sent indicating the correct usage.

    Test Case 2: Non-Existent Channel
        Set client to be registered and in a channel.
        Call cmd_part with a channel that does not exist.
        Verify that an error message is sent indicating that the channel does not exist.

    Test Case 3: Not in the Channel
        Set client to be registered but not in any channels.
        Create a channel.
        Call cmd_part with the channel name.
        Verify that an error message is sent indicating that the client is not in the channel.

    Test Case 4: Successful Part
        Set client to be registered and in a channel.
        Call cmd_part with a valid channel and a reason.
        Verify that the client is removed from the channel.
        Verify that the part message is sent to the client and the channel.
        Verify that the QUIT message is sent to the channel.
        Verify that the client is removed from the channel's client list.
        Verify that the channel is removed if it becomes empty.

    Test Case 5: Multiple Channels
        Set client to be registered and in multiple channels.
        Call cmd_part with multiple channels and a reason.
        Verify that the client is removed from all specified channels.
        Verify that the part messages are sent to the clients and channels.

    Test Case 6: Channel Operators
        Set client to be a channel operator in a channel.
        Call cmd_part with the channel name.
        Verify that the client is removed from the channel's operators list.

    Test Case 7: Last Client Leaving Channel
        Set client to be the last client in a channel.
        Call cmd_part with the channel name.
        Verify that the channel is removed after the client leaves.

    Test Case 8: Invalid Part Vector
        Call getPartVector with various invalid inputs (missing parameters, invalid format).
        Verify that the returned vector is empty or contains the correct values.

    Test Case 9: Special Characters in Reason
        Set client to be registered and in a channel.
        Call cmd_part with a valid channel and a reason containing special characters.
        Verify that the part message is sent correctly.

    Test Case 10: No Reason Provided
        Set client to be registered and in a channel.
        Call cmd_part with a valid channel and no reason.
        Verify that the part message is sent without a reason.

TOPIC

    Test Case 1: Missing Parameters
        Call cmd_topic with an empty or insufficient input.
        Verify that an error message is sent indicating the correct usage.

    Test Case 2: Non-Existent Channel
        Set client to be registered.
        Call cmd_topic with a channel that does not exist.
        Verify that an error message is sent indicating that the channel does not exist.

    Test Case 3: Client Not an Operator
        Set client to be registered but not an operator in any channels.
        Create a channel.
        Call cmd_topic with the channel and a topic.
        Verify that an error message is sent indicating that the client is not an operator in the channel.

    Test Case 4: Successful Topic Set by Operator
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_topic with the channel and a topic.
        Verify that the topic is set successfully.

    Test Case 5: Successful Topic Set by Non-Operator Client
        Set client to be registered but not an operator in any channels.
        Create a channel.
        Add client to the channel.
        Call cmd_topic with the channel and a topic.
        Verify that the topic is set successfully.

    Test Case 6: Successful Topic Set with Leading Whitespace
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_topic with the channel and a topic that has leading whitespace.
        Verify that the topic is set successfully.

    Test Case 7: Successful Topic Set with Special Characters
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_topic with the channel and a topic that contains special characters.
        Verify that the topic is set successfully.

    Test Case 8: Successful Topic Set to Empty String
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_topic with the channel and an empty string as the topic.
        Verify that the topic is set to an empty string successfully.

    Test Case 9: Successful Topic Set with Leading Colon
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_topic with the channel and a topic that starts with a colon.
        Verify that the topic is set successfully.

MODE

    Test Case 1: Missing Parameters
        Call cmd_mode with an empty or insufficient input.
        Verify that an error message is sent indicating the correct usage.

    Test Case 2: Non-Existent Channel
        Set client to be registered.
        Call cmd_mode with a channel that does not exist.
        Verify that an error message is sent indicating that the channel does not exist.

    Test Case 3: Client Not an Operator
        Set client to be registered but not an operator in any channels.
        Create a channel.
        Call cmd_mode with the channel and a mode change.
        Verify that an error message is sent indicating that the client is not an operator in the channel.

    Test Case 4: Successful Channel Mode Info Request
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel but without specifying a mode.
        Verify that the channel mode information is sent successfully.

    Test Case 5: Unknown Mode
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel and an unknown mode.
        Verify that an error message is sent indicating that the mode is unknown.

    Test Case 6: Successful Mode Change (+o)
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel, mode set to "+o," and the nickname of the client to be given operator status.
        Verify that the mode change is successful.

    Test Case 7: Successful Mode Change (-o)
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel, mode set to "-o," and the nickname of the client to be removed from operator status.
        Verify that the mode change is successful.

    Test Case 8: Successful Mode Change (+k)
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel, mode set to "+k," and a keyword as an argument.
        Verify that the mode change is successful.

    Test Case 9: Successful Mode Change (-k)
        Set client to be an operator in a channel.
        Create another client and add them to the same channel.
        Call cmd_mode with the channel, mode set to "-k," and no argument.
        Verify that the mode change is successful.

    Test Case 10: Invalid Mode Change (Wrong Argument)

    Set client to be an operator in a channel.
    Create another client and add them to the same channel.
    Call cmd_mode with the channel, an invalid mode, and an argument that doesn't match the mode requirements.
    Verify that an error message is sent indicating the correct usage.

MODE handle

    Test Case 1: Invalid Mode
        Call handleMode with an invalid mode (not one of +o, -o, +k, -k, +i, -i, +t, -t, +l, -l).
        Verify that the function returns 2, indicating an invalid mode.

    Test Case 2: Mode Plus O - Client Not in Channel
        Set up a channel and a registered client (client A).
        Call handleMode with the channel, mode "+o," and the nickname of a client that is not in the channel.
        Verify that an error message is sent indicating that the client is not in the channel.

    Test Case 3: Mode Plus O - Client Already an Operator
        Set up a channel and add a client (client A) to the channel as an operator.
        Call handleMode with the channel, mode "+o," and the nickname of client A.
        Verify that an error message is sent indicating that the client is already an operator.

    Test Case 4: Mode Plus O - Success
        Set up a channel and add a client (client A) to the channel.
        Call handleMode with the channel, mode "+o," and the nickname of client A.
        Verify that client A becomes an operator, and success messages are sent to the client and the channel.

    Test Case 5: Mode Minus O - Client Not in Channel
        Set up a channel and a registered client (client A).
        Call handleMode with the channel, mode "-o," and the nickname of a client that is not in the channel.
        Verify that an error message is sent indicating that the client is not in the channel.

    Test Case 6: Mode Minus O - Client Not an Operator
        Set up a channel and add a client (client A) to the channel.
        Call handleMode with the channel, mode "-o," and the nickname of client A.
        Verify that an error message is sent indicating that the client is not an operator.

    Test Case 7: Mode Minus O - Last Operator
        Set up a channel and add a client (client A) to the channel as the only operator.
        Call handleMode with the channel, mode "-o," and the nickname of client A.
        Verify that an error message is sent indicating that the client is the last operator.

    Test Case 8: Mode Minus O - Success
        Set up a channel and add two clients (client A and client B) to the channel, with client A as an operator.
        Call handleMode with the channel, mode "-o," and the nickname of client B.
        Verify that client B is removed as an operator, and success messages are sent to the client and the channel.

    Test Case 9: Mode Plus K - Channel Already Password Protected
        Set up a channel with a password.
        Call handleMode with the channel, mode "+k," and a password.
        Verify that an error message is sent indicating that the channel is already password protected.

    Test Case 10: Mode Plus K - Invalid Password
        Set up a channel.
        Call handleMode with the channel, mode "+k," and an invalid password.
        Verify that an error message is sent indicating that the password is invalid.

    Test Case 11: Mode Plus K - Success
        Set up a channel.
        Call handleMode with the channel, mode "+k," and a valid password.
        Verify that the channel is password protected, and a success message is sent to the channel.

    Test Case 12: Mode Minus K - Channel Not Password Protected
        Set up a channel without a password.
        Call handleMode with the channel, mode "-k."
        Verify that an error message is sent indicating that the channel is not password protected.

    Test Case 13: Mode Minus K - Success
        Set up a channel with a password.
        Call handleMode with the channel, mode "-k."
        Verify that the channel is no longer password protected, and a success message is sent to the channel.

    Test Case 14: Mode Plus I - Channel Already Invite Only
        Set up a channel as invite-only.
        Call handleMode with the channel, mode "+i."
        Verify that an error message is sent indicating that the channel is already invite-only.

    Test Case 15: Mode Plus I - Success
        Set up a channel.
        Call handleMode with the channel, mode "+i."
        Verify that the channel is now invite-only, and a success message is sent to the channel.

    Test Case 16: Mode Minus I - Channel Not Invite Only
        Set up a channel without invite-only.
        Call handleMode with the channel, mode "-i."
        Verify that an error message is sent indicating that the channel is not invite-only.

    Test Case 17: Mode Minus I - Success
        Set up a channel as invite-only.
        Call handleMode with the channel, mode "-i."
        Verify that the channel is no longer invite-only, and a success message is sent to the channel.

    Test Case 18: Mode Plus T - Channel Already Topic Protected
        Set up a channel as topic-protected.
        Call handleMode with the channel, mode "+t."
        Verify that an error message is sent indicating that the channel is already topic-protected.

    Test Case 19: Mode Plus T - Success
        Set up a channel.
        Call handleMode with the channel, mode "+t."
        Verify that the channel is now topic-protected, and a success message is sent to the channel.

    Test Case 20: Mode Minus T - Channel Not Topic Protected
        Set up a channel without topic protection.
        Call handleMode with the channel, mode "-t."
        Verify that an error message is sent indicating that the channel is not topic-protected.

    Test Case 21: Mode Minus T - Success
        Set up a channel as topic-protected.
        Call handleMode with the channel, mode "-t."
        Verify that the channel is no longer topic-protected, and a success message is sent to the channel.

    Test Case 22: Mode Plus L - Channel Already Limited
        Set up a channel with a user limit.
        Call handleMode with the channel, mode "+l," and a user limit.
        Verify that an error message is sent indicating that the channel is already limited.

    Test Case 23: Mode Plus L - No Limit Specified
        Set up a channel.
        Call handleMode with the channel, mode "+l," without specifying a limit.
        Verify that an error message is sent indicating that no limit is specified.

    Test Case 24: Mode Plus L - Invalid Limit (Not a Number)
        Set up a channel.
        Call handleMode with the channel, mode "+l," and an invalid limit (not a number).
        Verify that an error message is sent indicating that the limit is not a valid number.

    Test Case 25: Mode Plus L - Invalid Limit (Out of Integer Range)
        Set up a channel.
        Call handleMode with the channel, mode "+l," and an invalid limit (out of integer range).
        Verify that an error message is sent indicating that the limit is out of the integer range.

    Test Case 26: Mode Plus L - Success
        Set up a channel.
        Call handleMode with the channel, mode "+l," and a valid user limit.
        Verify that the channel now has a user limit, and a success message is sent to the channel.

    Test Case 27: Mode Minus L - Channel Not Limited
        Set up a channel without a user limit.
        Call handleMode with the channel, mode "-l."
        Verify that an error message is sent indicating that the channel is not limited.

    Test Case 28: Mode Minus L - Success
        Set up a channel with a user limit.
        Call handleMode with the channel, mode "-l."
        Verify that the channel no longer has a user limit, and a success message is sent to the channel.

WHO

    Test Case 1: No Argument Provided
        Call cmd_who with an empty string as the argument.
        Verify that an error message is sent indicating that no argument is provided.

    Test Case 2: Channel Does Not Exist
        Call cmd_who with the name of a channel that does not exist.
        Verify that an error message is sent indicating that the channel does not exist.

    Test Case 3: Channel Has Clients
        Set up a channel with multiple clients.
        Call cmd_who with the name of the channel.
        Verify that sendWhoReplyMessages is called for each client in the channel.
        Verify that RPL_WHOREPLY messages are sent for each client.
        Verify that the RPL_ENDOFWHO message is sent at the end.

    Test Case 4: Channel Has No Clients
        Set up an empty channel.
        Call cmd_who with the name of the channel.
        Verify that sendWhoReplyMessages is not called.
        Verify that the RPL_ENDOFWHO message is sent immediately.

    Test Case 5: Invalid Channel Name Format
        Call cmd_who with an invalid channel name format (e.g., containing spaces or special characters).
        Verify that an error message is sent indicating that the channel name is invalid.

    Test Case 6: Valid Channel Name Format
        Call cmd_who with a valid channel name format.
        Verify that the function executes successfully and sends the appropriate WHO messages.

    Test Case 7: Channel Name Case Sensitivity
        Set up a channel with a specific case (e.g., #Channel1).
        Call cmd_who with a different case (e.g., #channel1).
        Verify that the function recognizes the case-insensitive match and sends the WHO messages.

    Test Case 8: Multiple Channels Exist
        Set up multiple channels with clients.
        Call cmd_who for each channel.
        Verify that the WHO messages are sent correctly for each channel.

    Test Case 9: Random Arguments Provided
        Call cmd_who with random arguments that are not channel names.
        Verify that an error message is sent indicating that the argument is not valid.

    Test Case 10: Valid Channel and Clients with Different Modes
        Set up a channel with clients having different modes (operators, regular clients).
        Call cmd_who for the channel.
        Verify that the WHO messages correctly reflect the modes of the clients.

LIST

    Test Case 1: List All Channels
        Call cmd_list with an empty string as the input.
        Verify that RPL_LISTSTART is sent.
        Verify that RPL_LIST messages are sent for each channel.
        Verify that RPL_LISTEND is sent at the end.

    Test Case 2: List Specific Channels
        Set up multiple channels.
        Call cmd_list with a string containing specific channel names (e.g., "#channel1,#channel2").
        Verify that RPL_LISTSTART is sent.
        Verify that RPL_LIST messages are sent for the specified channels.
        Verify that RPL_LISTEND is sent at the end.

    Test Case 3: List Channels with Invalid Names
        Call cmd_list with a string containing channel names that do not exist.
        Verify that RPL_LISTSTART is sent.
        Verify that ERR_NOSUCHCHANNEL messages are sent for each non-existing channel.
        Verify that RPL_LISTEND is sent at the end.

    Test Case 4: List Channels When No Channels Exist
        Call cmd_list when there are no channels in the server.
        Verify that RPL_LISTSTART is sent.
        Verify that RPL_LISTEND is sent immediately without sending any RPL_LIST messages.

    Test Case 5: List Channels with Empty Input
        Set up multiple channels.
        Call cmd_list with an empty string as the input.
        Verify that RPL_LISTSTART is sent.
        Verify that RPL_LIST messages are sent for each channel.
        Verify that RPL_LISTEND is sent at the end.

    Test Case 6: List Channels with Random Input
        Call cmd_list with a random string as the input.
        Verify that RPL_LISTSTART is sent.
        Verify that ERR_NOSUCHCHANNEL messages are sent for each non-existing channel.
        Verify that RPL_LISTEND is sent at the end.

    Test Case 7: List Channels with Case-Insensitive Input
        Set up multiple channels with specific case (e.g., #Channel1).
        Call cmd_list with a different case (e.g., #channel1).
        Verify that the function recognizes the case-insensitive match and sends the appropriate RPL_LIST messages.

    Test Case 8: List Channels with Clients and Topic
        Set up channels with clients and topics.
        Call cmd_list to list all channels.
        Verify that RPL_LIST messages correctly reflect the number of clients and topics for each channel.

    Test Case 9: List Channels with Different Modes
        Set up channels with different modes (e.g., password-protected channels).
        Call cmd_list to list all channels.
        Verify that RPL_LIST messages correctly reflect the modes of each channel.

    Test Case 10: List Channels with Random Input and Existing Channels
        Call cmd_list with a mix of existing and non-existing channel names in the input.
        Verify that RPL_LISTSTART is sent.
        Verify that RPL_LIST messages are sent for existing channels.
        Verify that ERR_NOSUCHCHANNEL messages are sent for non-existing channels.
        Verify that RPL_LISTEND is sent at the end.

EXIT

    Test Case 1: Disconnect an Existing Client
        Add a client to the server.
        Call disconnect_client with the client's file descriptor.
        Verify that the client is removed from all channels.
        Verify that the client's file descriptor is closed.
        Verify that the client is removed from the _clients vector.


    Test Case 2: Disconnect Multiple Clients
        Add multiple clients to the server.
        Call disconnect_client for each client.
        Verify that all clients are removed from all channels, their file descriptors are closed, and they are removed from the _clients vector.

    Test Case 3: Disconnect Clients in Channels
        Add clients to multiple channels.
        Call disconnect_client for some of these clients.
        Verify that the disconnected clients are removed from the channels they were part of, while other clients and channels remain unaffected.

    Test Case 4: Disconnect All Clients
        Add multiple clients to the server.
        Call disconnect_client for each client in a loop.
        Verify that all clients are successfully disconnected.

    Test Case 5: Verify Channel State After Disconnection
        Add clients to multiple channels.
        Disconnect some clients.
        Verify that the channels' state is correctly updated, and the disconnected clients are removed from the channels.

    Test Case 6: Verify Exception Handling
        Call disconnect_client with an invalid file descriptor (e.g., -1).
        Verify that the function throws a std::runtime_error indicating that the client could not be found.

    Test Case 7: Disconnect Clients with Different States
        Add clients to the server with various states (e.g., some in channels, some not in any channel).
        Disconnect these clients.
        Verify that the disconnection process works correctly for clients in different states.

HALP

Test Case 1: Normal User

    Create a client with normal user privileges (not an admin or operator).
    Call cmd_help for this client.
    Verify that the client receives the list of available commands for normal users.
    Verify that operator commands are not listed.

Test Case 2: Admin User

    Create a client with admin privileges.
    Call cmd_help for this client.
    Verify that the client receives the list of available commands for normal users.
    Verify that operator commands are also listed.

NO Existing command
	Test Case 1: COMMAND
		USE a non existing command. 

INVITE

    Test Case 1: Successful Invitation
        Create a channel.
        Create an operator client and join the channel.
        Create a non-operator client.
        Call cmd_invite with the operator client inviting the non-operator client to the channel.
        Verify that the non-operator client receives the invitation message.
        Verify that the non-operator client is added to the list of invited clients in the channel.

    Test Case 2: Non-Operator Trying to Invite
        Create a channel.
        Create a non-operator client and join the channel.
        Create another client.
        Call cmd_invite with the non-operator client trying to invite the other client.
        Verify that the non-operator client receives an error message indicating insufficient privileges.

    Test Case 3: Non-Existing Channel
        Create a non-operator client.
        Call cmd_invite with the non-operator client trying to invite another client to a non-existing channel.
        Verify that the client receives an error message indicating that the channel does not exist.

    Test Case 4: Non-Existing Client
        Create a channel.
        Create an operator client and join the channel.
        Call cmd_invite with the operator client trying to invite a non-existing client.
        Verify that the operator client receives an error message indicating that the user with the specified nickname does not exist.

    Test Case 5: Client Already Invited
        Create a channel.
        Create an operator client and join the channel.
        Create a non-operator client.
        Call cmd_invite to invite the non-operator client to the channel.
        Call cmd_invite again to invite the non-operator client (second invitation).
        Verify that the operator client receives an error message indicating that the client is already invited.

    Test Case 6: Client Already in Channel
        Create a channel.
        Create an operator client and join the channel.
        Create a non-operator client and join the channel.
        Call cmd_invite to invite the non-operator client to the channel.
        Verify that the operator client receives an error message indicating that the client is already in the channel.

    Test Case 7: Malformed Command
        Create a channel.
        Create an operator client and join the channel.
        Call cmd_invite with a malformed command (missing parameters).
        Verify that the operator client receives an error message indicating the correct usage of the INVITE command.

    Test Case 8: Invite Multiple Clients
        Create a channel.
        Create an operator client and join the channel.
        Create multiple non-operator clients.
        Call cmd_invite to invite each non-operator client to the channel.
        Verify that each non-operator client receives the invitation message.
        Verify that each non-operator client is added to the list of invited clients in the channel.

Test the moderator function of the bot

    Test Case 1: Clean Message
        Create a channel.
        Create a client.
        Call big_brother with a clean message (no naughty words).
        Verify that the function returns false.
        Verify that the message is sent to the channel.

    Test Case 2: Warning for One Strike
        Create a channel.
        Create a client with 0 strikes.
        Call big_brother with a message containing a single naughty word.
        Verify that the function returns false.
        Verify that the client receives a warning message.
        Verify that the warning message is sent to the channel.

    Test Case 3: Ban for Two Strikes
        Create a channel.
        Create a client with 1 strike.
        Call big_brother with a message containing a naughty word.
        Verify that the function returns true.
        Verify that the client receives a ban message.
        Verify that the ban message is sent to the channel.
        Verify that the client is banned.

    Test Case 4: Multiple Naughty Words in Message
        Create a channel.
        Create a client.
        Call big_brother with a message containing multiple naughty words.
        Verify the appropriate number of warnings or bans based on the strikes.

    Test Case 5: Message in Uppercase
        Create a channel.
        Create a client.
        Call big_brother with a message containing a naughty word in uppercase.
        Verify that the function correctly detects and handles uppercase naughty words.

    Test Case 6: Load Naughty Words Error
        Set up the getLoadNaughtyWordsFromFile function to return false.
        Create a channel.
        Create a client.
        Call big_brother.
        Verify that the function returns false.
        Verify that an error message is printed to stderr.

    Test Case 7: Empty Message
        Create a channel.
        Create a client.
        Call big_brother with an empty message.
        Verify that the function returns false.
        Verify that the message is sent to the channel.

    Test Case 8: Empty Naughty Words List
        Set up the _naughty_words vector to be empty.
        Create a channel.
        Create a client.
        Call big_brother with a message.
        Verify that the function returns false.
        Verify that the message is sent to the channel.