#include "string.h"

int create_string(struct string *dest, int len, char *str) {
    dest->len = len;
    dest -> str = str;
    return 0;
}