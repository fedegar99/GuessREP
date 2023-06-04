#ifndef STRUTTURE_C
#define STRUTTURE_C

#include "../header/strutture.h"

stanza *stanze[50];
int stanze_lenght = 50;

pthread_mutex_t mutex;
pthread_cond_t condition;

//Allocazione di un nuovo utente
utente *new_utente(char *name, char *pass, char *em, int wongames, int socket, int imgid, pthread_t tid)
{
	utente *nuovo = (utente*)malloc(sizeof(utente));
	strcpy(nuovo->username, name);
	strcpy(nuovo->passw, pass);
	strcpy(nuovo->email, em);
	nuovo->partiteVinte = wongames;
	nuovo->idStanza = -1;
	nuovo->clientSocket = socket;
	nuovo->imgId = imgid;
	nuovo->tid = tid;
	nuovo->isReady = false;
	return nuovo;
}

//Allocazione di una nuova stanza
stanza *new_stanza(int id, char* nome, int maxPlayer, int _rounds, utente *admin)
{
	stanza *nuovo = (stanza*)malloc(sizeof(stanza));
	nuovo->idStanza = id;
	strcpy(nuovo->nomeStanza, nome);
	nuovo->numeroMaxGiocatori = maxPlayer;
	nuovo->rounds = _rounds;
	nuovo->started = false;
	nuovo->adminUser = admin;
	nuovo->players = (utente**)malloc(maxPlayer*(sizeof(utente*)));
	for(int i = 0; i < maxPlayer; i++)
	{
		nuovo->players[i] = NULL;
	}
	nuovo->players[0] = admin;
	pthread_mutex_init(&nuovo->roomMutex, NULL);
	pthread_mutex_lock(&nuovo->roomMutex);
	
	nuovo->tmp = false;
	return nuovo;
}


int rm_stanza_by_id(int id){
	int success = false;
	if(id < stanze_lenght){
		printf("arrivo al semaforo rm_stanza");
		//acquisizione
		pthread_mutex_lock(&mutex);
		// pthread_cond_wait(&condition, &mutex);

		if(stanze[id] != NULL){
			success = true;
			stanza *room = stanze[id];
			pthread_mutex_destroy(&room->roomMutex);

			for (int i = 0; i < room->numeroMaxGiocatori; i++)
			{
				if(room->players[i] != NULL){
					room->players[i]->idStanza = -1;
				}
			}

			free(stanze[id]);
			stanze[id] = NULL;
		}

		pthread_mutex_unlock(&mutex);

	}
	return success;
}

int rm_stanza(stanza *room){
	return rm_stanza_by_id(room->idStanza);
}

//aggiungi utente					forse solo id
int add_user_in_room(utente *user, stanza *room){
	if(user == NULL || room == NULL){
		return false;
	}
	
	rm_user_from_last_room(user);

	int success = false;
	//acquisizione
	pthread_mutex_lock(&mutex);
	int tmp = -1;
	for (int i = 0; i < room->numeroMaxGiocatori; i++){
		if(room->players[i] != NULL ){
			if(room->players[i]->username == user->username){
				success = true;
				break;
			}
		}else if(tmp == -1){
			tmp = i;
		}
	}
	if(tmp != -1 && !success){
		user->idStanza = room->idStanza;
		room->players[tmp] = user;
		success = true;
	}

	pthread_mutex_unlock(&mutex);
	return success;
}

int add_user_in_room_by_id(utente *user, int id){
	delete_utente_from_connected_room(user);
	add_user_in_room(user,get_stanza_by_id(id));
}

bool isAdmin(utente *user, stanza *room){
	return (room->adminUser->username == user->username);
}

int rm_user_from_room(utente *user, stanza *room ){
	if(user == NULL){
		return false;
	}
	return rm_user_from_room_by_username(user->username, room);
}

//rimuovi utente !!!!!CONFRONTA GLI USERNAME!!!!
int rm_user_from_room_by_username(char username[32], stanza *room ){
	if(room == NULL){
		return true;
	}
	bool success = false;
	if((strcmp(room->adminUser->username, username) == 0) && !room->started){
		success = rm_stanza(room);
	}
	else {
		for (int i = 0; i < room->numeroMaxGiocatori; i++){
			if(room->players[i] != NULL){
				if(strcmp(room->players[i]->username, username) == 0){
					room->players[i]->idStanza = -1;
					room->players[i] = NULL;
					success = true;
					break;
				}
			}
		}
		if(getActualPlayes(room) == 0) {
			success = rm_stanza(room);
		}
	}
	return success;
}

void rm_user_from_last_room(utente *user){
	if(user->idStanza != -1){
		rm_user_from_room(user,get_stanza_by_id(user->idStanza));
	}
}

int add_stanza(char *name, int max_player, int nRound, utente *admin){

	rm_user_from_last_room(admin);

	int id = 0;
	pthread_mutex_lock(&mutex);
	//pthread_cond_wait(&condition, &mutex);
	for (int i = 0; i < stanze_lenght; i++)
	{
		if (stanze[i] == NULL)
		{
			stanze[i] = new_stanza(i, name, max_player, nRound, admin);
			id = i;
			admin->idStanza = id;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
	return id;
}

stanza *get_stanza_by_id(int id){
	if(id < stanze_lenght){
		if(stanze[id] != NULL){
			return stanze[id];
		}
	} else {
		return NULL;
	}
}

stanza* get_stanze(){
	//FUNZIONE PER OTTENERE TUTTE LE STANZE DISPONIBILI
	stanza *stanze;
	return stanze;
}

int get_number_of_player_in_stanza(int id){
	stanza * actRoom = stanze[id];
	int numPlayers = 0;
	if(actRoom != NULL){
		for(int i = 0; i<actRoom->numeroMaxGiocatori; i++){
			if(actRoom->players[i] != NULL){
				numPlayers++;
			}
		}
	}
	return numPlayers;
}

void visualizza_stanze(){
	for (int i = 0; i < stanze_lenght; i++)
	{
		if(stanze[i] != NULL){
			printf("nome: %s, id: %d\n", stanze[i]->nomeStanza, stanze[i]->idStanza);
			for (int j = 0; j < stanze[i]->numeroMaxGiocatori; j++)
			{
				if(stanze[i]->players[j] != NULL){
					printf("-->%s\n", stanze[i]->players[j]->username);
				}
			}
		}
	}
}

utente* visualizza_stanza(int id){
	if(id < stanze_lenght && stanze[id] != NULL){
		int n = stanze[id]->numeroMaxGiocatori;
		utente * tmp[n];
		int last = 0;
		for(int j = 0; j < stanze[id]->numeroMaxGiocatori; j++){
			if(stanze[id]->players[j] != NULL){
					tmp[last] = stanze[id]->players[j];
					last++;
			}
		}
		return *tmp;
	}
	return NULL;
}

void delete_utente_from_connected_room(utente * user){
	for (int i = 0; i < stanze_lenght; i++)
	{
		if(stanze[i] != NULL){
			for (int j = 0; j < stanze[i]->numeroMaxGiocatori; j++)
			{
				if(stanze[i]->players[j] != NULL){
					if(stanze[i]->players[j]->username == user->username){
						rm_user_from_room_by_username(user->username, stanze[i]);
					}
				}
			}
		}
	}
}

void wait_until_ready(stanza * room, utente * user){
	if(room->started == true){
		user->isReady = true;
		bool canNext = true;
		for(int i = 0; i < room->numeroMaxGiocatori; i++){
			if(room->players[i] != NULL){
				if(room->players[i]->isReady == false){
					printf("canNext = false\n");
					canNext = false;
				}
			}
		}
		if (canNext == true){
			printf("pthread_mutex_unlock pre\n");
			pthread_mutex_unlock(&room->roomMutex);
			printf("pthread_mutex_unlock post\n");
		}else if(!room->tmp){
			printf("pthread_mutex_trylock pre\n");
			pthread_mutex_lock(&room->roomMutex);
			printf("pthread_mutex_trylock post\n");
		}
	}
	
}

int getActualPlayes(stanza *room) {
	int i, n = 0;
	if(room == NULL){
		return -1;
	}
	for(i = 0; i < room->numeroMaxGiocatori; i++) {
		if(room->players[i] != NULL) {
			n++;
		}	
	}
	return n;
}

#endif

//se un utente entra in una nuova stanza deve uscire dalla stanza attuale
