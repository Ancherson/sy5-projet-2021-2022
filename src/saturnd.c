#include "saturnd.h"

task *test(task *t, int *len, int *nb_tasks) {
    char *argv[] = {"echo", "test", "1235"};
    commandline c;
    alloc_commandline(&c, 3, argv);
    timing time = {1,1,1};
    return add_task(t, len, nb_tasks,1, c, time);
}

task *test2(task *t, int *len, int *nb_tasks) {
    char *argv[] = {"une", "commande", "pour", "tester", "si", "le", "malloc", "est", "bon"};
    commandline c;
    alloc_commandline(&c, 9, argv);
    timing time = {1,1,1};
    return add_task(t, len, nb_tasks,568, c, time);
}

task *test3(task *t, int *len, int *nb_tasks) {
    char *argv[] = {"je", "sais", "plus", "quoi", "mettre", "comme", "commande"};
    commandline c;
    alloc_commandline(&c, 7, argv);
    timing time = {1,1,1};
    return add_task(t, len, nb_tasks,4008, c, time);
}


void print_task(task *t) {
    printf("taskid = %lu\n", t->taskid);
    printf("commandline : ");
    for(int i = 0; i < t->cmd.argc; i++) {
        printf("%s ", t->cmd.argv[i].str);
    }
    printf("\n");
}

void print_task_array(task *t, int len) {
    for(int i = 0; i < len; i++) {
        print_task(t + i);
    }
}

int main(){
    int len = 3;
    int nb_task = 0;
    task *t = create_task_array(len);
    t = test(t, &len, &nb_task);
    t = test2(t, &len, &nb_task);
    t = test3(t, &len, &nb_task);
    remove_task(t, &nb_task, 1);
    t = test3(t, &len, &nb_task);
    printf("%d\n", len);
    t = test(t, &len, &nb_task);
    print_task_array(t, nb_task);
    //create(*t);

    int fd0 = open("run/pipes/saturnd-request-pipe", O_RDONLY);
    char read_buf[sizeof(uint16_t)];
    read(fd0, read_buf, sizeof(uint16_t));
    int fd = open("run/pipes/saturnd-reply-pipe", O_WRONLY);

    int n = 0;
    char buf[4096];
    n += write_opcode(buf, SERVER_REPLY_OK);
    n += list(buf+n, t, nb_task);

    write(fd, buf, n);

    free_task_array(t, &nb_task);

    return 0;
}
