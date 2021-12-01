#include "string2.h"

int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}

int write_string(char * buf, string str){
    *((uint32_t *) buf) = htobe32(str.len);
    buf += sizeof(uint32_t);
    memcpy(buf, str.str, str.len);
    return sizeof(uint32_t)+str.len;
}