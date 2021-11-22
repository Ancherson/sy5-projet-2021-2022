#ifndef STRING2_H
#define STRING2_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "convert-uint.h"

typedef struct string {
  uint32_t len;
  char *str;
} string; 

int create_string(string *dest, int len, char *str);
string *create_string_alloc(int len);
void free_string(string *s);
int write_string(int fd, string str);

#endif // STRING2_H