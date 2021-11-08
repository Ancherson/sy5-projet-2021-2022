#include "string2.h"

int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}

int write_string(int fd, string str){
    if (write(fd, &str.len, sizeof(int)) <= 0)  return 1;

    if (write(fd, str.str, str.len *sizeof(char)) != (str.len *sizeof(char))) return 1;
    return 0;
}