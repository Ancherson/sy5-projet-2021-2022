#include "string2.h"

int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}