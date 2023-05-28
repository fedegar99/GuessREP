#ifndef DATABASE_H
#define DATABASE_H

#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>

//sudo apt-get install libpq-dev
#include"/usr/include/postgresql/libpq-fe.h"
#include"strutture.h" 
#include"base64.h"

utente* login(PGconn *conn, char email[32], char password[16], int socket, pthread_t tid);
bool signUp(PGconn *conn, char username[32], char password[16], char email[64]);
utente* getUserByEmail(PGconn *conn, char email[64], int socket, pthread_t tid);
bool updateUserPartiteVinte(PGconn *conn, char email[64]);
char* getAvatarByEmail(PGconn *conn, char email[64], pthread_t tid);
bool setAvatarOfUser(PGconn *conn, char email[64], int idAvatar);

#endif
