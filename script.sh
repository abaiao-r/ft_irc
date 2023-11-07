#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  sleep 1
  echo "PASS 1234"
  sleep 1
  echo "USER xisde 1 * :Joao Pereira"
  sleep 1
  echo "NICK fritick"
  sleep 1
  echo "CREATE test"
} | nc localhost 6697