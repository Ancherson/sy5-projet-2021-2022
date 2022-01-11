#ifndef STRING2_H
#define STRING2_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <string.h>

/* Structure représentant une chaîne de caractères 
 * len est la longueur de la chaîne
 * str est le tableau des caractères qu'elle contient*/
typedef struct string {
  uint32_t len;
  char *str;
} string; 
typedef struct string string;

/* Ensemble de fonctions permettant la manipulation d'une string */
int create_string(string *dest, int len, char *str);
int write_string(char *buf, string str);
void alloc_string(string *dest, int len, char *str);
void free_string(string *s);

#endif // STRING2_H