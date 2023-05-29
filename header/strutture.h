#ifndef STRUTTURE_H
#define STRUTTURE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<pthread.h>

typedef struct {
	char username[32];
	char passw[16];
	char email[64];
	int partiteVinte;
	int idStanza;
	int clientSocket;
	int imgId;
	bool isReady;
	pthread_t tid;
} utente;

typedef struct {
	int idStanza;
	char nomeStanza[16]; //Da aumentare
	int numeroMaxGiocatori;
	int turn;
	bool started;
	bool tmp;
	utente *adminUser;
	utente **players; //Array di utenti
	pthread_mutex_t roomMutex;
} stanza;

extern stanza *stanze[50];
extern int stanze_lenght;

extern pthread_mutex_t mutex;
extern pthread_cond_t condition;

//Allocazione di un nuovo utente
utente *new_utente(char *name, char *pass, char *em, int wongames, int socket, int imgid, pthread_t tid);

//Allocazione di una nuova stanza !!NON USARE PER AGGIUNGERE STANZA USA add_stanza!!
stanza *new_stanza(int id, char* nome, int maxPlayer, utente *admin);

int rm_stanza_by_id(int id);

int rm_stanza(stanza *room);

//aggiungi utente					forse solo id
int add_user_in_room(utente *user, stanza *room);

//rimuovi utente !!!!!CONDRONTA I PUNTATORI!!!!!
int rm_user_from_room(utente *user, stanza *room );

int add_stanza(char *name, int max_player, utente *admin);

stanza *get_stanza_by_id(int id);

int get_number_of_player_in_stanza(int id);

utente* visualizza_stanza(int id);

//STAMPA SU CONSOLE FUNZIONE TEMPORANEA
void visualizza_stanze();

bool isAdmin(utente*, stanza*);

void rm_user_from_last_room(utente *user);

int rm_user_from_room_by_username(char username[32], stanza *room );

void delete_utente_from_connected_room(utente * user);
void wait_until_ready(stanza * room, utente * user);

int getActualPlayes(stanza *room);

#endif
