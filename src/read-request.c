#include "read-request.h"

int read_remove(int fd) {
    int rep = read_reptype(fd);
    if(rep == 1 || rep == -1) {
        if(rep == 1) {
            printf("Erreur Requête Remove\n");
            uint16_t errcode;
            if(read(fd, &errcode, sizeof(uint16_t)) < sizeof(uint16_t)) {
                perror("Erreur read errcode read_remove");
            }
            if(be16toh(errcode) == SERVER_REPLY_ERROR_NOT_FOUND) {
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
    rep = read_taskid(fd);
    if(rep == 0) printf("\n");
    return rep;
}

int read_taskid(int fd){
    uint64_t taskid;
    if(read(fd, &taskid, sizeof(uint64_t)) < sizeof(uint64_t)){
        perror("Erreur read_taskid");
        return EXIT_FAILURE;
    }
    taskid = be64toh(taskid);
    printf("%lu", taskid);
    return EXIT_SUCCESS;
}

int read_timing(int fd){
    timing t;
    if(read(fd, &t, TIMING_SIZE) < TIMING_SIZE){
        perror("Erreur read_timing");
        return EXIT_FAILURE;
    }
    t.minutes = be64toh(t.minutes);
    t.hours = be32toh(t.hours);
    char s[TIMING_TEXT_MIN_BUFFERSIZE];
    if(timing_string_from_timing(s, &t) == 1) {
        printf("Erreur read_timing timing_string_from_timing\n");
        return EXIT_FAILURE;
    }
    printf("%s", s);
    return EXIT_SUCCESS;
}

int read_string(int fd){
    uint32_t len;
    if(read(fd, &len, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_string lecture de la taille de la string");
        return EXIT_FAILURE;
    }
    len = be32toh(len);

    char str[len + 1];
    if(read(fd, str, len) < len) {
        perror("read_string string");
        return EXIT_FAILURE;
    }
    str[len] = '\0';

    printf("%s", str);
    return EXIT_SUCCESS;
}

int read_commandline(int fd) {
    uint32_t argc;
    if(read(fd, &argc, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_commandline lecture du nombre d'argument");
        return EXIT_FAILURE;
    }
    argc = be32toh(argc);
    
    for(unsigned int i = 0; i < argc; i++) {
        if(read_string(fd)) {
            printf("Erreur read_commandline read_string\n");
            return EXIT_FAILURE;
        }
        printf(" ");
    }

    return EXIT_SUCCESS;
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
    uint16_t rep; 
    if (read(fd,&rep,sizeof(uint16_t)) != sizeof(uint16_t)){
        perror("read reptype");
        return -1;
    }
    rep = be16toh (rep);
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
        if(read_string(fd)) {
            printf("Erreur read_stdout_stderr read_string\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if(rep == 1){
        uint16_t errcode;
        if (read(fd, &errcode, sizeof(uint16_t)) < sizeof(uint16_t)){
            perror("Erreur read_stdout_stderr read errcode");
            return EXIT_FAILURE;
        }
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
    uint32_t nbtasks;
    if(read(fd, &nbtasks, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_list lecture du nombre de taches");
        return EXIT_FAILURE;
    }
    nbtasks = be32toh(nbtasks);

    for(unsigned int i = 0; i < nbtasks; i++){
        if(read_taskid(fd)){
            printf("Erreur read_list read_taskid\n");
            return EXIT_FAILURE;
        }
        printf(": ");
        if(read_timing(fd)){
            printf("Erreur read_list read_timing\n");
            return EXIT_FAILURE;
        }
        printf(" ");
        if(read_commandline(fd)){
            printf("Erreur read_list read_commandline\n");
            return EXIT_FAILURE;
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

int read_times_exitcode(int fd){
    if (read_reptype(fd) != 0){
        return EXIT_FAILURE;   
    }
    uint32_t nbRun;
    if (read(fd,&nbRun,sizeof(uint32_t)) != sizeof(uint32_t)){
        return EXIT_FAILURE;
    }
    nbRun = be32toh(nbRun);
    for (uint32_t i = 0; i<nbRun;i++){
        int64_t time;
        if (read(fd,&time,sizeof(int64_t)) != sizeof(int64_t)){
            perror ("echec read du time");
            return EXIT_FAILURE;
        }
        time = (int64_t) be64toh(time);
        print_time(time);
        uint16_t exitCode;
        if (read(fd,&exitCode,sizeof(uint16_t)) != sizeof(uint16_t)){
            perror ("echec read du exit code");
            return EXIT_FAILURE;
        }
        exitCode = be16toh(exitCode);
        printf(" %u\n",(unsigned int) exitCode);
    }
    return EXIT_SUCCESS;
}