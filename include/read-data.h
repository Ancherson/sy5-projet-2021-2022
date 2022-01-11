#ifndef READ_REPLY_H
#define READ_REPLY_H
#define PIPEBUF 512

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <endian.h>


#include "timing.h"
#include "timing-text-io.h"
#include "string2.h"
#include "commandline.h"
#include "server-reply.h"


/* Ensemble de fonctions permettant la lecture de donnée à partir d'un descripteur */
int read_remove(int fd);
int read_create(int fd);
int read_terminate(int fd);
int read_stdout_stderr(int fd);
int read_list(int fd);
int read_times_exitcode(int fd);

int read_reptype (int fd);
uint16_t read_uint16(int fd);
uint32_t read_uint32(int fd);
uint64_t read_taskid(int fd);
timing read_timing(int fd);
char *read_string(int fd, uint32_t *l);
commandline read_commandline(int fd);


void print_commandline(commandline cmd);
void print_timing(timing t);
int print_time (int64_t time);


#endif // READ_REPLY_H