#ifndef READ_REQUEST_H
#define READ_REQUEST_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <endian.h>


#include "timing.h"
#include "timing-text-io.h"
#include "string2.h"
#include "commandline.h"
#include "server-reply.h"

int read_remove(int fd);
int read_create(int fd);
int read_taskid(int fd);
int read_timing(int fd);
int read_string(int fd);
int read_commandline(int fd);
int read_reptype (int fd);
int read_terminate(int fd);
int read_stdout_stderr(int fd);
int read_list(int fd);

int print_time (int64_t time);
int read_times_exitcode(int fd);


#endif // READ_REQUEST_H