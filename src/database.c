#include"../header/database.h"

utente* login(PGconn *conn, char email[32], char password[16], int socket, pthread_t tid){
	char queryString[512] = "SELECT * FROM utente WHERE email = '";
	strcat(queryString, email);
	strcat(queryString, "' AND passw = '");
	strcat(queryString, password);
	strcat(queryString, "'");
	
	printf("QUERY: %s \n", queryString);
	
	PGresult * res = PQexec(conn, queryString);
    
    if (PQresultStatus(res) == PGRES_TUPLES_OK){
        int row = PQntuples(res);
        if(row > 0){
            char *retrived_username = PQgetvalue(res, 0, 0);
            char *retrived_password = PQgetvalue(res, 0, 1);
            char *retrived_email = PQgetvalue(res, 0, 2);
            int retrived_partiteVinte = atoi(PQgetvalue(res, 0, 3));
            int imgId = atoi(PQgetvalue(res, 0, 4));
            PQclear(res);
            return new_utente(retrived_username, retrived_password, retrived_email, retrived_partiteVinte, socket, imgId, tid);
        }   
	}
    printf("Errore nella select\n");
	PQclear(res);
    return NULL;   
}

bool signUp(PGconn *conn, char username[32], char password[16], char email[64]){
	char queryString[512] = "INSERT INTO utente(username, passw, email, partitevinte) VALUES ('";
	strcat(queryString, username);
	strcat(queryString, "', '");
	strcat(queryString, password);
	strcat(queryString, "', '");
    strcat(queryString, email);
    strcat(queryString, "', 0)");
	
	printf("QUERY: %s \n", queryString);
	
	PGresult * res = PQexec(conn, queryString);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Errore nell'inserimento\n");
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

utente* getUserByEmail(PGconn *conn, char email[64], int socket, pthread_t tid){
	char queryString[512] = "SELECT * FROM utente WHERE email = '";
	strcat(queryString, email);
	strcat(queryString, "'");

	printf("QUERY: %s \n", queryString);
	
	PGresult * res = PQexec(conn, queryString);
    
    if (PQresultStatus(res) == PGRES_TUPLES_OK){
        int row = PQntuples(res);
        if(row > 0){
            char *retrived_username = PQgetvalue(res, 0, 0);
            char *retrived_password = PQgetvalue(res, 0, 1);
            char *retrived_email = PQgetvalue(res, 0, 2);
            int retrived_partiteVinte = atoi(PQgetvalue(res, 0, 3));
            int imgId = atoi(PQgetvalue(res, 0, 4));
            PQclear(res);
            return new_utente(retrived_username, retrived_password, retrived_email, retrived_partiteVinte, socket, imgId, tid);
        }   
	}
    printf("Errore nella select\n");
	PQclear(res);
    return NULL;   
}

bool updateUserPartiteVinte(PGconn *conn, char email[64]){
	char queryString[512] = "UPDATE utente SET partiteVinte = partiteVinte + 1";
	strcat(queryString, " WHERE email = '");
    strcat(queryString, email);
    strcat(queryString, "'");

	printf("QUERY: %s \n", queryString);
	
	PGresult * res = PQexec(conn, queryString);
    
    if (PQresultStatus(res) == PGRES_COMMAND_OK){
        PQclear(res);
        return true;
    }
    printf("Errore nell'update\n");
	PQclear(res);
    return false;   
}

char* getAvatarByEmail(PGconn *conn, char email[64], pthread_t tid){
    const char BASE_PATH_AVATAR[30] = "../../avatars/avatar";
    utente * tmp = getUserByEmail(conn, email, -1, tid);
    char buf[25];
	snprintf(buf, 12, "%d.png", tmp->imgId);
    char* imagebase64 = readFile(strcat((char*)BASE_PATH_AVATAR, buf));
    free(tmp);
    return imagebase64;
}

bool setAvatarOfUser(PGconn *conn, char email[64], int idAvatar){
    char queryString[512] = "UPDATE utente SET imgId = " ;
    char buf[20];
	snprintf(buf, 12, "%d", idAvatar);
    strcat(queryString, buf);
	strcat(queryString, " WHERE email = '");
    strcat(queryString, email);
    strcat(queryString, "'");

	printf("QUERY: %s \n", queryString);
	
	PGresult * res = PQexec(conn, queryString);
    
    if (PQresultStatus(res) == PGRES_COMMAND_OK){
        PQclear(res);
        return true;
    }
    printf("Errore nell'update\n");
	PQclear(res);
    return false;   
}



