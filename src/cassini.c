#include "cassini.h"

const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes)\n\
";

int get_pipes_file(char *pipes_directory, char **pipe_request, char **pipe_reply) {
  //Si le pipes_directory n'est pas précisé, on choisi /tmp/USER/saturnd/pipes
  if(pipes_directory == NULL) {
    char *user = getenv("USER");
    char *dir_pre  = "/tmp/";
    char *dir_next = "/saturnd/pipes";
    pipes_directory = malloc(strlen(dir_pre) + strlen(user) + strlen(dir_next) + 1);
    if(pipes_directory == NULL) {
      perror("PBM malloc pipes_directory");
      return 1;
    }
    strcpy(pipes_directory, dir_pre);
    strcat(pipes_directory, user);
    strcat(pipes_directory, dir_next);
    pipes_directory[strlen(dir_pre) + strlen(user) + strlen(dir_next)] = '\0';
  }

  //Creation des chaines de caractères pour l'ouverture des FIFO
  char *str_pipe_request = "saturnd-request-pipe";
  char *str_pipe_reply = "staturnd-reply-pipe";

  char *pipe_request_file = malloc(strlen(pipes_directory) + strlen(str_pipe_request) + 2);
  if(pipe_request_file == NULL) {
    perror("PBM malloc pipe_request_file");
    free(pipes_directory);
    return 1;
  }

  char *pipe_reply_file = malloc(strlen(pipes_directory) + strlen(str_pipe_reply) + 2);
  if(pipe_request_file == NULL) {
    perror("PBM malloc pipe_reply_file");
    free(pipes_directory);
    free(pipe_request_file);
    return 1;
  }

  strcpy(pipe_request_file,pipes_directory);
  strcat(pipe_request_file, "/");
  strcat(pipe_request_file,str_pipe_request);
  pipe_request_file[strlen(pipes_directory) + strlen(str_pipe_request) + 1] = '\0';

  strcpy(pipe_reply_file,pipes_directory);
  strcat(pipe_reply_file, "/");
  strcat(pipe_reply_file,str_pipe_reply);
  pipe_reply_file[strlen(pipes_directory) + strlen(str_pipe_reply) + 1] = '\0';

  free(pipes_directory);

  *pipe_request = pipe_request_file;
  *pipe_reply = pipe_reply_file;

  return 0;
}

int main(int argc, char * argv[]) {
  errno = 0;
  
  char * minutes_str = "*";
  char * hours_str = "*";
  char * daysofweek_str = "*";
  char * pipes_directory = NULL;
  
  uint16_t operation = CLIENT_REQUEST_LIST_TASKS;
  uint64_t taskid;
  
  int opt;
  char * strtoull_endp;
  while ((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
    switch (opt) {
    case 'm':
      minutes_str = optarg;
      break;
    case 'H':
      hours_str = optarg;
      break;
    case 'd':
      daysofweek_str = optarg;
      break;
    case 'p':
      pipes_directory = strdup(optarg);
      if (pipes_directory == NULL) goto error;
      break;
    case 'l':
      operation = CLIENT_REQUEST_LIST_TASKS;
      break;
    case 'c':
      operation = CLIENT_REQUEST_CREATE_TASK;
      break;
    case 'q':
      operation = CLIENT_REQUEST_TERMINATE;
      break;
    case 'r':
      operation = CLIENT_REQUEST_REMOVE_TASK;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'x':
      operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'o':
      operation = CLIENT_REQUEST_GET_STDOUT;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'e':
      operation = CLIENT_REQUEST_GET_STDERR;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'h':
      printf("%s", usage_info);
      return 0;
    case '?':
      fprintf(stderr, "%s", usage_info);
      goto error;
    }
  }

  // --------
  // | TODO |
  // --------

  //Creation des chaines de caractères pour l'ouverture des FIFO
  char *pipe_request_file = NULL;
  char *pipe_reply_file = NULL;
  if(get_pipes_file(pipes_directory, &pipe_request_file, &pipe_reply_file)) {
    printf("Erreur construction chaine de caractere des fichiers pipes\n");
    exit(EXIT_FAILURE);
  }
  
  //Ouverture du FIFO request
  int fd_request = open(pipe_request_file, O_WRONLY, 0750);
  if(fd_request == -1) {
    perror("PBM OUVERTURE PIPE !\n");
    if(errno == ENOENT) {
      printf("Fichier : %s n'existe pas \n", pipe_request_file);
    }
    free(pipe_request_file);
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  }

  free(pipe_request_file);

  //Dans tous les cas on écrit le opcode
  if(write_opcode(fd_request, operation) == 1) {
    printf("PBM ECRITURE OPCODE !\n");
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  } 
  //Ensuit suivant les cas ...
  switch(operation) {
    case CLIENT_REQUEST_REMOVE_TASK :
    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
    case CLIENT_REQUEST_GET_STDOUT :
    case CLIENT_REQUEST_GET_STDERR :
      //On écrit en plus le taskid
      if(write_taskid(fd_request, taskid) == 1) {
        printf("PBM ECRITURE TASKID !\n");
        free(pipe_reply_file);
        exit(EXIT_FAILURE);
      } 
      break;
    case CLIENT_REQUEST_CREATE_TASK :
      //On écrit tous ce qu'il faut la requête create
      if(write_create(fd_request, minutes_str, hours_str, daysofweek_str, argc - optind, argv + optind)) {
        printf("PBM ECRITURE CREATE !\n");
        free(pipe_reply_file);
        exit(EXIT_FAILURE);
      }
      break;
  }

  //Fermeture du FIFO request
  if(close(fd_request) == -1) {
    perror("PBM CLOSE");
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  }

  //Ouverture du FIFO reply
  // int fd_reply = open(pipe_reply_file, O_RDONLY);
  // if(fd_reply == -1) {
  //   perror("PBM OPEN REPLY");
  //   if(errno == ENOENT) {
  //     printf("Fichier : %s n'existe pas \n", pipe_reply_file);
  //   }
  //   free(pipe_reply_file);
  //   exit(EXIT_FAILURE);
  // }
  free(pipe_reply_file);

  //Toute la partie où on read le reste

  return EXIT_SUCCESS;

 error:
  if (errno != 0) perror("main");
  free(pipes_directory);
  pipes_directory = NULL;
  return EXIT_FAILURE;
}

