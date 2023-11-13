#!/bin/bash

# Andre.sh - Script for Andre to create channels

# Open a connection to the server on localhost port 6697
{
    echo "PASS pass"
    sleep 0.5
    for ((i=1; i<=1; i++)); do
        echo "NICK andre_$i"
        sleep 0.5
        echo "USER Andre_$i"
        sleep 0.5
        echo "JOIN #channel_x"

        sleep 30  # Adjust as needed

        for ((j=1; j<=1; j++)); do
            echo "TOPIC #channel_x IRC"
            sleep 0.2
            echo "INVITE gabriel_$i #channel_x"
            sleep 0.2
            echo "MODE #channel_x +o gabriel_$i"
            sleep 0.1
            echo "KICK #channel_x gabriel_$i"
            sleep 0.2
        done
    done
} | nc localhost 6697
