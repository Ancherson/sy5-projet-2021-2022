#include "task.h"

void create_task(task *t, uint64_t taskid, commandline c, timing time) {

    t->cmd = c;
    t->taskid = taskid;
    t->time = time;
}

void free_task(task *t) {
    free_commandline(&t->cmd);
}

task *create_task_array(int len) {
    task *t = malloc(sizeof(task) * len);
    if(t == NULL) {
        perror("malloc create_task_array");
        exit(1);
    }
    return t;
}

void free_task_array(task *t, int *nb_tasks) {
    for(int i = 0; i < *nb_tasks; i++) {
        free_task(t + i);
    }
    *nb_tasks = 0;
    free(t);
}

task *add_task(task *t, int *len, int *nb_tasks,uint64_t taskid, commandline c, timing time) {
    if(*nb_tasks < *len) {
        create_task(t + *nb_tasks, taskid, c, time);
        *nb_tasks += 1;
        return t;
    }
    int oldlen = *len;
    *len *= 2;
    t = realloc(t, sizeof(task) * (*len));
    if(t == NULL) {
        perror("realloc add task");
        exit(1);
    }
    create_task(t + oldlen, taskid, c, time);   
    *nb_tasks += 1;
    return t;
}

// 0 si ça marche,  1 pas trouvé
int remove_task(task *t, int *nb_tasks, uint64_t taskid) {
    for(int i = 0; i < *nb_tasks; i++) {
        if(t[i].taskid == taskid) {
            free_task(t + i);
            *nb_tasks -= 1;
            memmove(t + i, t + i + 1, sizeof(task) * (*nb_tasks - i));
            return 0;
        }
    }
    return 1;
}

int task_exist(task *t, int nb_tasks, uint64_t taskid) {
    for(int i = 0; i < nb_tasks; i++) {
        if(t[i].taskid == taskid) return 1;
    } 
    return 0;
}