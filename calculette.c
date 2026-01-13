#include <stdlib.h>
#include <stdio.h>

#include "type_ast.h"
#include "analyse_syntaxique.h"
#include "ast_parcours.h"
#include "table_symbole.h"

int main (int argc, char *argv[]) {
    int syntaxeCorrecte;
    Ast programme_ast = NULL;
    char* nomFichier = "";

    if (argc == 2) {
        nomFichier = argv[1];
        printf("Analyse du fichier : %s\n", nomFichier);
    } else if (argc > 2) {
        printf("Usage: %s [<nom_fichier>]\n", argv[0]);
        exit(1);
    } else {
        printf("Analyse depuis l'entrée standard (stdin)...\n");
    }

    syntaxeCorrecte = analyser(nomFichier, &programme_ast);

    if (syntaxeCorrecte && programme_ast != NULL) {
        printf("\nProgramme analysé avec succès. AST construit.\n");

        printf("\n--- Lancement Interpréteur ---\n");
        interpreter(programme_ast);
        printf("--- Fin Interpréteur ---\n");

        printf("\nÉtat final de la table des symboles :\n");
        afficher_table();

    } else if (syntaxeCorrecte && programme_ast == NULL) {
        printf("\nAnalyse terminée: Fichier vide ou sans instructions.\n");
    }
    else {
        printf("\nL'analyse a échoué. Pas d'interprétation possible.\n");
        return 1;
    }

    return 0;
}