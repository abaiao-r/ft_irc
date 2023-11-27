#!/bin/bash

# Script to run Andre.sh and Gabriel.sh in new xterm windows

# Run Andre.sh in a new xterm window
terminator -e "./list/Andre.sh" &


# Run Gabriel.sh in a new xterm window
terminator --new-tab -e "./list/Gabriel.sh" &
