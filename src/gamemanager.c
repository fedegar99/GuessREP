#ifndef GAMEMANAGER_C
#define GAMEMANAGER_C

#include "../header/gameManager.h"
#define ADMIN_ERROR -1
#define USER_ERROR -2

char* start_room(stanza* currentRoom, int targetScore) {
    int i = 0;
    char response[BUFFDIM];
    int *score = (int*)calloc(currentRoom->numeroMaxGiocatori, sizeof(int));
    int winnerIndex = -1;
    char *winnerEmail;

    printf("Game started in %s\nGiocatori:\n", currentRoom->nomeStanza);
    for(i=0; i<currentRoom->numeroMaxGiocatori; i++) {
        if(currentRoom->players[i] != NULL)
            printf("%s\n", currentRoom->players[i]->username);
    }

    //Set connection time-out per le socket
    for(i = 0; i < currentRoom->numeroMaxGiocatori; i++) {
        if(currentRoom->players[i] == NULL)
            continue;
        struct timeval tv;
        tv.tv_sec = 45;
        tv.tv_usec = 0;
        setsockopt(currentRoom->players[i]->clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    }

    i=0;
    do
    {
        if(currentRoom->players[i] == NULL){
            i = ++i % currentRoom->numeroMaxGiocatori;
            continue;
            //forse sbagliato?
        }
        sendBroadcast(currentRoom, i, "WAIT");
        send(currentRoom->players[i]->clientSocket, "CHOOSE", 7, 0);
        recv(currentRoom->players[i]->clientSocket, response, BUFFDIM, 0);
        printf("HOST RESPONSE %s\n", response);
        //printf("Host id: %d\n", i);
        winnerIndex = start_round(currentRoom, i);
        if(winnerIndex != ADMIN_ERROR)
            score[winnerIndex]++;
        else
            return NULL;  
        i = ++i % currentRoom->numeroMaxGiocatori;
    } while(score[winnerIndex] < targetScore);
    sendBroadcast(currentRoom, -1, "END");

    //Sblocca thread degli altri utenti
    for(i = 0; i < currentRoom->numeroMaxGiocatori; i++) {
        if(currentRoom->players[i] == NULL)
            continue;
        pthread_kill(currentRoom->players[i]->tid, SIGUSR1);
    }

    //Ripristina connection time-out per le socket
    for(i = 0; i < currentRoom->numeroMaxGiocatori; i++) {
        if(currentRoom->players[i] == NULL)
            continue;
        struct timeval tv;
        tv.tv_sec = 3600;
        tv.tv_usec = 0;
        setsockopt(currentRoom->players[i]->clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    }

    //Restituice il vincitore
    winnerEmail = currentRoom->players[winnerIndex]->email;
    return winnerEmail;
}

int start_round(stanza *room, int idHostPlayer) {
    int i = 0, j = 0;
    char choosenWord[32];
    char buffer[BUFFDIM] = {0};
    char *parole[NUMBER_OF_SUGGESTED_WORD][2];
    int choosenWordIndex = 0, winnerIndex = -1;

    //Selezione parola da parte dell'host del round
    printf("Reupero le parole per la stanza %s...\n", room->nomeStanza);
    generateSuggestedWords(NUMBER_OF_SUGGESTED_WORD, parole);
    prepareWords(buffer, parole);
    //printf("%s", buffer);
    send(room->players[idHostPlayer]->clientSocket, buffer, strlen(buffer), 0); //Invio delle parole suggerite
    memset(buffer, 0, sizeof(buffer));
    recv(room->players[idHostPlayer]->clientSocket, buffer, 32, 0 ); //In attesa di ricevere la parola scelta
    if(buffer[0] == 0) {
        printf("Connection error\n");
        return ADMIN_ERROR;
    }
    while(strcmp(parole[choosenWordIndex][0], buffer) != 0){ //Estrazione dell'indice della parola scelta dalla matrice
        choosenWordIndex++;
    }
    strcpy(choosenWord, parole[choosenWordIndex][0]);
    printf("%s ha scelto %s\n - %s\n", room->players[idHostPlayer]->username, choosenWord, parole[choosenWordIndex][1]);

    //Invio della parola scelta + definizione ai giocatori
    struct json_object *jsObj = json_object_new_object();
    json_object_object_add(jsObj, "word", json_object_new_string(choosenWord));
    json_object_object_add(jsObj, "definition", json_object_new_string(parole[choosenWordIndex][1]));
    sendBroadcast(room, idHostPlayer, (char*) json_object_to_json_string(jsObj));

    //Svolgimento del round
    int guessed = 0;
    int *hints = (int*)calloc(strlen(choosenWord), sizeof(int));
    do {
        for(i = 0; i < room->numeroMaxGiocatori; i++) {
            if(room->players[i] == NULL || i == idHostPlayer)
                continue;
            else {
                send(room->players[i]->clientSocket, "YOUR_TURN", 10, 0);
                memset(buffer, 0, sizeof(buffer));
                recv(room->players[i]->clientSocket, buffer, BUFFDIM, 0); //In attesa del tentativo
                if(buffer[0] == 0) {
                    printf("Connection error\n");
                    rm_user_from_room(room->players[i], room);
                    return USER_ERROR;
                }
                printf("%s\n", buffer);
                sendBroadcast(room, i, buffer);
                struct json_object *jsObj = json_tokener_parse(buffer);
                guessed = json_object_get_boolean(json_object_object_get(jsObj, "guessed"));
                if(guessed) {
                    break;
                }
            }
        }
        if(!guessed) {
            struct json_object *js_hint = generateHint(choosenWord, hints);
            if(js_hint != NULL) {
                sendBroadcast(room, -1, "NEW_HINT");
                sendBroadcast(room, -1, (char*) json_object_to_json_string(js_hint));
                free(js_hint);
            }
            else {
                sendBroadcast(room, -1, "HINT_END");
                winnerIndex = idHostPlayer;
            }
        }
        else {
            winnerIndex = i;
        }
    } while(winnerIndex == -1);
    free(hints);
    free(jsObj);
    printf("%s ha indovinato la parola\n", room->players[winnerIndex]->username);
    return winnerIndex;
}

void sendBroadcast(stanza* room, int idHost, char* msg) {
    int i;
    char response[BUFFDIM];
    memset(response, 0, sizeof(response));
    for(i = 0; i < room->numeroMaxGiocatori; i++){
        if(room->players[i] == NULL || i == idHost)
            continue;
        else {
            send(room->players[i]->clientSocket, msg, strlen(msg), 0);
            printf("waiting response from sock %d ... ", room->players[i]->clientSocket);
            recv(room->players[i]->clientSocket, response, BUFFDIM, 0);
            printf("%s\n", response);
        }   
    }
}

struct json_object *generateHint(char *parola, int *hints) {
    time_t t;
    srand((unsigned) time(&t));
    
    char c[1];
    int len = strlen(parola), position;
    do {
        position = rand()%len;
        if(hintsIsFull(hints, len)){
            return NULL;
        }
    } while(hints[position] == 1);
    hints[position] = 1;
    c[0] = parola[position];

    struct json_object *jsObj = json_object_new_object();
    json_object_object_add(jsObj, "letter", json_object_new_string(c));
    json_object_object_add(jsObj, "position", json_object_new_int(position));
    return jsObj;
}

int hintsIsFull(int *hints, int len) {
    int i = 0;
    for(i = 0; i < len; i++) {
        if(hints[i] == 0)
            return 0;
    }
    return 1;
}

void prepareWords(char dest[BUFFDIM], char *parole[NUMBER_OF_SUGGESTED_WORD][2]){
    int i;
    char key[64] = {0};
    char num[64] = {0};
    struct json_object *js_obj = json_object_new_object();
    for(i = 0; i < NUMBER_OF_SUGGESTED_WORD; i++){
        strcpy(key, "word");
        sprintf(num, "%d", i);
        strcat(key, num);
        struct json_object *js_array = json_object_new_array();
        json_object_array_add(js_array, json_object_new_string(parole[i][0]));
        json_object_array_add(js_array, json_object_new_string(parole[i][1]));
        json_object_object_add(js_obj, key, js_array);
    }
    strcpy(dest, json_object_to_json_string(js_obj));
    free(js_obj);
}

void thread_unlock(){}

#endif
