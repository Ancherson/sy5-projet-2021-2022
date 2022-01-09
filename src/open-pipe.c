#include "open-pipe.h"

/* Mets dans pipe_request et dans pipe_reply les chaines de caractères de où ouvrir les pipes */
int get_pipes_file(char *pipes_directory, char **pipe_request, char **pipe_reply) {
  //Si le pipes_directory n'est pas précisé, on choisi /tmp/USER/saturnd/pipes
  int b = (pipes_directory == NULL);
  if(b) {
    char *user = getenv("USER");
    char *dir_pre  = "/tmp/";
    char *dir_next = "/saturnd/pipes";
    pipes_directory = malloc(strlen(dir_pre) + strlen(user) + strlen(dir_next) + 1);
    if(pipes_directory == NULL) {
      perror("PBM malloc pipes_directory");
      exit(EXIT_FAILURE);
    }
    strcpy(pipes_directory, dir_pre);
    strcat(pipes_directory, user);
    strcat(pipes_directory, dir_next);
    pipes_directory[strlen(dir_pre) + strlen(user) + strlen(dir_next)] = '\0';
  }

  //Creation des chaines de caractères pour l'ouverture des FIFO
  char *str_pipe_request = "saturnd-request-pipe";
  char *str_pipe_reply = "saturnd-reply-pipe";

  char *pipe_request_file = malloc(strlen(pipes_directory) + strlen(str_pipe_request) + 2);
  if(pipe_request_file == NULL) {
    perror("PBM malloc pipe_request_file");
    exit(EXIT_FAILURE);
  }

  char *pipe_reply_file = malloc(strlen(pipes_directory) + strlen(str_pipe_reply) + 2);
  if(pipe_request_file == NULL) {
    perror("PBM malloc pipe_reply_file");
    exit(EXIT_FAILURE);
  }

  strcpy(pipe_request_file,pipes_directory);
  strcat(pipe_request_file, "/");
  strcat(pipe_request_file,str_pipe_request);
  pipe_request_file[strlen(pipes_directory) + strlen(str_pipe_request) + 1] = '\0';

  strcpy(pipe_reply_file,pipes_directory);
  strcat(pipe_reply_file, "/");
  strcat(pipe_reply_file,str_pipe_reply);
  pipe_reply_file[strlen(pipes_directory) + strlen(str_pipe_reply) + 1] = '\0';

  if(b) free(pipes_directory);

  *pipe_request = pipe_request_file;
  *pipe_reply = pipe_reply_file;

  return 0;
}

/* Créer le dossier /tmp/USER/saturnd/pipes, qui servira de lieu d'ouverture des pipes */
void create_tmp() {
    char path[4096];
    memset(path, 0, 4096);
    snprintf(path, 4096, "/tmp/%s", getenv("USER"));
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
    strcat(path, "/saturnd");
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
    strcat(path, "/pipes");
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
}

/* Créer les pipes */
void create_pipes(char *pipes_request, char *pipe_reply) {
    if(mkfifo(pipes_request, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkfifo %s\n", pipes_request);
        exit(EXIT_FAILURE);
    }
    if(mkfifo(pipe_reply, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkfifo %s\n", pipe_reply);
        exit(EXIT_FAILURE);
    }
}