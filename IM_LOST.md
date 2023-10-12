What is IRC?
- IRC is an old chat protocol. People use it to chat on channels or have private conversations (discord, teams, slack...)

## Sockets
Os sockets são como pontos virtuais de envio ou receção de dados numa rede informática.
Eles são basicamente a espinha dorsal do nosso servidor

### Criar, escolher opções e atribuir o Socket
Ao criar um socket, estamos a criar o nosso canal. As opções bão ser os ajustes especiais que aplicamos para fazê-lo comprtar-se da maneira que queremos. Por exemplo "Usa X endereço" ou "escuta por este IP"


Atruibuir o Socket (Binding) é onde o nosso soket ganha uma identidade. Ligamo-lo a um IP e porta para que os outros saibam onde encontrá-lo.

## Listen and Accept
Uma vez configurado o nosso socket, temos de escutar conexões de entrada. É como se o nosso servidor abrisse a porta e esperasse por visitantes.

## Polling: verificar múltiplas Portas
No nosso caso, em vez de olhar para uma porta, o nosso servidor verifica várias portas para ver se alguém está a tentar conectar-se. Esta é uma maneira efeciente de lidar com múltiplos clientes (exigido pelo subject).

Em termos técnicos, polling é um mecanismo que permite ao nosso servidor verificar múltiplos sockets para ver se há dados disponíveis para leitura ou escritura

### Porque é que usamos?
Sem o polling, o nosso servidor teria que verificar cada socket individualmente, um de cada vez, o que é bastante lento. Assim, podemos perguntar ao sistema se tem updates de qualquer um dos sockets. Se sim, ele vai dizer de quais sockets têm dados prontos para serem lidos.

No entanto, isto não quer dizer que ele esteja sempre a atualizar todos os sockets. Pelo contrário, ele bloqueia até que pelo menos um dos sockets esteja pronto.

## Poll no código
```shell
pollfd pfd;
pfd.fd = server_fd;
pfd.events = POLLIN;

while(1)
{
    int poll_ret = poll(&pfd, 1, -1);
    if(poll_ret == -1)
    {
        std::cerr << "Error: poll failed" << std::endl;
        break;
    }
    else if(poll_ret > 0)
    {
        if(pfd.revents & POLLIN)
        {
            // ...
        }
    }
}
```

- pollfd pfd; Estás a declarar uma estrutura que representa um FD que queremos monitorizar

- pfd.fd = server_fd; estás a dizer que queres monitorizar o server_fd (socket do servidor)

- pdf.evenets = POLLIN; Estás a especificar o tipo de eventos que queres monitorizar. POLLIN signfica que estás interessados em eventos de entrada, como os dados que chegam ou um novo cliente que tenta conectar-se

- poll(&pfd, 1, -1); Aqui estás a invocar a função poll e estás a passar um array de FDs para monitorizar (neste caso apenas um), o número de fds no array e o tempo de espera. -1 significa que vais esperar indefinidamente até que algo aconteça.

- if(pfd.revents & POLLIN) - Estás a verificar se o evento foi de tipo POLLIN. Se foi, alguma ação é tomada.