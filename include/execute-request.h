#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include "read-reply.h"
#include <fcntl.h>
#include <errno.h>

void do_create(task t);
int create(int fd, char *buf, task **t, int *len);
int list(char *buf, task *t, int len);

#endif // EXECUTE_REQUEST_H