#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include "read-reply.h"
#include "client-request.h"
#include <fcntl.h>
#include <errno.h>

int list(char *buf, task *t, uint32_t nbtasks);
int times_exitcodes(int fd, char *buf, task *t, int len);
void do_create(task t);
int create(int fd, char *buf, task **pt, int *len, int *nb_task);
int stdout_stderr(int fd, char *buf, task *t, int nb_task, uint16_t opcode);

#endif // EXECUTE_REQUEST_H