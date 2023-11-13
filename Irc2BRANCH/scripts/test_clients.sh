#!/bin/bash

# Function to launch IRC client script in the background
launch_client() {
    # $1 = Andre.sh
    # $2 = Joao.sh
    ./Gabriel.sh "$3" &
}

# Launch multiple clients concurrently
for ((i=0; i<1; i++)); do
    launch_client "Gabriel_Client$i"
    sleep 5
done

# Wait for all background processes to finish
wait $(jobs -p)

echo "All clients have completed their actions."


