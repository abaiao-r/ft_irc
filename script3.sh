#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  echo "PASS 1234"
  sleep 0.2
  echo "USER test 0 * :Rodrigo"
  sleep 0.2
  echo "NICK testador"
  sleep 0.2
  echo "JOIN #test 12345"
  sleep 0.2
} | nc localhost 6697