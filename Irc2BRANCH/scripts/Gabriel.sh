#!bin/bash

# Andre.sh - Script para o Andre

# Open a connection to the server on localhost port 6697
{
    echo "PASS pass"
    sleep 0.2
    echo "NICK gacorrei"
    sleep 0.2
    echo "USER Gabriel"
    sleep 0.2
    echo "JOIN #sala qwerty"
    sleep 0.2
    echo "TOPIC #sala asa"
    sleep 0.2
    echo "TOPIC #sala"
    sleep 0.2
    echo "KICK #sala abaiao-r"
    sleep 0.2
} | nc localhost 6697

