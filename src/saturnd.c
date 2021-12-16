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
    printf("taskid = %lu\n", t->taskid);
    printf("commandline : ");
    for(int i = 0; i < t->cmd.argc; i++) {
        printf("%s ", t->cmd.argv[i].str);
    }
    printf("\n");
}

void print_task_array(task *t, int len) {
    for(int i = 0; i < len; i++) {
        if(t[i].alive) print_task(t + i);
    }
}

int main(){

    int len = 1;
    task *t = create_task_array(len);

    // int len = 3;
    // task *t = create_task_array(len);
    // t = test(t, &len);
    // t = test2(t, &len);
    // t = test3(t, &len);
    // remove_task(t, len, 1);
    // t = test3(t, &len);
    // t = test(t, &len);

    // TODO Lecture des répertoires et ajouté les tasks save dans le tableau

    int fd0 = open("run/pipes/saturnd-request-pipe", O_RDONLY|O_NONBLOCK);
    int fd_gohst = open("run/pipes/saturnd-request-pipe", O_WRONLY);

    int nfds = fd0+1;

    fd_set fd0_set;



    while(1){
        struct timeval timeV;
        struct timeval * tv = &timeV;

        tv->tv_sec = 10;
        tv->tv_usec = 0;

        FD_ZERO(&fd0_set);
        FD_SET(fd0,&fd0_set);

        int cond = select(nfds,&fd0_set,NULL,NULL,tv);
        if (cond == 0){
            printf("J'ai rien lu \n");
        }
        if (cond == -1) {
            perror("PB select saturnd");
            return 1;
        }
        if(FD_ISSET(fd0, &fd0_set)){
            uint16_t op_code= read_uint16(fd0);
            switch (op_code){
                
                case CLIENT_REQUEST_LIST_TASKS :{
                    int fd1 = open("run/pipes/saturnd-reply-pipe", O_WRONLY);
                    int x = 0;
                    char buf[4096];
                    x += write_opcode(buf, SERVER_REPLY_OK);
                    x += list(buf+x, t, len);
                    write(fd1, buf, x);
                    close(fd1);
                    break;
                }
                case CLIENT_REQUEST_CREATE_TASK :{
                     
                    int fd1 = open("run/pipes/saturnd-reply-pipe", O_WRONLY);
                    task **pt = &t;
                    char buf[sizeof(uint16_t)+sizeof(uint64_t)];
                                       

                    int x = create(fd0,buf,pt,&len);
                    write(fd1,buf, x);
                    close(fd1);
                    break;
                }

                case CLIENT_REQUEST_REMOVE_TASK :{
                    return 0;
                    break;
                }

                case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :{
                    return 0;
                    break;
                }
                case CLIENT_REQUEST_TERMINATE :{
                    return 0;
                    break;
                }
                case CLIENT_REQUEST_GET_STDOUT :{
                    return 0;
                    break;   
                }
                case CLIENT_REQUEST_GET_STDERR :{
                    return 0;
                    break;  
                }
                default:{
                    return 1;
                    break;
                }
            }
        }
    }


    // int len = 3;
    // task *t = create_task_array(len);
    // t = test(t, &len);
    // t = test2(t, &len);
    // t = test3(t, &len);
    // remove_task(t, len, 1);
    // t = test3(t, &len);
    // t = test(t, &len);
    // print_task_array(t, len);
    // //create(*t);

    // int fd0 = open("run/pipes/saturnd-request-pipe", O_RDONLY);
    // char read_buf[sizeof(uint16_t)];
    // read(fd0, read_buf, sizeof(uint16_t));
    
    // int fd = open("run/pipes/saturnd-reply-pipe", O_WRONLY);

    // int n = 0;
    // char buf[4096];
    // n += write_opcode(buf, SERVER_REPLY_OK);
    // n += list(buf+n, t, len);
    // write(fd, buf, n);

    // free_task_array(t, len);

    return 0;
}
