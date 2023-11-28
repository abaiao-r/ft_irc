#!/bin/bash

# Script to run Andre.sh and Gabriel.sh in new xterm windows

# Run Andre.sh in a new xterm window
terminator -e "./list/Andre.sh" &

#-e is to execute the command in the new terminal
#to print the comand on the terminal use -x example: terminator -x "./list/Andre.sh" &


# Run Gabriel.sh in a new xterm window
terminator --new-tab -e "./list/Gabriel.sh" &
