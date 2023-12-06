
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


