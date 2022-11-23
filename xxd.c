// Auteurs : Simon NICOD & Julien MOURCELY 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int saisir_chaine(char * lpBuffer, size_t nBufSize);

// Commande xxd : Affiche le contenu d'un fichier en hexadécimal et en texte
// Chaque ligne est composée de :
//    - L'adresse depuis le début du fichier, en base 16 sur 8 chiffres, suivie d'un double-point.
//    - Une espace, puis les valeurs de 16 octets en base 16 sur deux chiffres, avec une espace pour séparer tous les groupes de deux octets.
//    - Deux espaces, puis les caractères correspondant aux octets. Si le caractère n'est pas imprimable, il est remplacé par un point (.).
//    - Si la taille du fichier n'est pas un multiple de 16, la dernière ligne est tronquée, comme dans l'exemple.
// Le programme doit prendre le nom du fichier à afficher sur sa ligne de commande. Si aucun nom de fichier n'est donné, le programme doit lire les données sur son entrée standard.

// Usage : ./xxd fichier
// Or    : ./xxd
//          -> Enter the string you want to read

int main(int argc, char *argv[]){
    // Initialisations
    int fd;
    int nbOctetsLus;
    unsigned char buffer[1000];
    char *nomFichier;
    int i;
    int entry = 0;

    // Si aucun argument n'est donné
    if (argc == 1){
        // On récupère l'entrée
        char texte[1000];
        saisir_chaine(texte, sizeof(texte));

        // On lit default.txt
        nomFichier = texte;

        entry=1;

    } else {
        // On lit le fichier donner en argument
        nomFichier=argv[1];
    }

    fd = open(nomFichier, O_RDONLY);

    // Si le nom de fichier donné ne correspond à aucun fichier
    if (fd < 0){
        char texte2[1000];
        for (int i=1;i<argc;i++){
            strcat(texte2,argv[i]);
            strcat(texte2," ");
        } 
        // On écrit le paramètre dans le fichier temporaire default.txt
        FILE* fichier = fopen("default.txt","w");
        fputs(texte2,fichier);
        fclose(fichier);

        entry=1;

        fd = open("default.txt", O_RDONLY);
    }

    // On lit le fichier par blocs de 16 octets
    while ((nbOctetsLus = read(fd, buffer, 16)) > 0){
        // On affiche l'adresse du bloc
        printf("%08x: ", (int)(lseek(fd, 0, SEEK_CUR) - nbOctetsLus));

        // On affiche les valeurs en hexadecimal
        for (i = 0; i < 16; i++){
            // Par groupe de deux caractères
            if (i%2 == 0){ 
                if (i+1 < nbOctetsLus){
                    if ((int)buffer[i+1]!=0)
                        printf("%02x%02x ", (int)buffer[i], (int)buffer[i+1]);
                } else {
                    // Si la dernière ligne contient un nombre impair de caractères, on affiche qu'un seul caractère pour le dernier
                    if (i < nbOctetsLus)
                        printf("%02x   ",(int)buffer[i]);
                    // Si on arrive à la fin du fichier, on tronque la ligne par le nombre d'espaces correspondants
                    else
                        printf("     ");
                }
            } 
        }
        
        // On affiche le deuxième espace à la fin des 16 octets
        printf(" ");

        // On affiche les valeurs en texte
        for (i = 0; i < nbOctetsLus; i++){
            if (isprint(buffer[i])){
                printf("%c", buffer[i]);
            } else {
                // On remplace les caractères "illisibles" par des points
                printf(".");
            }
        }

        // On saute une ligne
        printf("\n");
    }

    // On ferme le fichier
    close(fd);

    if (entry) remove("default.txt");

    return 0;
}

// Permet de récupérer l'entrée de l'utilisateur
int saisir_chaine(char * buffer, size_t size){
    size_t nbCar = 0;    
    int c;
 
    c = getchar();
    // On remplit buffer avec les caractères entrés
    while (nbCar < size - 1 && c != EOF && c != '\n'){
        buffer[nbCar] = (char)c;
        nbCar++;
        c = getchar();
    }

    // On indique la fin de la chaîne de caractères
    buffer[nbCar] = '\0';
 
    return 1;
}