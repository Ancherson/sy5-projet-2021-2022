#include "task.h"

/* Remplit les champs du pointeur de task t avec : le taskid, la commandline, et le timing */
void create_task(task *t, uint64_t taskid, commandline c, timing time) {
    t->cmd = c;
    t->taskid = taskid;
    t->time = time;
}

/* Permet de désallouer la mémoire allouée par la commandline */
void free_task(task *t) {
    free_commandline(&t->cmd);
}

/* Permet de créer un tableau de task en donnant la longueur len de ce tableau
   Cette fonction renvoie un tableau vide pour qu'il soit remplie plus tard
   Cette fonction permet surtout d'allouer la mémoire neccessaire */
task *create_task_array(int len) {
    task *t = malloc(sizeof(task) * len);
    if(t == NULL) {
        perror("malloc create_task_array");
        exit(1);
    }
    return t;
}

/* Permet de libérer la mémoire allouée
   A la fois la mémoire allouée par le tableau
   Et A la fois la mémoire allouée par les tasks */
void free_task_array(task *t, int *nb_tasks) {
    for(int i = 0; i < *nb_tasks; i++) {
        free_task(t + i);
    }
    *nb_tasks = 0;
    free(t);
}

/* Ajoute une task au tableau de task t */
task *add_task(task *t, int *len, int *nb_tasks,uint64_t taskid, commandline c, timing time) {
    // Si il y a la place pour mettre la tache, il suffit de la mettre et on incrémente le nombre de task
    if(*nb_tasks < *len) {
        create_task(t + *nb_tasks, taskid, c, time);
        *nb_tasks += 1;
        return t;
    }
    /* Si on n'a pas la place, on double la place à allouer et on place la tache dans le tableau 
       tout en incrémentant le nombre de tache et en mettant à jour la len du tableau */
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

/* Enleve la tache du tableau t, et "bouche le trou" en décalant les tâches
   renvoie 0 si tache trouvé,  1 pas trouvé */
int remove_task(task *t, int *nb_tasks, uint64_t taskid) {
    for(int i = 0; i < *nb_tasks; i++) {
        if(t[i].taskid == taskid) {
            free_task(t + i);
            *nb_tasks -= 1;
            /* On décale le restant des tâches vers la gauche */
            memmove(t + i, t + i + 1, sizeof(task) * (*nb_tasks - i));
            return 0;
        }
    }
    return 1;
}

/* Renvoie l'indice dans le tableau de tache t, de la tache a pour taskid : taskid
   renvoie l'indice si trouvé, -1 sinon */ 
int get_index(task *t, int nb_tasks, uint64_t taskid) {
    for(int i = 0; i < nb_tasks; i++) {
        if(t[i].taskid == taskid) return i;
    } 
    return -1;
}