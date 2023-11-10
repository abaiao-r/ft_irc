#!/bin/bash

# Open a connection to localhost on port 6697
{ 
  echo "PASS 1234"
  sleep 0.2
  echo "USER test 0 * :Rodrigo"
  sleep 0.2
  echo "NICK testador"
  sleep 0.2
  echo "JOIN #test2 123456"
  sleep 1
  echo "PRIVMSG #test2 :Mensagem de teste"
  sleep 1
  echo "PRIVMSG fritick :Mensagem de teste"
  sleep 1
  echo "JOIN #test2 12345"
  sleep 1
  echo "JOIN #test"
  sleep 1
  echo "TOPIC #test Cores"
  sleep 1
  echo "TOPIC #test2 Animais"
  sleep 1
  echo "TOPIC #test"
} | nc localhost 6697