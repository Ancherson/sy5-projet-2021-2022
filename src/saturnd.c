#include "saturnd.h"

task *test(task *t, int *len) {
    char *argv[] = {"echo", "test", "1235"};
    commandline c;
    alloc_commandline(&c, 3, argv);
    timing time = {1,1,1};
    return add_task(t, len, 1, c, time);
}

task *test2(task *t, int *len) {
    char *argv[] = {"une", "commande", "pour", "tester", "si", "le", "malloc", "est", "bon"};
    commandline c;
    alloc_commandline(&c, 9, argv);
    timing time = {1,1,1};
    return add_task(t, len, 568, c, time);
}

task *test3(task *t, int *len) {
    char *argv[] = {"je", "sais", "plus", "quoi", "mettre", "comme", "commande"};
    commandline c;
    alloc_commandline(&c, 7, argv);
    timing time = {1,1,1};
    return add_task(t, len, 4008, c, time);
}


void print_task(task *t) {
    printf("taskid = %d\n", t->taskid);
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
    task *t = create_task_array(len);
    t = test(t, &len);
    t = test2(t, &len);
    t = test3(t, &len);
    remove_task(t, len, 1);
    t = test3(t, &len);
    t = test(t, &len);
    print_task_array(t, len);
    free_task_array(t, len);


    return 0;
}
