#include "read-reply.h"

int read_remove(int fd) {
    int rep = read_reptype(fd);
    if(rep == 1 || rep == -1) {
        if(rep == 1) {
            printf("Erreur Requête Remove\n");
            uint16_t errcode = read_uint16(fd);
            if(errcode == SERVER_REPLY_ERROR_NOT_FOUND) {
                printf("Erreur Task Not Found : Requête Remove\n");
            }
        } 
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int read_create(int fd) {
    int rep = read_reptype(fd);
    if(rep == 1 || rep == -1) {
        if(rep == 1) printf("Erreur Requête Create\n");
        return EXIT_FAILURE;
    }
    uint64_t taskid = read_taskid(fd);
    printf("%lu\n", taskid);
    return EXIT_SUCCESS;
}

uint64_t read_taskid(int fd){
    uint64_t taskid;
    if(read(fd, &taskid, sizeof(uint64_t)) < sizeof(uint64_t)){
        perror("Erreur read_taskid");
        exit(EXIT_FAILURE);
    }
    taskid = be64toh(taskid);
    return taskid;
}

timing read_timing(int fd){
    timing t;
    if(read(fd, &t, TIMING_SIZE) < TIMING_SIZE){
        perror("Erreur read_timing");
        exit(EXIT_FAILURE);
    }
    t.minutes = be64toh(t.minutes);
    t.hours = be32toh(t.hours);
    return t;
}

void print_timing(timing t) {
    char s[TIMING_TEXT_MIN_BUFFERSIZE];
    if(timing_string_from_timing(s, &t) == 1) {
        printf("Erreur read_timing timing_string_from_timing\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", s);
}

char *read_string(int fd, uint32_t *l){
    uint32_t len = read_uint32(fd);

    char *str = malloc(len + 1);
    if(str == NULL) {
        perror("malloc read string");
        exit(EXIT_FAILURE);
    }
    if(read(fd, str, len) < len) {
        perror("read_string string");
        exit(EXIT_FAILURE);
    }
    str[len] = '\0';
    *l = len;
    return str;
    //printf("%s", str);
    //return EXIT_SUCCESS;
}



commandline read_commandline(int fd) {
    uint32_t argc = read_uint32(fd);
    commandline cmd;
    alloc_commandline_incomplete(&cmd, argc);
    for(unsigned int i = 0; i < argc; i++) {
        cmd.argv[i].str = read_string(fd, &cmd.argv[i].len);
    }
    return cmd;
}

void print_commandline(commandline cmd) {
    for(uint32_t i = 0; i < cmd.argc; i++) {
        printf("%s ", cmd.argv[i].str);
    }
}

int print_time (int64_t time){
  struct tm  ts;
  char buf[80];

  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  ts = *localtime(&time);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
  printf("%s", buf);
  return EXIT_SUCCESS;
}

/* -1 if fail
    1 if it's an 'ER' REPTYPE
    0 if it's an 'OK' REPTYPE */
int read_reptype (int fd){
    uint16_t rep = read_uint16(fd);
    if (rep == SERVER_REPLY_OK) return EXIT_SUCCESS;
    else return EXIT_FAILURE;
}

int read_terminate(int fd) {
    int rep = read_reptype(fd);
    if(rep != 0) {
        printf("Erreur read_terminate Requête\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int read_stdout_stderr(int fd){
    int rep = read_reptype(fd);
    if(rep == -1){
        printf("Erreur read_stdout_stderr read_reptype\n");
        return EXIT_FAILURE;
    }
    if(rep == 0){
        uint32_t len;
        char *str = read_string(fd, &len);
        printf("%s", str);
        free(str);
        return EXIT_SUCCESS;
    }
    if(rep == 1){
        uint16_t errcode = read_uint16(fd);
        if(errcode == SERVER_REPLY_ERROR_NOT_FOUND){
            printf("Erreur: il n'existe aucune tâche avec cet identifiant\n");
        } else if(errcode == SERVER_REPLY_ERROR_NEVER_RUN){
            printf("Erreur: la tâche n'a pas encore été exécutée au moins une fois\n");
        } else {
            printf("Erreur read_stdout_stderr errcode anormal\n");
        }
        return EXIT_FAILURE;
    }
    printf("Erreur read_stdout_stderr reptype anormal\n");
    return EXIT_FAILURE;
}

int read_list(int fd){
    if(read_reptype(fd) != 0){
        printf("Erreur read_list reptype anormal\n");
        return EXIT_FAILURE;
    }
    uint32_t nbtasks = read_uint32(fd);

    for(unsigned int i = 0; i < nbtasks; i++){
        uint64_t taskid = read_taskid(fd);
        printf("%lu", taskid);
        printf(": ");
        timing t = read_timing(fd);
        print_timing(t);
        printf(" ");
        commandline cmd = read_commandline(fd);
        print_commandline(cmd);
        free_commandline(&cmd);
        printf("\n");
    }
    return EXIT_SUCCESS;
}

int read_times_exitcode(int fd){
    if (read_reptype(fd) != 0){
        return EXIT_FAILURE;   
    }
    uint32_t nbRun = read_uint32(fd);
    for (uint32_t i = 0; i<nbRun;i++){
        int64_t time;
        if (read(fd,&time,sizeof(int64_t)) != sizeof(int64_t)){
            perror ("echec read du time");
            return EXIT_FAILURE;
        }
        time = (int64_t) be64toh(time);
        print_time(time);
        uint16_t exitCode = read_uint16(fd);
        printf(" %u\n",(unsigned int) exitCode);
    }
    return EXIT_SUCCESS;
}

uint16_t read_uint16(int fd){
    uint16_t rep; 
    if (read(fd,&rep,sizeof(uint16_t)) != sizeof(uint16_t)){
        perror("echec read uint16");
        exit(1);
    }
    return be16toh (rep);
}

uint32_t read_uint32(int fd){
    uint32_t rep; 
    if (read(fd,&rep,sizeof(uint32_t)) != sizeof(uint32_t)){
        perror("echec read uint32");
        exit(1);
    }
    return be32toh (rep);
}
