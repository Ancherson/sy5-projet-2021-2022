#include "string2.h"

/* Initialise une string avec la longueur len et les caractères str */
int create_string(string *dest, int len, char *str) {
    dest->len = len;
    dest->str = str;
    return 0;
}

/* Ecrit la string str dans le buffer buf (avec l'attribut len converti en big endian)
 * Retourne le nombre d'octets écrits*/
int write_string(char * buf, string str){
    *((uint32_t *) buf) = htobe32(str.len);
    buf += sizeof(uint32_t);
    memcpy(buf, str.str, str.len);
    return sizeof(uint32_t)+str.len;
}

/* Alloue la mémoire pour le champ str d'une string et copie l'argument str dedans */
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

/* Libère la mémoire de l'attribut str de la string s*/
void free_string(string *s) {
    free(s->str);
}