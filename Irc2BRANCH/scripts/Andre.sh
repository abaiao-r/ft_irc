#!/bin/bash

# Andre.sh - Script for Andre to create channels

# Open a connection to the server on localhost port 6697
{
    echo "PASS pass"
    sleep 0.5
    for ((i=1; i<=2; i++)); do
        echo "NICK andre_$i"
        sleep 0.5
        echo "USER Andre_$i"
        sleep 0.5
        echo "JOIN #channel_x"

        sleep 15  # Adjust as needed

        for ((j=1; j<=1; j++)); do
            echo "KICK #channel_x Joao_$j"
            sleep 0.1
        done
    done
} | nc localhost 6697
