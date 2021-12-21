#include "saturnd.h"

void print_task(task *t) {
    printf("taskid = %lu\n", t->taskid);
    printf("commandline : ");
    for(int i = 0; i < t->cmd.argc; i++) {
        printf("%s ", t->cmd.argv[i].str);
    }
    printf("\n");
}

void print_task_array(task *t, int nb_tasks) {
    for(int i = 0; i < nb_tasks; i++) {
        print_task(t + i);
    }
}

int main(){
    char buf[BUFFER_SIZE];
    int nb_tasks = 0;
    int len = 1;
    task *t = create_task_array(len);

    // TODO Lecture des répertoires et ajouté les tasks save dans le tableau

    int fd_request = open("run/pipes/saturnd-request-pipe", O_RDONLY|O_NONBLOCK);
    if(fd_request == -1) {
        perror("open request");
        return EXIT_FAILURE;
    }
    int fd_gohst = open("run/pipes/saturnd-request-pipe", O_WRONLY);
    if(fd_gohst == -1) {
        perror("open request gohst");
        return EXIT_FAILURE;
    }

    int nfds = fd_request+1;
    fd_set read_set;

    while(1){
        struct timeval timeV;

        timeV.tv_sec = 10;
        timeV.tv_usec = 0;

        FD_ZERO(&read_set);
        FD_SET(fd_request,&read_set);

        int cond = select(nfds,&read_set,NULL,NULL,&timeV);
        if (cond == 0){
            printf("J'ai rien lu \n");
        }
        if (cond == -1) {
            perror("PB select saturnd");
            return 1;
        }
        if(FD_ISSET(fd_request, &read_set)){
            uint16_t op_code= read_uint16(fd_request);
            //TODO A ne plus hardcoder
            int fd_reply = open("run/pipes/saturnd-reply-pipe", O_WRONLY);
            if(fd_reply == -1) {
                perror("open reply");
                return EXIT_FAILURE;
            }
            int x = 0;
            switch (op_code){
                case CLIENT_REQUEST_LIST_TASKS :
                    x += write_opcode(buf, SERVER_REPLY_OK);
                    x += list(buf+x, t, nb_tasks);
                    break;
                
                case CLIENT_REQUEST_CREATE_TASK :            
                    x += create(fd_request,buf,&t,&len,&nb_tasks);
                    break;

                case CLIENT_REQUEST_REMOVE_TASK :
                    return 0;
                    break;
        
                case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    x += times_exitcodes(fd_request, buf, t, nb_tasks);
                    break;

                case CLIENT_REQUEST_TERMINATE :
                    return 0;
                    break;
                
                case CLIENT_REQUEST_GET_STDOUT :
                    x += stdout_stderr(fd_request, buf, t, nb_tasks, CLIENT_REQUEST_GET_STDOUT);
                    break;
    
                case CLIENT_REQUEST_GET_STDERR :
                    x += stdout_stderr(fd_request, buf, t, nb_tasks, CLIENT_REQUEST_GET_STDERR);
                    break;
                
                default:
                    return 1;
                    break;
                
            }
            if(write(fd_reply,buf, x) < x) {
                perror("write reply");
                return EXIT_FAILURE;
            }
            if(close(fd_reply) == -1) {
                perror("close reply");
                return EXIT_FAILURE;
            }
        }
    }

    free_task_array(t, &nb_tasks);
    if(close(fd_request) == -1) {
        perror("close request");
        return EXIT_FAILURE;
    }
    if(close(fd_gohst) == -1) {
        perror("close gohst");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
