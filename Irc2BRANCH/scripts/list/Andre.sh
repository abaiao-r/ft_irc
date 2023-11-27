#!/bin/bash

# Open a connection to the server on localhost port 6697
# scripts/list/Andre.sh
# commands echo
# PASS pass
# NICK andre_1
# USER Andre_1
# JOIN #channel_x
# JOIN #channel_y
# LIST #channel_x
# LIST #channel_z
# LIST
{
    # Define blue and reset colors
    blue="\e[34m"
    reset="\e[0m"

    # all output of echo commands are blue and send to a terminal
    echo -e "$blue" 'PASS pass' "$reset"
    for ((i=1; i<=1; i++)); do
        echo -e "$blue" 'NICK andre_$i' "$reset"
        sleep 0.5
        echo -e "$blue" 'USER Andre_$i' "$reset"
        sleep 0.5
        echo -e "$blue" 'JOIN #channel_x' "$reset"
        sleep 0.5
        echo -e "$blue" 'JOIN #channel_y' "$reset"
        sleep 0.5
        echo -e "$blue" 'LIST #channel_x' "$reset" # Channel exists
        sleep 0.5
        echo -e "$blue" 'LIST #channel_z' "$reset" # Channel does not exist
        sleep 0.5
        echo -e "$blue" 'LIST' "$reset" # List all channels
        sleep 0.5
    done
} | nc localhost 6697
