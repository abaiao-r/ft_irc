#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  echo "PASS 1234"
  sleep 0.2
  echo "USER lolz 1 * :Rodrigo"
  sleep 0.2
  echo "NICK test"
  sleep 0.2
  echo "KICK #test testador"
  sleep 0.2
} | nc localhost 6697