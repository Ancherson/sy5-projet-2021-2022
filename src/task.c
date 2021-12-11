#include "task.h"

void create_task(task *t, uint64_t taskid, commandline c, timing time) {

    t->cmd = c;
    t->taskid = taskid;
    t->time = time;
    t->alive = 1;
}

void free_task(task *t) {
    free_commandline(&t->cmd);
    t->alive = 0;
}

task *create_task_array(int len) {
    task *t = malloc(sizeof(task) * len);
    if(t == NULL) {
        perror("malloc create_task_array");
        exit(1);
    }

    for(int i = 0; i < len; i++) {
        t[i].alive = 0;
    }

    return t;
}

void free_task_array(task *t, int len) {
    for(int i = 0; i < len; i++) {
        if(t[i].alive) free_task(t + i);
    }
    free(t);
}

task *add_task(task *t, int *len, uint64_t taskid, commandline c, timing time) {
    for(int i = 0; i < *len; i++) {
        if(!t[i].alive) {
            create_task(t + i, taskid, c, time);
            return t;
        } 
    }
    *len *= 2;
    t = realloc(t, sizeof(task) * (*len));
    if(t == NULL) {
        perror("realloc add task");
    }
    for(int i = 0; i < *len; i++) {
        if(!t[i].alive) {
            create_task(t + i, taskid, c, time);
            return t;
        } 
    }   
    return t;
}

// 0 si ça marche,  1 pas trouvé
int remove_task(task *t, int len, uint64_t taskid) {
    for(int i = 0; i < len; i++) {
        if(t[i].alive && t[i].taskid == taskid) {
            free_task(t + i);
            return 0;
        }
    }
    return 1;
}