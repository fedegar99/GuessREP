#include "../header/database.h"
#include "../header/word.h"
#include "../header/main.h"
#include "../header/gameManager.h"
// sudo apt install libjson-c-dev
#include <json-c/json.h>
#include <sys/wait.h>

void checkConnectionToDb(PGconn *conn){
	if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
    }else{
		printf("Successful connected to database [V]\n");
	}
}

void sendResponse(struct json_object *json, int socket){
    const char *jsonStr = json_object_to_json_string(json);

    printf("%ld JSON response: %s\n", strlen(jsonStr), jsonStr);
    if(send(socket, jsonStr, strlen(jsonStr), 0) == -1) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    send(socket, "\n", 1, 0);
}

typedef struct thread_par {
    int new_socket;
    PGconn* conn;
} thread_par;

void *handle2_client(void *par_) {
    utente* utenteLoggato = NULL;
    stanza* stanzaAttuale = NULL;
    thread_par par = *(thread_par*)par_;
    int socket = par.new_socket;
    PGconn * conn = par.conn;
    char buffer[2048] = {0};

    printf("New client connected.\n");

    while (1) {
        visualizza_stanze();

        if ((read(socket, buffer, 2048)) < 1) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received message from client: %s\n", buffer);

        struct json_object *js = json_tokener_parse(buffer);
        const char *operation = json_object_get_string(json_object_object_get(js, "operation"));
        struct json_object *json = json_object_new_object();

		if(strcmp(operation, "login") == 0){
			char* email = (char*) json_object_get_string(json_object_object_get(js, "email"));
			char* password = (char*) json_object_get_string(json_object_object_get(js, "password"));
			utenteLoggato = login(conn, email, password, socket, pthread_self());
            if(utenteLoggato != NULL){
                printf("Login avvenuto con successo!\n");
                json_object_object_add(json, "logged", json_object_new_string("true"));

            }else{
                printf("Login fallito\n");
                json_object_object_add(json, "logged", json_object_new_string("false"));
            }
            sendResponse(json, socket);
		}
		else if(strcmp(operation, "signup") == 0){
			char* email = (char*) json_object_get_string(json_object_object_get(js, "email"));
			char* password = (char*) json_object_get_string(json_object_object_get(js, "password"));
			char* username = (char*) json_object_get_string(json_object_object_get(js, "username"));
            if(signUp(conn, username, password, email)){
                printf("Signup avvenuto con successo!\n");
                json_object_object_add(json, "signed", json_object_new_string("true"));
            }else{
                printf("Signup fallito\n");
                json_object_object_add(json, "signed", json_object_new_string("false"));
            }
            sendResponse(json, socket);
		}
        else if(strcmp(operation, "getAvatar") == 0){
            char* email = (char*) json_object_get_string(json_object_object_get(js, "email"));
            char* base64image_encoded = getAvatarByEmail(conn, email,  pthread_self());

            json_object_object_add(json, "email", json_object_new_string(email));
            json_object_object_add(json, "avatarBase64", json_object_new_string(base64image_encoded)); //al client arriverà con dei backslash in più base64image_encoded
            sendResponse(json, socket);
        }
        else if(strcmp(operation, "setAvatar") == 0){
            char* email = (char*) json_object_get_string(json_object_object_get(js, "email"));
            int avatarType = (int) json_object_get_int(json_object_object_get(js, "avatarType"));
            bool result = setAvatarOfUser(conn, email, avatarType);

            json_object_object_add(json, "email", json_object_new_string(email));
            json_object_object_add(json, "isSuccess", json_object_new_boolean(result));
            sendResponse(json, socket);
        }
        else if(strcmp(operation, "getUserInfo") == 0){
            char* email = (char*) json_object_get_string(json_object_object_get(js, "email"));
            utente* u = getUserByEmail(conn, email, socket, pthread_self());

            json_object_object_add(json, "email", json_object_new_string(email));
            json_object_object_add(json, "idStanza", json_object_new_int(u->idStanza));
            json_object_object_add(json, "imgId", json_object_new_int(u->imgId));
            json_object_object_add(json, "partiteVinte", json_object_new_int(u->partiteVinte));
            json_object_object_add(json, "username", json_object_new_string(u->username));
            free(u);

			sendResponse(json, socket);
			printf("\neseguo getUserInfo\n");

        }
        else if(strcmp(operation, "joinRoom") == 0){
            int idStanza = (int) json_object_get_int(json_object_object_get(js, "idStanza"));
            stanza* stanza = get_stanza_by_id(idStanza);
            bool result = add_user_in_room(utenteLoggato, stanza);
            stanzaAttuale = stanza;
            json_object_object_add(json, "isSuccess", json_object_new_boolean(result));
            sendResponse(json, socket);
        }
        else if(strcmp(operation, "searchRoom") == 0){
            // stanza * stanzeTmp[50];
            struct json_object * jsonArray = json_object_new_array();
            for(int i = 0; i < stanze_lenght; i++){
		        if(stanze[i] != NULL && stanze[i]->started == false){
                    struct json_object * jsonStanza = json_object_new_object();

                    json_object_object_add(jsonStanza, "id", json_object_new_int(i));
                    json_object_object_add(jsonStanza, "numeroMaxGiocatori", json_object_new_int(stanze[i]->numeroMaxGiocatori));
                    json_object_object_add(jsonStanza, "actualPlayersNumber", json_object_new_int(get_number_of_player_in_stanza(i)));
                    json_object_object_add(jsonStanza, "nomeStanza", json_object_new_string(stanze[i]->nomeStanza));

                    json_object_array_add(jsonArray, jsonStanza);
		        }
	        }
            json = jsonArray;
            //strcpy(json, json_object_to_json_string(jsonArray));
            //printf("%s",json);
            sendResponse(json, socket);
        }
        else if(strcmp(operation, "createRoom") == 0){
            char* nomeStanza = (char*) json_object_get_string(json_object_object_get(js, "nomeStanza"));
            int numeroMaxGiocatori = (int) json_object_get_int(json_object_object_get(js, "numeroMaxGiocatori"));

            int id = add_stanza(nomeStanza, numeroMaxGiocatori, utenteLoggato);
            stanzaAttuale = get_stanza_by_id(id);
            printf("stanza creata: %s\n",stanzaAttuale->nomeStanza);
            json_object_object_add(json, "id", json_object_new_int(id));

            sendResponse(json, socket);
        }
        else if(strcmp(operation, "quitRoom") == 0){
            bool result = rm_user_from_room(utenteLoggato, stanzaAttuale);
            json_object_object_add(json, "isSuccess", json_object_new_boolean(result));
            stanzaAttuale = NULL;
            sendResponse(json, socket);
        }
        else if(strcmp(operation, "startGame") == 0){
            char winnerEmail[64];
            char* email;
            bool result = false;
            stanza *currentRoom = get_stanza_by_id(utenteLoggato->idStanza);
            if(currentRoom != NULL) {
                currentRoom->started = true; //moved down
                result = true;
            }
            json_object_object_add(json, "isSuccess", json_object_new_boolean(result));
            sendResponse(json, socket);
            sleep(3);
            //Svolgimento del gioco
            if(result) {
                //wait_until_ready(currentRoom,utenteLoggato);
                //currentRoom->tmp = true;
                currentRoom->started = true;
                email = start_room(get_stanza_by_id(utenteLoggato->idStanza), 2);
            }
            //Fine del gioco
            if(email != NULL) {
                strcpy(winnerEmail, email);
                updateUserPartiteVinte(conn, winnerEmail);
            }
            rm_stanza(currentRoom);
            currentRoom = NULL;
        }
        else if(strcmp(operation, "updateLobby") == 0){
            struct json_object * jsonArray = json_object_new_array();
            bool isAdminExited = true;

            if(stanzaAttuale != NULL && utenteLoggato->idStanza != -1){
                printf("stanza attuale != NULL\n");
                for(int i = 0; i < stanzaAttuale->numeroMaxGiocatori; i++){
                    if(stanzaAttuale->players[i] != NULL){
                        if(stanzaAttuale->players[i] == stanzaAttuale->adminUser){
                            isAdminExited = false;
                        }
                        struct json_object * jsonUser = json_object_new_object();
                        json_object_object_add(jsonUser, "username", json_object_new_string(stanzaAttuale->players[i]->username));
                        json_object_array_add(jsonArray, jsonUser);
                    }
                }
            }
            if(isAdminExited){
                stanzaAttuale = NULL;
                json_object_object_add(json, "isAdminExited", json_object_new_boolean(isAdminExited));
            }else{
                json_object_object_add(json, "usersInLobby", jsonArray);
                json_object_object_add(json, "isGameStarted", json_object_new_boolean(stanzaAttuale->started)); //temporaneo, non so come capire se il gioco è iniziato
            }
            printf("stiamo per mandare %s", json_object_to_json_string(json));
            sendResponse(json, socket);
            
            //wait_until_ready(stanzaAttuale,utenteLoggato);
            //{isAdminExited: true} oppure { usersInLobby[{username: user1}, {username: user2}...], isGameStarted: true/false }

            if(stanzaAttuale != NULL && !isAdminExited && stanzaAttuale->started) {
                signal(SIGUSR1, thread_unlock);
                pause();
                printf("tread unlocked for %s\n", utenteLoggato->username);
                stanzaAttuale = NULL;
            }
        }

        //free(json);

        printf("\n\n");
        memset(buffer, 0, sizeof(buffer));
    }

    close(socket);

    return NULL;
}

int main(int argc, char *argv[]) {

    int status, pid;

    do {
        pid = fork();
        if(pid > 0) {
            waitpid(pid, &status, 0);
            if (WIFSIGNALED(status)) {
                printf("Server down - ERROR %d\n", status);
                // It was terminated by a signal
                if (WTERMSIG(status) == SIGSEGV) {
                    printf("SEG FAULT\n");
                }
            }
            printf("Restarting server...\n");
        }
        else {
            pthread_mutex_init(&mutex, NULL);
            int server_fd;
            struct sockaddr_in address;
            int addrlen = sizeof(address);
            pthread_t thread_id;
            thread_par par;
            par.conn = PQconnectdb("postgresql://user:admin@localhost:5432/postgres"); //sudo docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' container_name
            checkConnectionToDb(par.conn);

            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }

            //CODICE FEDE consente al socket di riutilizzare l'indirizzo e la porta
            int reuse = 1;
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
                perror("Errore durante l'impostazione dell'opzione SO_REUSEADDR");
                exit(1);
            }

            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            //address.sin_addr.s_addr = inet_addr("127.0.0.1");
            address.sin_port = htons(PORT);

            if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }

            if (listen(server_fd, MAX_CLIENTS) < 0) {
                perror("listen failed");
                exit(EXIT_FAILURE);
            }

            printf("Server started listening on port %d...\n", PORT);

            while (1) {
                if ((par.new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                    perror("accept failed");
                    exit(EXIT_FAILURE);
                }

                if (pthread_create(&thread_id, NULL, handle2_client, (void *)&par) < 0) {
                    perror("pthread_create failed");
                    exit(EXIT_FAILURE);
                }
            }
            pthread_mutex_destroy(&mutex);
            exit(EXIT_SUCCESS);
        }
    } while(1);
}


