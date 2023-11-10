#!bin/bash

# Andre.sh - Script para o Andre

# Open a connection to the server on localhost port 6697
{
    echo "PASS pass"
    sleep 0.2
    echo "NICK joao-per"
    sleep 0.2
    echo "USER JOAO"
    sleep 0.2
    echo "JOIN #sala qwwe"
    sleep 0.2
    echo "JOIN #sala qwerty"
    sleep 0.2
    echo "MODE #sala +t"
    sleep 0.2
    echo "TOPIC #sala TOPIC"
    sleep 0.2
    echo "PrivMSG #sala Mensagem"
    sleep 0.2
    echo "KICK #sala gacorreia"
} | nc localhost 6697