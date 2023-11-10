<h1 align="center">
	🌐 FT_IRC 🌐
</h1>

<p align="center">
	<b><i> Bridging the Bytes for Seamless Conversations 🌉</i></b>
</p>

<div align="center">
<img alt="FT_IRC" src="https://github.com/joao-per/joao-per/blob/main/Badges/Ft_irc.png" />
</div>

## 🧠 About the project
A basic implementation of an IRC server that handles authentication, user registration, and several chat commands. 🖥️

## 🌟 Features
🔒 **Authentication**: Authenticate users with a password.

🖋️ **Registration**: Register users by setting their nickname (NICK) username (USER) and realname!

📜 **Command Handling**: Process various commands like JOIN, PRIVMSG and MODE.

🧑‍🤝‍🧑 **Multi-Client Handling**: Uses poll() system call to handle multiple clients simultaneously.

## 🚀 Getting Started
### 🛠️ Prerequisites
A C++ compiler with support for C++98 only.
Basic knowledge of IRC protocols (optional).

### 📖 Compilation
You can compile the project using:

```bash
make
```

And then run the server:
```bash
./ircserv <port> <password>
```

For example:
```bash
./ircserv 6667 12345
```
## 📖 Instructions
PASS <password>

NICK <nickname>

USER <username> <mode> <hostname> :<realname>

**All available commands:** CREATE, JOIN, PRIVMSG, MODE(i,k,t,l,o)

## 🙏 Acknowledgments
A huge thank you to my colleagues gacorrei and abaiao-r for their invaluable contributions and insights. This project wouldn't be the same without their assistance. 🙌

## 📝 License
This project is licensed under the MIT License - see the LICENSE file for details.
