#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lecture_caracteres.h"

int CarCour;
unsigned int LigneCour, ColonneCour;
FILE *File;

void demarrer_car(char *nom_fichier) {
    if (strlen(nom_fichier) == 0) {
        File = stdin;
    } else {
        File = fopen(nom_fichier, "r");
        if (File == NULL) {
            perror("Erreur ouverture fichier");
             exit(1);
        }
    }
    LigneCour = 1;
    ColonneCour = 0;
    avancer_car();
}

void avancer_car() {
    if (File != NULL && !feof(File)) {
        CarCour = fgetc(File);
        if (CarCour != '\n') {
            ColonneCour = ColonneCour + 1;
        } else {
            ColonneCour = 1;
            LigneCour = LigneCour + 1;
        }
    } else {
        CarCour = EOF;
    }
}

char caractere_courant() {
    return CarCour;
}

int fin_de_sequence_car() {
    return CarCour == EOF;
}

unsigned int numero_ligne() {
    return LigneCour;
}

unsigned int numero_colonne() {
    return ColonneCour;
}

void arreter_car() {
    if (File != NULL && File != stdin) {
        fclose(File);
        File = NULL;
    }
}