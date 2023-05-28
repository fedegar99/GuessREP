FROM gcc:latest

COPY src /app/src
COPY header /app/header

RUN apt-get update && apt-get install -y libjson-c-dev
RUN apt-get install -y libpq-dev
RUN apt-get install -y libjson-c-dev

WORKDIR /app

RUN gcc -o  /app/myprogram src/main.c src/database.c src/strutture.c src/word.c src/gamemanager.c src/base64.c -ljson-c -pthread -g -I/usr/include/postgresql -lpq

CMD ["./myprogram"]