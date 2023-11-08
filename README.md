OK entao basicamente, COISAS A FAZER:

1- dar fix no login. A ordem pode ser aleatoria e nao pode ser como esta. Tambem adicionar a autenticacao por hexchat que esta no meu ficheiro Hexchat.cpp

2- Adicionar todos os comandos. Todos estao presentes no Commands.cpp . So copiar accordingly

3- Verificar que todas as variaveis guardadas (nome, username, etc) sao guardadas sem \r e \n

4- Se sobrar tempo, testar com Hexchat, caso contrario, so com nc mesmo.

5- comandos a serem testados conforme a implementacao que fiz:
CREATE #<nome da sala> <OPCIONAL: topico da sala>
JOIN #<nome da sala>
TOPIC #<nome da sala> <topico>
KICK #<nome da sala> <user>
INVITE #<nome da sala> <user>

POR FAVOR QUALQUER DUVIDA, LIGUEM-ME, MANDEM-ME MENSAGEM, O QUE QUISEREM
