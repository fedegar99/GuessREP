#ifndef BASE46_H
#define BASE46_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char* readFile(const char *name);
char* base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);

#endif
