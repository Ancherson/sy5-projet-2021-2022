#include "saturnd.h"


/*Lancement de saturnd en tant que demon*/
void launch() {
    int r = fork();
    if(r == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if(r != 0) exit(0);
}

int main(int argc, char **argv){
    //lancement en demon
    launch();
    char *pipes_directory = NULL;
    //recuperation de path si un repertoire a ete mis en argument
    if(argc < 2) create_tmp();
    else pipes_directory = argv[1];

    char *pipe_request_file = NULL;
    char *pipe_reply_file = NULL;

    //creation du chemin pour la creation des pipes
    if(get_pipes_file(pipes_directory, &pipe_request_file, &pipe_reply_file)) {
        printf("Erreur construction chaine de caractere des fichiers pipes\n");
        exit(EXIT_FAILURE);
    }

    int nb_tasks;
    int len;
    uint64_t max_id;

    //creation du tableau des taches qui etaient sauvegarde
    task *t = init_task(&len, &nb_tasks, &max_id);
    launch_executable_tasks(t, nb_tasks);

    //creation des tubes de requete et de reponse
    create_pipes(pipe_request_file, pipe_reply_file);

    //ouverture du tube de requete en lecteur non bloquant
    int fd_request = open(pipe_request_file, O_RDONLY|O_NONBLOCK);
    if(fd_request == -1) {
        perror("open request");
        return EXIT_FAILURE;
    }
    //ouverture du tube de requete en tant qu'ecrivain fantome
    int fd_gohst = open(pipe_request_file, O_WRONLY);
    if(fd_gohst == -1) {
        perror("open request gohst");
        return EXIT_FAILURE;
    }
    //ouverture du tube de reponse en tant que lecteur non bloquant fantome
    int fd_reply_gohst = open(pipe_reply_file, O_RDONLY | O_NONBLOCK);
    if(fd_reply_gohst == -1) {
        perror("Error fd reply gohst");
        return EXIT_FAILURE;
    }
    //ouverture du tube de reponse en tant qu'ecrivain
    int fd_reply = open(pipe_reply_file, O_WRONLY);
    if(fd_reply == -1) {
        dprintf(2, "Error fd reply\n");
        return EXIT_FAILURE;
    }

    //fermeture du lecteur fantome
    if(close(fd_reply_gohst) == -1) {
        perror("close reply_gohst");
        return EXIT_FAILURE;
    }




    int nfds = fd_request+1;
    fd_set read_set;

    //structure pour definir le temps d'attente de select
    struct timeval timeV;
    time_t current_time;

    //structure pour recuperer le temps actuel de facon simple
    struct tm ts;
    time(&current_time);
    ts = *localtime(&current_time);
    int last_minute = ts.tm_min;
    int running = 1;

    //lancement de la boucle qui va tourner toutes les minutes ou sinon a chaque requete
    while(running){
        time(&current_time);
        ts = *localtime(&current_time);

        //test afin de savoir si on doit executer des taches
        if((timeV.tv_sec == 0 && timeV.tv_usec == 0) || last_minute != ts.tm_min){
            launch_executable_tasks(t, nb_tasks);
            time(&current_time);
            ts = *localtime(&current_time);
            last_minute = ts.tm_min;
        } 
        //definition du temps d'attente de select en fonction du temps actuel
        timeV.tv_sec = 60 - ts.tm_sec;
        timeV.tv_usec = 0;

        //tue tout les fils qui ont fini l'execution de leur tache
        clean_defunct();

        FD_ZERO(&read_set);
        FD_SET(fd_request,&read_set);


        //attente de la fin du timeout ou d'une requete
        int cond = select(nfds,&read_set,NULL,NULL,&timeV);
        if (cond == 0){
            //printf("J'ai rien lu \n");
        }
        //probleme lecture de la pipe
        if (cond == -1) {
            perror("PB select saturnd");
            return 1;
        }
        //test si c'est bien une requete
        if(FD_ISSET(fd_request, &read_set)){
            uint16_t op_code= read_uint16(fd_request);
            if(fd_reply == -1) {
                perror("open reply");
                return EXIT_FAILURE;
            }
            //gestion des requetes en fonction de l'opcode
            switch (op_code){
                case CLIENT_REQUEST_LIST_TASKS :
                    list(fd_reply, t, nb_tasks);
                    break;
                
                case CLIENT_REQUEST_CREATE_TASK :            
                    create(fd_request, fd_reply, &t, &len, &nb_tasks, &max_id);
                    break;

                case CLIENT_REQUEST_REMOVE_TASK :
                    remove_(fd_request, fd_reply, t, len, &nb_tasks);
                    break;
        
                case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    times_exitcodes(fd_request, fd_reply, t, nb_tasks, max_id);
                    break;

                case CLIENT_REQUEST_TERMINATE :
                    terminate(fd_reply,&running);
                    break;
                
                case CLIENT_REQUEST_GET_STDOUT :
                    stdout_stderr(fd_request, fd_reply, t, nb_tasks, CLIENT_REQUEST_GET_STDOUT, max_id);
                    break;
    
                case CLIENT_REQUEST_GET_STDERR :
                    stdout_stderr(fd_request, fd_reply, t, nb_tasks, CLIENT_REQUEST_GET_STDERR, max_id);
                    break;
                
                default:
                    return 1;
                    break;
            }

        }

    }
    //free le tableau de tache lors de l'arret de saturnd
    free_task_array(t, &nb_tasks);
    if(close(fd_reply) == -1) {
        perror("close reply");
        return EXIT_FAILURE;
    }

    //fermeture des descripteurs

    if(close(fd_request) == -1) {
        perror("close request");
        return EXIT_FAILURE;
    }
    if(close(fd_gohst) == -1) {
        perror("close gohst");
        return EXIT_FAILURE;
    }

    //suppression des tubes nommés
    if(unlink(pipe_reply_file)== -1){
        perror("delete pipe_reply");
    }
    if(unlink(pipe_request_file)== -1){
        perror("delete pipe_request");
    }

    //free les chemins des tubes nommés
    free(pipe_request_file);
    free(pipe_reply_file);
    return EXIT_SUCCESS;
}
