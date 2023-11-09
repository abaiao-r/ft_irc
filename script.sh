#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  echo "PASS 1234"
  sleep 0.2
  echo "USER xisde 1 * :Joao Pereira"
  sleep 0.2
  echo "NICK fritick"
  sleep 0.2
  echo "CREATE test Macacos"
  sleep 0.2
  echo "MODE #test +k 12345"
  sleep 0.2
  echo "INVITE #test coxinha"
} | nc localhost 6697