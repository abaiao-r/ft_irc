#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  echo "PASS 1234"
  sleep 0.2
  echo "USER xisde 1 * :JoaoPereira"
  sleep 0.2
  echo "NICK fritick"
  sleep 0.2
  echo "CREATE test Macacos"
  sleep 0.2
  echo "JOIN #test2 Gorilas"
  sleep 0.2
  echo "MODE #test2 +k 12345"
  sleep 0.2
  echo "MODE #test2 +l 1"
  sleep 0.2
  echo "MODE #test"
  sleep 0.2
  echo "MODE #test -t"
  sleep 0.2
  echo "TOPIC #test2 Orangotangos"
  sleep 0.2

  echo "INVITE #test testador"
} | nc localhost 6697