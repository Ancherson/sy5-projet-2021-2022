#include "string2.h"

int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}

int write_string(int fd, string str){
    uint32_t len = reverse_byte32(str.len);
    if (write(fd, &len, sizeof(uint32_t)) < sizeof(uint32_t))  return 1;

    if (write(fd, str.str, str.len) != str.len) return 1;
    return 0;
}