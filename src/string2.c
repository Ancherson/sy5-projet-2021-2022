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

void alloc_string(string *dest, int len, char *str) {
    dest->str = malloc(len + 1);
    if(dest->str == NULL) {
        perror("malloc alloc string");
        exit(1);
    }
    strcpy(dest->str, str);
    dest->str[len] = '\0';
    dest->len = len;
}

void free_string(string *s) {
    free(s->str);
}