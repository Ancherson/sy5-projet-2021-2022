#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "commandline.h"
#include "timing.h"

typedef struct task {
    uint64_t taskid;
    commandline cmd;
    timing time;
} task;

void create_task(task *t, uint64_t taskid, commandline c, timing time);
void free_task(task *t);
task *create_task_array(int len);
void free_task_array(task *t, int *nb_tasks);
task *add_task(task *t, int *len, int *nb_tasks,uint64_t taskid, commandline c, timing time);
int remove_task(task *t, int *nb_tasks, uint64_t taskid);

#endif // TASK_H