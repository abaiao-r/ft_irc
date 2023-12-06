#!/bin/bash

# Joao.sh - Script for Joao to join channels created by Andre

# Open a connection to the server on localhost port 6697
{
    sleep 5  # Adjust as needed to ensure Andre.sh has completed its actions
    echo "PASS pass"
    sleep 0.5

    # Join channel_x with a unique nick for each Joao
    for ((i=1; i<=2; i++)); do
        echo "NICK joao_$i"
        sleep 0.5
        echo "USER JOAO_$i"
        sleep 0.5
        echo "JOIN #channel_x"
        sleep 0.1
    done

    # Join channel_y (uncomment if you have channel_y)
    # for ((i=1; i<=60; i++)); do
    #     echo "NICK joao_y_$i"
    #     sleep 0.1
    #     echo "USER JOAO"
    #     sleep 0.1
    #     echo "JOIN #channel_y"
    #     sleep 0.1
    # done

    # Join channel_z (uncomment if you have channel_z)
    # for ((i=1; i<=60; i++)); do
    #     echo "NICK joao_z_$i"
    #     sleep 0.1
    #     echo "USER JOAO"
    #     sleep 0.1
    #     echo "JOIN #channel_z"
    #     sleep 0.1
    # done

    sleep 5  # Adjust as needed to ensure all joins are completed before exiting
} | nc localhost 6697