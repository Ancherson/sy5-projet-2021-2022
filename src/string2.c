#include "string2.h"

int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}

int write_string(int fd, string str){
    assert (write(fd, &str->len) > 0);
    assert (write(fd, str->str)== str-> len);

    return 0;
}