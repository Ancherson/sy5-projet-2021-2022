#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-data.h"
#include "read-data.h"
#include "client-request.h"
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

/* Ensemble de fonctions qui font ce qui est demandé par les requêtes et envoient les messages de réponse
 * Elles utilisent l'argument fd pour lire le reste de la requête (après l'opcode) si nécessaire
 * Et utilisent fd_reply pour envoyer le message de réponse */

void list(int fd_reply, task *t, uint32_t nb_task);
void create(int fd, int fd_reply, task **pt, int *len, int *nb_task, uint64_t *max_id);
void remove_(int fd, int fd_reply, task *t, int len, int *nb_task);
void times_exitcodes(int fd, int fd_reply, task *t, int len, uint64_t max_id);
void stdout_stderr(int fd, int fd_reply, task *t, int nb_tasks, uint16_t opcode, uint64_t max_id);
void execute_task(task t);
void terminate(int fd, int* run);

#endif // EXECUTE_REQUEST_H