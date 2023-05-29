#!/bin/bash
gcc -o server src/serverTest.c src/database.c src/strutture.c src/word.c src/gamemanager.c src/base64.c -ljson-c -pthread -g -I/usr/include/postgresql -lpq
