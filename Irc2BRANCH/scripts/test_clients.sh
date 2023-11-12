#!/bin/bash

# Function to launch IRC client script in the background
launch_client() {
    ./Andre.sh "$1" &
    ./Joao.sh "$2" &
    ./Gabriel.sh "$3" &
}

# Launch multiple clients concurrently
for ((i=0; i<2; i++)); do
    launch_client "Andre_Client$i" "Joao_Client$i" "Gabriel_Client$i"
    sleep 5
done

# Wait for all background processes to finish
wait $(jobs -p)

echo "All clients have completed their actions."


