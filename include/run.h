#ifndef RUN_H
#define RUN_H

#include "task.h"
#include <sys/stat.h>
#include "write-data.h"
#include "read-data.h"
#include "client-request.h"
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

void execute_task(task t);
void launch_executable_tasks(task *t, int nb_tasks);
task *init_task(int *len, int *nb_task, uint64_t *max_id);
void clean_defunct();

#endif