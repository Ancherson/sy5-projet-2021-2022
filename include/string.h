#ifndef STRING_H
#define STRING_H

#include <stdint.h>

struct string {
  uint32_t len;
  char *str;
};

int create_string(struct string *dest, int len, char *str);

#endif // STRING_H