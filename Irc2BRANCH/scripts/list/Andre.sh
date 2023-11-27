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

    # all output of echo commands are blue and send to a terminal
    echo 'PASS pass'
    sleep 0.5
    echo 'NICK andre_'
    sleep 0.5
    echo 'USER Andre_'
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
    sleep 0.5
    #print all output of echo commands to xterm
} | nc localhost 6697

