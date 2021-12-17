#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include "read-reply.h"
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

int list(char *buf, task *t, uint32_t nbtasks);
int create(int fd, char *buf, task **pt, int *len, int *nb_task);
int remove_(int fd, char *buf, task *t, int len, int *nb_task);

#endif // EXECUTE_REQUEST_H