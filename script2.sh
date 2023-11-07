#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  sleep 1
  echo "PASS 1234"
  sleep 1
  echo "USER test 1 * :Rodrigo"
  sleep 1
  echo "NICK testador"
  sleep 1
  echo "JOIN #test"
  sleep 1
  echo "PRIVMSG #test :Hello World!"
  sleep 1
  echo "QUIT"
} | nc localhost 6697