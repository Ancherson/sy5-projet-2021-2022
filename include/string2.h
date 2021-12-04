#ifndef STRING2_H
#define STRING2_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <string.h>

typedef struct string {
  uint32_t len;
  char *str;
} string; 

int create_string(string *dest, int len, char *str);
int write_string(char *buf, string str);

#endif // STRING2_H