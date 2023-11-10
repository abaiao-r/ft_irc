#!bin/bash

# Andre.sh - Script para o Andre

# Open a connection to the server on localhost port 6697
{
    echo "PASS pass"
    sleep 0.2
    echo "NICK abaiao-r"
    sleep 0.2
    echo "USER Andre"
    sleep 20
    echo "JOIN #sala p"
    sleep 0.2
    echo "JOIN #sala qwerty"
    sleep 0.2
    echo "MODE #sala +i"
    sleep 0.2
    echo "MODE #sala +t"
    sleep 0.2
    echo "TOPIC #sala Irc rules"
    sleep 0.2
    echo "INVITE joao-per #sala"
    sleep 20
    echo "MODE #sala +o joao-per"
    sleep 20
    echo "INVITE gacorrei #sala"
    sleep 0.2
} | nc localhost 6697