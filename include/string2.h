#ifndef STRING2_H
#define STRING2_H

#include <stdint.h>
#include <unistd.h>

typedef struct string {
  uint32_t len;
  char *str;
} string; 

int create_string(string *dest, int len, char *str);
int write_string(int fd, string str);

#endif // STRING2_H