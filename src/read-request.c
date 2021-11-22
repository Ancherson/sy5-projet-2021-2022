#include "read-request.h"

int read_timing(int fd, timing *t){
    if(read(fd, t, TIMING_SIZE) < TIMING_SIZE){
        perror("Erreur read_timing");
        return 1;
    }
    t->minutes = reverse_byte64(t->minutes);
    t->hours = reverse_byte32(t->hours);
    return 0;
}

string *read_string(int fd){
    uint32_t len;
    if(read(fd, &len, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_string lecture de la taille de la string");
        return NULL;
    }
    string *s = create_string_alloc(len);
    if(s == NULL){
        perror("Erreur read_string create_string_alloc");
        return NULL;
    }
    if(read(fd, s->str, len) < len){
        perror("Erreur read_string");
        return NULL;
    }
    return s;
}