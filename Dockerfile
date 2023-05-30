FROM gcc:latest

COPY src /app/src
COPY header /app/header

WORKDIR /app

RUN apt-get update && apt-get install -y libjson-c-dev
RUN apt-get install -y libpq-dev
RUN apt-get install -y libjson-c-dev

RUN gcc -o  /app/myprogram src/serverTest.c src/database.c src/strutture.c src/word.c src/gamemanager.c src/base64.c -ljson-c -pthread -g -I/usr/include/postgresql -lpq
# RUN gcc -o myprogram app/src/serverTest.c app/src/database.c app/src/strutture.c app/src/word.c app/src/gamemanager.c app/src/base64.c -ljson-c -pthread -g -I/usr/include/postgresql -lpq

CMD ["./myprogram"]