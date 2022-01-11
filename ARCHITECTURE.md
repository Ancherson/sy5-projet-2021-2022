# Architecture du Projet

## Les Tubes
Le projet afin de faire la communication entre **saturnd** et **cassini** utilise des tubes. **saturnd** va créer des tubes dans le repertoire **/tmp/user/saturnd/tubes** dans le cas où le repertoire pour stocker les tubes n'est pas précisé et elles seront supprimées lors de l'extinction de saturn avec la commande -q de **cassini**.

### l'écriture / lecture

**cassini** lui pour envoyer une requête utilise un seul appel système write afin d'écrire un bloc en une fois sur la tube. Pour cela à chaque création de requête on construit le message (dans un buffer) avec des fonctions dans `write-data.c` puis on le write dans la tube de requête.

**saturnd** lui fait à peu près là même chose avec les fonctions dans `execute-request.c` qui utilise les fonctions de `write-data.c` pour construire la réponse puis l'écrit d'un seul bloc dans la tube de réponse.

En revanche les lectures dans les tubes utilisent plusiseurs read puisqu'on ne connait pas la taille du messsage à recevoir.

### l'ouverture

l'ouverture des tubes dans **cassini** est simple car les tubes ont déjà été ouverts dans **saturnd**. On ouvre d'abord le tube des requêtes en écriture puis celle des réponses en lecture.

Dans **saturnd** en revanche l'ouverture des tubes est plus complexe. On ouvre dans un premier temps le tube de requête en  en lecture non-bloquante puis on crée un _écrivain fantôme_ en ouvrant la tube de requête en écriture. Cela est nécessaire car si il n'y pas d'écrivain sur le tube l'appel système à select n'est plus bloquant.
Ensuite on ouvre le tube de réponse avec un _lecteur fantôme_ en l'ouvrant en lecture non-bloquante afin de pouvoir ouvrir le tube en écriture bloquante.


## Les Tâches

### Le stockage sur le disque

Chaque tâche est stocké dans un repertoire du nom de son `id`, dans ce repertoire on a 4 fichiers, les données où sont stocké la commande à exécuter ainsi que son timing, c'est le fichier `data`. Il y a aussi la sortie standard `stdout` et la sortie d'erreur `stderr` qui sont créés que lors de la 1ère éxecution de la tâche. Enfin le fichier "historique" `times_exitcodes` qui stocke toutes les éxecutions précedentes de la tâche (le temps final d'éxecution + l'exit code).

Lorsque le fichier ``data`` n'existe plus, la tâche a été désactivé, elle ne s'éxecutera plus cependant on peut toujours obtenir son historique ainsi que ses sorties.

### Le stockage dans saturnd

Les tâches sont représentées par une structure appelée `task`. On y stocke l'id de la tâche (**taskid**), sa ligne de commande à éxecuter (**cmd**) et les temps auquels elle doit être éxecutée (**time**).
Elles sont ensuite stocké sous cette forme dans un tableau durant l'éxecution de **saturnd** si elles sont à éxecuter (non désactivée).

## L'Execution

### Execution de tâches

Chaque tâche, lorsqu'elle doit être éxecuté s'éxecute dans un processus fils de **saturnd** propre à cette tâche  afin de ne pas perturber le fonctionnement de **saturnd**. **Saturnd** tue tout ses fils qui ont fini d'être éxecuté à chaque tour de la boucle (1 minute).

### initialisation

Le tableau des tâches est initialisé au lancement de **saturnd**. On parcours le repertoire des tâches et pour chaque tâche où le fichier `data` est présent on l'inclut dans les tâches éxecutables donc dans le tableau.

### Gestion du temps

A chaque tour de boucle on va faire un appel système à time afin de récuperer le temps actuel puis on va calculer combien de secondes il nous manque pour atteindre la prochaine minute. Cette durée est le temps donné en argument à select. Select va alors attendre cette durée si il n'y a pas de nouvelle requête. Dans le cas d'une nouvelle requête afin de ne pas rater une minute, on stocke la dernière minute exécutée (`last_minute`). On vérifie ensuite à chaque début de boucle si la minutes actuelle est différente de `last_minute` ou alors que la durée passée en argument de select soit nulle.

## Découpage modulaire

-`string2` -> défintion de la structure **string** stockant une chaîne de caractères et sa longueur ainsi que les fonctions permettants sa manipulation

-`commandline` -> définition de la structure **commandline** stockant la ligne de commande et le nombre d'arguments ainsi que les fonctions permettants la manipulation de la structure.

-`open-pipe` -> regoupe les fonctions permettant la création de tubes et les dossiers dans lesquels ils sont stocké.

-`write-data` -> regroupe toutes les fonctions permettant l'écriture de données (dans un buffer, sauf **write_pipebuf** qui écrit dans un tube)

-`read-data` -> regroupe toutes les fonctions permettant la lecture de données dans les tubes.

-`task` -> définition de la structure **task** stockant l'id de la task, la ligne de command et l'heure d'éxecution de la tâche ainsi que toutes les fonctions permettant la gestion de la structure.

-`execute-request` ->  regroupe les fonctions permettant l'éxecution des requêtes envoyées par **cassini** à **satrund** et d'envoyer la réponse à **cassini**.

-`run` -> regroupe les fonctions permettant la gestion des taches dans **saturnd**, allant de de l'initialisation à l'éxecution des tâches.

-`cassini` -> main du client permettant l'envoie de requête à **saturnd**.

-`saturnd` -> main du démon qui boucle toutes les minutes pour éxecuter les tâches et qui attend les requêtes de **cassini**.