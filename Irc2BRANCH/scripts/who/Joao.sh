#!/bin/bash

# Open a connection to the server on localhost port 6697
# scripts/list/Andre.sh
# commands echo
# PASS pass
# NICK Joao
# USER Joao
# JOIN #channel_x
# WHO
# WHO #channel_y
# WHO #channel_x

{

    # all output of echo commands are blue and send to a terminal
    echo 'PASS pass'
    sleep 0.5
    echo 'NICK Joao'
    sleep 0.5
    echo 'USER Joao'
    sleep 0.5
    echo 'JOIN #channel_x'
    sleep 0.5
    echo 'WHO' # no arguments
    sleep 0.5
    echo 'WHO #channel_y' # channel does not exist
    sleep 0.5
    echo 'WHO #channel_x' # channel exists
    sleep 0.5
} | nc localhost 6697