//auteurs: Simon Nicod et Julien Mourcely
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{

    //récupération des commandes 
    char **commande1;
    char **commande2;
    int cpt1 = 1;
    int cpt2 = 0;

    while(cpt1<argc-1 && strcmp(argv[cpt1], "--")!=0) { // argc-1 pour éviter une seg fault
        cpt1++;
    }

    if (strcmp(argv[cpt1], "--")) { // traitement d'erreur de syntaxe
        perror("erreur de syntaxe");
        return EXIT_FAILURE;
    }

    commande1 = calloc((cpt1), sizeof(char*)); // on alloue de la mémoire pour nos doubles pointeurs et on vérifie qu'il n'y a pas d'erreur
    if (commande1 == NULL) {
        perror("Erreur allocation mémoire");
        return EXIT_FAILURE;
    }
    cpt1++;
    while (cpt1 < argc) {
        cpt1++;
        cpt2++;
    }
    commande2 = calloc(cpt2+1, sizeof(char*));
    if (commande2 == NULL) {
        perror("Erreur allocation mémoire");
        return EXIT_FAILURE;
    }
    cpt1 = 1;
    cpt2 = 0;

    while(cpt1<argc && strcmp(argv[cpt1], "--")!=0) {
        commande1[cpt1-1] = argv[cpt1];
        cpt1++;
    }
    commande1[cpt1-1] = NULL;
    cpt1++;
    while (cpt1 < argc) {
        commande2[cpt2] = argv[cpt1];
        cpt1++;
        cpt2++;
    }
    commande2[cpt2] = NULL;

    //début des processus
    pid_t pid1, pid2;
    int status, pidwait;
    int fa, fb;
    char fichierA[] = "/tmp/fichierA.XXXXXX";
    char fichierB[] = "/tmp/fichierB.XXXXXX";

    //création des fichiers temporaires
    fa = mkstemp(fichierA);
    if (fa < 0) {
        perror("open");
        exit(1);
    }

    fb = mkstemp(fichierB);
    if (fb < 0) {
        perror("open");
        exit(1);
    }


//processus 1
    pid1 = fork();
    if (pid1 == 0) {
        dup2(fa, 1);
        execvp(commande1[0], commande1);
    } else {
        pidwait = wait(&status);
    } 

//processus 2
    pid2 = fork();
    if (pid2 == 0) {
        dup2(fb, 1);
        execvp(commande2[0], commande2);
    } else {
        pidwait = wait(&status);
    } 

    //comparaison des fichiers avec diff -u dans un 3e processus
    pid_t pid3;
    int status3;
    int st;
    pid3 = fork();
    if (pid3 == 0)
    {
        execlp("diff", "diff", "-u", fichierA, fichierB, NULL);
        exit(0);
    }
    else
    {
        do {
            pid3 = wait(&status3);
            if (pid3 == -1) {
                if (WIFEXITED(status3)) {
                    st = WEXITSTATUS(status3);
                }
            }
        }while (pid3 != -1);
    }

    //fermeture des fichiers 
    close(fa);
    close(fb);

    //suppression des fichiers temporaires
    if ((remove(fichierA) != 0)) {
        perror("removeA");
    }
    if ((remove(fichierB) != 0)) {
        perror("removeB");
    }
    
    //libération de la mémoire allouée
    free(commande1);
    free(commande2);

    //permet de return la valeur souhaitée en fonction du résultat du diff
    switch (st) {
        case 0:
            return st;
        case 1:
            return st;
        default:
            return 2;
    }

}

