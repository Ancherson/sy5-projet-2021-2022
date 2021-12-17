#ifndef READ_REPLY_H
#define READ_REPLY_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <endian.h>


#include "timing.h"
#include "timing-text-io.h"
#include "string2.h"
#include "commandline.h"
#include "server-reply.h"

uint16_t read_uint16(int fd);
uint32_t read_uint32(int fd);
int read_remove(int fd);
int read_create(int fd);
uint64_t read_taskid(int fd);
timing read_timing(int fd);
char *read_string(int fd, uint32_t *l);
commandline read_commandline(int fd);
int read_reptype (int fd);
int read_terminate(int fd);
int read_stdout_stderr(int fd);
int read_list(int fd);

void print_commandline(commandline cmd);
void print_timing(timing t);
int print_time (int64_t time);
int read_times_exitcode(int fd);


#endif // READ_REPLY_H