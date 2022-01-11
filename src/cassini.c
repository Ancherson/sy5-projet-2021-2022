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

  //Creation des chaines de caractères pour l'ouverture des FIFO
  char *pipe_request_file = NULL;
  char *pipe_reply_file = NULL;
  get_pipes_file(pipes_directory, &pipe_request_file, &pipe_reply_file);
  if(pipes_directory != NULL) free(pipes_directory);
  
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

  //buffer qui stocke le message à envoyer
  char buf[BUFFER_SIZE];

  //Dans tous les cas on écrit le opcode
  int n = write_opcode(buf, operation);

  //Ensuit suivant les cas ...
  switch(operation) {
    case CLIENT_REQUEST_REMOVE_TASK :
    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
    case CLIENT_REQUEST_GET_STDOUT :
    case CLIENT_REQUEST_GET_STDERR :
      //On écrit en plus le taskid
      n += write_taskid(buf+n, taskid);
      break;
    case CLIENT_REQUEST_CREATE_TASK :
      //On écrit tous ce qu'il faut la requête create
      n += write_create(buf+n, minutes_str, hours_str, daysofweek_str, argc - optind, argv + optind);
      break;
  }

  //On écrit la requête dans le tube
  if(write(fd_request, buf, n) < n) {
    printf("PBM ECRITURE REQUETE!\n");
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  }

  //Fermeture du FIFO request
  if(close(fd_request) == -1) {
    perror("PBM CLOSE");
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  }

  //Ouverture du FIFO reply
  int fd_reply = open(pipe_reply_file, O_RDONLY);
  if(fd_reply == -1) {
    perror("PBM OPEN REPLY");
    if(errno == ENOENT) {
      printf("Fichier : %s n'existe pas \n", pipe_reply_file);
    }
    free(pipe_reply_file);
    exit(EXIT_FAILURE);
  }
  free(pipe_reply_file);

  //On read la reply et on stocke l'exit-code à renvoyer
  int exit_code = EXIT_SUCCESS;
  switch(operation){
    case CLIENT_REQUEST_LIST_TASKS:
      exit_code = read_list(fd_reply);
      break;
    case CLIENT_REQUEST_CREATE_TASK:
      exit_code = read_create(fd_reply);
      break;
    case CLIENT_REQUEST_REMOVE_TASK:
      exit_code = read_remove(fd_reply);
      break;
    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
      exit_code = read_times_exitcode(fd_reply);
      break;
    case CLIENT_REQUEST_TERMINATE:
      exit_code = read_terminate(fd_reply);
      break;     
    case CLIENT_REQUEST_GET_STDOUT:
    case CLIENT_REQUEST_GET_STDERR:
      exit_code = read_stdout_stderr(fd_reply);
      break;
  }

  //On renvoie l'exit-code
  return exit_code;

 error:
  if (errno != 0) perror("main");
  free(pipes_directory);
  pipes_directory = NULL;
  return EXIT_FAILURE;
}

