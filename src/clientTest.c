#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <json-c/json.h>

#define PORT 5000

void play(int);
char *prepareStartGame();

char* prepareLogin(){
    char email[30];
    char password[30];
    printf("Inserisci email: ");
    scanf("%s", email);
    printf("Inserisci password: ");
    scanf("%s", password);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("login"));
    json_object_object_add(json, "email", json_object_new_string(email));
    json_object_object_add(json, "password", json_object_new_string(password));
    
    //free(json);
    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareSignup(){
    char email[30];
    char password[30];
    char username[30];
    printf("Inserisci username: ");
    scanf("%s", username);
    printf("Inserisci email: ");
    scanf("%s", email);
    printf("Inserisci password: ");
    scanf("%s", password);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("signup"));
    json_object_object_add(json, "email", json_object_new_string(email));
    json_object_object_add(json, "password", json_object_new_string(password));
    json_object_object_add(json, "username", json_object_new_string(username));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareGetAvatar(){
    char email[30];
    printf("Inserisci l'email dell'utente del quale si vuole l'avatar: ");
    scanf("%s", email);
    printf("%s", email);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("getAvatar"));
    json_object_object_add(json, "email", json_object_new_string(email));
    
    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareSetAvatar(){
    int avatarType = 0;
    char email[30];
    printf("Inserisci l'email dell'utente del quale si vuole settare l'avatar: ");
    scanf("%s", email);
    printf("Inserisci il tipo di avatar (0, 15): ");
    scanf("%d", &avatarType);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("setAvatar"));
    json_object_object_add(json, "email", json_object_new_string(email));
    json_object_object_add(json, "avatarType", json_object_new_int(avatarType));
    
    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;

}

char* prepareGetUserInfo(){
    char email[30];
    printf("Inserisci l'email dell'utente del quale si vogliono le informazioni: ");
    scanf("%s", email);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("getUserInfo"));
    json_object_object_add(json, "email", json_object_new_string(email));
    
    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareJoinRoom(){
    int idStanza;
    printf("Inserisci l id della stanza: ");
    scanf("%d", &idStanza);

    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("joinRoom"));
    json_object_object_add(json, "idStanza", json_object_new_int(idStanza));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;

}

char* prepareCreateRoom(){
    char nomeStanza[16];
    printf("Inserisci il nome della stanza da creare: ");
    scanf("%s", nomeStanza);

    int numeroMaxGiocatori;
    printf("Inserisci il numero massimo di giocatori: ");
    scanf("%d", &numeroMaxGiocatori);

    // Creare un oggetto JSON
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("createRoom"));
    json_object_object_add(json, "nomeStanza", json_object_new_string(nomeStanza));
    json_object_object_add(json, "numeroMaxGiocatori", json_object_new_int(numeroMaxGiocatori));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareQuitRoom(){
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("quitRoom"));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}

char* prepareSearchRoom(){
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "operation", json_object_new_string("searchRoom"));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}


char* prepareOperation(int op){
    //while(true){
        switch(op){
            case 1:
                return prepareLogin();
            break;
            case 2:
                return prepareSignup();
            break;
            case 3:
                return prepareGetAvatar();
            break;
            case 4:
                return prepareSetAvatar();
            break;
            case 5:
                return prepareGetUserInfo();
            break;
            case 6:
                return prepareJoinRoom();
            break;
            case 7:
                return prepareSearchRoom();
            break;
            case 8:
                return prepareCreateRoom();
            break;
            case 9:
                return prepareQuitRoom();
            break;
            case 10:
                return prepareStartGame();
            break;
            default:

            break;
        }
    //}

}

void play(int sock) {

    struct json_object *jsObj;
    char *jsonStr;
    char buffer[1024] = {0};
    char name[1024] = "utenteTest";

    ////////////////////////////////////////////////////

    while(1) {
        while(1) {
            memset(buffer, 0, sizeof(buffer));
            read(sock, buffer, 1024);
            printf("%s\n", buffer);
            if(strcmp(buffer, "CHOOSE") == 0) {
                send(sock, "OK", 3, 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sock, buffer, 1024, 0);
                printf("%s\n", buffer);
                printf("Scegli la parola: ");
                memset(buffer, 0, sizeof(buffer));
                scanf("%s", buffer);
                send(sock, buffer, strlen(buffer), 0);
                break;
            }
            else if(strcmp(buffer, "WAIT") == 0){
                send(sock, "OK", 3, 0);
                printf("Waiting for the word...\n");
                memset(buffer, 0, sizeof(buffer));
                recv(sock, buffer, 1024, 0);
                send(sock, "OK", 1024, 0);
                printf("%s\n", buffer);
                break;
            }
            else if(strcmp(buffer, "END") == 0) {
                send(sock, "OK", 3, 0);
                return;
            }  
        }
        printf("%s\n", "In attesa del tuo turno");
        while(1) {
            int guessed = 0;
            memset(buffer, 0, sizeof(buffer));
            recv(sock, buffer, 1024, 0);
            printf("%s\n", buffer);
            if(strcmp(buffer, "YOUR_TURN") == 0) {
                jsObj = json_object_new_object();
                printf("Tentativo: ");
                scanf("%s", buffer);
                if(strcmp(buffer, "parola0") == 0) {
                    guessed = 1;
                    json_object_object_add(jsObj, "guessed", json_object_new_boolean(guessed));
                    json_object_object_add(jsObj, "playerName", json_object_new_string(name));
                }
                else {
                    json_object_object_add(jsObj, "guessed", json_object_new_boolean(0));
                    json_object_object_add(jsObj, "playerName", json_object_new_string(name));
                    json_object_object_add(jsObj, "word", json_object_new_string(buffer));
                }
                strcpy(buffer, json_object_to_json_string(jsObj));
                printf("%s\n", buffer);
                send(sock, buffer, strlen(buffer), 0);
            }
            else if(strcmp(buffer, "NEW_HINT") == 0) {
                send(sock, "OK", 1024, 0);
                memset(buffer, 0, sizeof(buffer));
                recv(sock, buffer, 1024, 0);
                printf("%s", buffer);
                send(sock, "OK", 1024, 0);
            }
            else if(strcmp(buffer, "HINT_END") == 0) {
                send(sock, "OK", 1024, 0);
                guessed = 1;
            }
            else {
                jsObj = json_tokener_parse(buffer);
                guessed = json_object_get_boolean(json_object_object_get(jsObj, "guessed"));
                send(sock, "OK", 1024, 0);
            }
            if(guessed)
                break;
        }
        //memset(buffer, 0, sizeof(buffer));
    }
    //char response[1024];
    //read(sock, response, 1024);
    //printf("%s\n", response);
    return;
}

char *prepareStartGame(){
    struct json_object *json = json_object_new_object();
    int idStanza;
    printf("Inserisci il nome della stanza da iniziare: ");
    scanf("%d", &idStanza);
    json_object_object_add(json, "operation", json_object_new_string("startGame"));
    json_object_object_add(json, "idStanzaStarting", json_object_new_int(idStanza));

    // Convertire l'oggetto JSON in una stringa
    const char *jsonStr = json_object_to_json_string(json);
    return (char*)jsonStr;
}


int main(int argc, char *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *json = "";
    int op = 0;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    while(1) {
        printf("1. Login\n2. Signup\n3. GetAvatar\n4. SetAvatar\n5. GetUserInfo\n");
        printf("6. prepareJoinRoom\n7. prepareSearchRoom\n8. prepareCreateRoom\n9. prepareQuitRoom \n10. startGame\n");
        scanf("%d", &op);
        json = prepareOperation(op);
        if (send(sock, json, strlen(json), 0) == -1) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        printf("Message sent to server: %s\n", json);

        /* PARTE AGGIUNTA PER LA LOBBY */
        struct json_object *js = json_tokener_parse(json);
        const char *operation = json_object_get_string(json_object_object_get(js, "operation"));
        if(strcmp(operation, "joinRoom") == 0 || strcmp(operation, "startGame") == 0 || strcmp(operation, "createRoom") == 0){
            struct json_object *js2;
            if(strcmp(operation, "createRoom") == 0){
                read(sock, buffer, 1024);
                printf("Reply from server: %s\n", buffer);
                js2 = json_tokener_parse(buffer);
            }
            int idStanzaJoined = -1, idStanzaStarted = -1, idStanzaCreated = -1;
            if(strcmp(operation, "createRoom") == 0){
                idStanzaCreated = json_object_get_int(json_object_object_get(js2, "id"));
                idStanzaJoined = idStanzaCreated;
            }
                
            if(strcmp(operation, "joinRoom") == 0)
                idStanzaJoined = json_object_get_int(json_object_object_get(js, "idStanza"));
            if(strcmp(operation, "startGame") == 0)
                idStanzaStarted = json_object_get_int(json_object_object_get(js, "idStanzaStarting"));
            printf("%d, %d\n", idStanzaJoined, idStanzaStarted);
            while(!(idStanzaJoined == idStanzaStarted)){
                printf("In while");
                read(sock, buffer, 1024);
                printf("%s\n", buffer);
                memset(buffer, 0, sizeof buffer);
            }
        }else{
            if ((valread = read(sock, buffer, 1024)) == 0) {
                printf("Server disconnected.\n");
            }
            else {
                printf("Reply from server: %s\n", buffer);
            }
            memset(buffer, 0, sizeof buffer);
        }
        /*----------------------------*/

        /*if ((valread = read(sock, buffer, 1024)) == 0) {
            if(op == 6) {
                printf("Waiting for admin starting the game\n");
                play(sock);
            }
            else if(op == 10) {
                play(sock);
            }
            
            if ((valread = read(sock, buffer, 1024)) == 0) {
                printf("Server disconnected.\n");
            }
            else {
                printf("Reply from server: %s\n", buffer);
            }
        }*/
    }
    
    close(sock);
    
    return 0;
}