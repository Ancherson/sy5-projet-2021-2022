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

int list(char *buf, task *t, uint32_t nbtasks);
int create(int fd, char *buf, task **pt, int *len, int *nb_task, uint64_t *max_id);
int remove_(int fd, char *buf, task *t, int len, int *nb_task);
int times_exitcodes(int fd, char *buf, task *t, int len, uint64_t max_id);
int stdout_stderr(int fd, char *buf, task *t, int nb_task, uint16_t opcode, uint64_t max_id);
void execute_task(task t);

#endif // EXECUTE_REQUEST_H