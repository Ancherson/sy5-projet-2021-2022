#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include "read-reply.h"
#include "client-request.h"
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

int list(char *buf, task *t, uint32_t nbtasks);
int create(int fd, char *buf, task **pt, int *len, int *nb_task, uint64_t *max_id);
int remove_(int fd, char *buf, task *t, int len, int *nb_task);
void times_exitcodes(int fd, int fd_reply, task *t, int len, uint64_t max_id);
void stdout_stderr(int fd, int fd_reply, task *t, int nb_tasks, uint16_t opcode, uint64_t max_id);
void execute_task(task t);

#endif // EXECUTE_REQUEST_H