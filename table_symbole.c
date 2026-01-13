#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table_symbole.h"
#include "analyse_lexicale.h"

TableSymboles table_globale;

void init_table() {
    table_globale.nb_symboles = 0;
    printf("Table des symboles initialisée.\n");
}

Symbole* rechercher_symbole(const char* nom) {
    for (int i = 0; i < table_globale.nb_symboles; i++) {
        if (strcmp(table_globale.table[i].nom, nom) == 0) {
            return &table_globale.table[i];
        }
    }
    return NULL;
}

int ajouter_symbole(const char* nom, int valeur) {
    Symbole* symbole_existant = rechercher_symbole(nom);

    if (symbole_existant != NULL) {
        symbole_existant->valeur = valeur;
        printf("DEBUG: Symbole '%s' mis à jour à %d\n", nom, valeur);
        return 1;
    } else {
        if (table_globale.nb_symboles < MAX_SYMBOLS) {
            strcpy(table_globale.table[table_globale.nb_symboles].nom, nom);
            table_globale.table[table_globale.nb_symboles].valeur = valeur;
            table_globale.nb_symboles++;
            printf("DEBUG: Symbole '%s' ajouté avec valeur %d\n", nom, valeur);
            return 1;
        } else {
            printf("ERREUR : Table des symboles pleine, impossible d'ajouter '%s'.\n", nom);
            return 0;
        }
    }
}

int get_valeur_symbole(const char* nom) {
    Symbole* symbole = rechercher_symbole(nom);
    if (symbole != NULL) {
        return symbole->valeur;
    } else {
        printf("ERREUR : Variable non définie '%s' utilisée dans l'expression.\n", nom);
        exit(1);
    }
}

void afficher_table() {
    printf("\n--- Table des Symboles ---\n");
    if (table_globale.nb_symboles == 0) {
        printf(" (vide)\n");
    } else {
        for (int i = 0; i < table_globale.nb_symboles; i++) {
            printf("  %s = %d\n", table_globale.table[i].nom, table_globale.table[i].valeur);
        }
    }
    printf("--------------------------\n");
}