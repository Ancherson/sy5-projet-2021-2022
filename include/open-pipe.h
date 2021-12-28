#ifndef OPEN_PIPE_H
#define OPEN_PIPE_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

int get_pipes_file(char *pipes_directory, char **pipe_request, char **pipe_reply);
void create_tmp();
void create_pipes(char *pipes_request, char *pipe_reply);

#endif