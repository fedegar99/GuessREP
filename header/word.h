#ifndef WORD_H
#define WORD_H
#include <netdb.h>

#include"database.h"

char* generateRandomWord();
void generateSuggestedWords(int numberOfWord, char* suggestedWord[numberOfWord][2]);
char* searchDefInDictionary(char* word);
void printSuggestedWords(int numberOfWord, char* suggestedWord[numberOfWord][2]);
#endif