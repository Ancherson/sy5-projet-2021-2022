#ifndef RUN_H
#define RUN_H

#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include "read-reply.h"
#include "client-request.h"
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

void execute_task(task t);

#endif