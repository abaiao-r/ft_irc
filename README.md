OK entao basicamente, COISAS A FAZER:

1- dar fix no login. A ordem pode ser aleatoria e nao pode ser como esta. Tambem adicionar a autenticacao por hexchat que esta no meu ficheiro Hexchat.cpp

2- Adicionar todos os comandos. Todos estao presentes no Commands.cpp . So copiar accordingly

3- Verificar que todas as variaveis guardadas (nome, username, etc) sao guardadas sem \r e \n

4- Se sobrar tempo, testar com Hexchat, caso contrario, so com nc mesmo.

<<<<<<< HEAD
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
PASS (password)

NICK (nickname)

USER (username) (mode) (hostname) :(realname)

**All available commands:** CREATE, JOIN, PRIVMSG, MODE(i,k,t,l,o)

## 🙏 Acknowledgments
A huge thank you to my colleagues gacorrei and abaiao-r for their invaluable contributions and insights. This project wouldn't be the same without their assistance. 🙌

## 📝 License
This project is licensed under the MIT License - see the LICENSE file for details.
=======
5- comandos a serem testados conforme a implementacao que fiz:
CREATE #<nome da sala> <OPCIONAL: topico da sala>
JOIN #<nome da sala>
TOPIC #<nome da sala> <topico>
KICK #<nome da sala> <user>
INVITE #<nome da sala> <user>

POR FAVOR QUALQUER DUVIDA, LIGUEM-ME, MANDEM-ME MENSAGEM, O QUE QUISEREM
>>>>>>> irc2
