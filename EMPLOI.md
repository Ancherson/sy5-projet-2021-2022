# Compilation
Se placer à la racine du projet et exécuter :
```
make
```


# Utilisation
## Saturnd
Lancer saturnd : 
```
./saturnd
```
On peut aussi lancer saturnd en spécifiant le dossier dans lequel sont les tubes de communication entre saturnd et cassini avec :
```
./saturnd PIPES_DIR
```
## Cassini
Pour afficher les options possibles de cassini :
```
./cassini -h
```
Pour lancer la requête `LIST` :
```
./cassini -l
```
ou
```
./cassini
```
Pour lancer la requête `TERMINATE` :
```
./cassini -q
```
Pour lancer la requête `CREATE` :
```
./cassini -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]
```
Pour lancer la requête `REMOVE` :
```
./cassini -r TASKID
```
Pour lancer la requête `TIMES_EXITCODES` :
```
./cassini -x TASKID
```
Pour lancer la requête `STDOUT` :
```
./cassini -o TASKID
```
Pour lancer la requête `STDERR` :
```
./cassini -e TASKID
```
On peut aussi spécifier le dossier dans lequel sont les tubes de communication entre saturnd et cassini avec :
```
./cassini -p PIPES_DIR ...
```