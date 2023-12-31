#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H
#define BUFFER_SIZE 32768
#define PIPEBUF 512

#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include<endian.h>

#include "timing-text-io.h"
#include "commandline.h"

/* Ensemble de fonctions qui permettent l'écriture de données */

int write_opcode(char * buf, uint16_t opcode);
int write_timing_from_strings(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str);
int write_timing(char * buf, timing t);
int write_taskid(char * buf, uint64_t taskid);
int write_create(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv);

void write_pipebuf(int fd, char *buf, int len);

#endif // WRITEREQUEST_H