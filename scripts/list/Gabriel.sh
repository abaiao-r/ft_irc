#!/bin/bash

# Open a connection to the server on localhost port 6697
# scripts/list/Gabriel.sh
# commands echo
# PASS pass
# NICK gabriel_1
# USER Gabriel_1
# JOIN #channel_x
# JOIN #channel_y
# LIST #channel_x
# LIST #channel_z
# LIST

{

    sleep 5  # Adjust as needed to ensure Andre.sh has completed its actions
    # all output of echo commands are blue and send to a terminal
    echo 'PASS pass'
    sleep 0.5
    echo 'NICK gabriel_'
    sleep 0.5
    echo 'USER Gabriel_'
    sleep 0.5
    echo 'JOIN #channel_x'
    sleep 0.5
    echo 'JOIN #channel_y'
    sleep 0.5
    echo 'LIST #channel_x' # Channel exists
    sleep 0.5
    echo 'LIST #channel_z' # Channel does not exist
    sleep 0.5
    echo 'LIST' # List all channels
    #print all output of echo commands to xterm
} | nc localhost 6697
