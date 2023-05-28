#include"../header/word.h"

char* extractWord(char* s, char* pattern1, char* pattern2){
    char *target = NULL;
    char *start, *end;

    if (start = strstr(s, pattern1)){
        start += strlen(pattern1);
        if (end = strstr(start, pattern2)){
            target = (char *)malloc(end - start + 1);
            memcpy(target, start, end - start);
            target[end - start] = '\0';
        }
    }
    if (target) printf("Extracted: %s\n", target);
    return target;
}

char* generateRandomWord(){
    struct addrinfo hints, *res;
    int sockfd;
    
    char buf[2056];
    int byte_count;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("random-word-api.herokuapp.com","80", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    char* header =  "GET /word?lang=en HTTP/1.1\r\n"
                    "Host: random-word-api.herokuapp.com\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n";
    send(sockfd, header, strlen(header), 0);
    byte_count = recv(sockfd, buf, sizeof(buf)-1, 0); 
    buf[byte_count] = 0; 

    char* parola = extractWord(buf, "[\"", "\"]");
    if(parola != NULL){
        return parola;
    }else{
        generateRandomWord();
    }    
}

char* searchDefInDictionary(char* word){
    struct addrinfo hints, *res;
    int sockfd;
    
    char buf[2056] = {};
    int byte_count;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("api.dictionaryapi.dev","80", &hints, &res);
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    char header[255] = "GET /api/v2/entries/en/";
    strcat(header, word);
    strcat(header, " HTTP/1.1\r\n"
                    "Host: api.dictionaryapi.dev\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n");
    send(sockfd, header, strlen(header), 0);
    byte_count = recv(sockfd, buf, sizeof(buf)-1, 0); 
    buf[byte_count] = 0; 

    return extractWord(buf, "[{\"definition\":\"", "\",");
   
}

void generateSuggestedWords(int numberOfWord, char* suggestedWord[numberOfWord][2]){
    
    for(int i = 0; i < numberOfWord; i++){
        char* word = NULL;
        char* description = NULL;
        while(description == NULL){
            word = generateRandomWord();
            description = searchDefInDictionary(word);
        }
        suggestedWord[i][0] = word;
        suggestedWord[i][1] = description;
    }
    /*
    int i;
    for(i = 0; i < numberOfWord; i++){
        suggestedWord[i][0] = (char*)malloc(32);
        suggestedWord[i][1] = (char*)malloc(1024);
        char n[4];
        sprintf(n, "%d", i);
        char p[32] = "parola";
        strcat(p, n);
        char d[1024] = "definizione";
        strcat(d, n);
        strcpy(suggestedWord[i][0], p);
        strcpy(suggestedWord[i][1], d);
    }
    */
}

void printSuggestedWords(int numberOfWord, char* suggestedWord[numberOfWord][2]){
    for(int i = 0; i< numberOfWord; i++){
        printf("Word: %s\nDescription: %s\n", suggestedWord[i][0], suggestedWord[i][1]);
    }
}