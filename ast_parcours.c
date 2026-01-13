#include <stdlib.h>
#include <stdio.h>
#include "type_ast.h"
#include "ast_parcours.h"
#include "table_symbole.h"
#include "analyse_lexicale.h"

void aff_operateur(TypeOperateur op){
    switch (op) {
        case N_PLUS:  printf(" + "); break;
        case N_MOINS: printf(" - "); break;
        case N_MUL:   printf(" * "); break;
        case N_DIV:   printf(" / "); break;
        case N_EQ:    printf(" == "); break;
        case N_NEQ:   printf(" != "); break;
        case N_LT:    printf(" < "); break;
        case N_LE:    printf(" <= "); break;
        case N_GT:    printf(" > "); break;
        case N_GE:    printf(" >= "); break;
        default: printf(" ??? ");
    }
}

void afficher_ast(Ast A) {
    if (A == NULL) {
         printf("<vide>");
        return;
    }

    switch (A->nature) {
        case N_SEPINST:
            printf("(");
            afficher_ast(A->gauche);
            printf(" ; ");
            afficher_ast(A->droite);
            printf(")");
            break;

        case N_LIRE:
            printf("lire(");
            if (A->gauche && A->gauche->nature == N_IDF) {
                 printf("%s", A->gauche->ident);
            } else {
                 printf("ERREUR_AFF_LIRE");
            }
            printf(")");
            break;

        case N_ECRIRE:
            printf("ecrire(");
            afficher_ast(A->gauche);
            printf(")");
            break;

        case N_AFF:
            if (A->gauche && A->gauche->nature == N_IDF) {
                 printf("%s", A->gauche->ident);
            } else {
                 printf("ERREUR_AFF_AFF_G");
            }
            printf(" = ");
            afficher_ast(A->droite);
            break;

        case N_IF:
            printf("if (");
            afficher_ast(A->gauche);
            printf(") then {");
            afficher_ast(A->milieu);
            printf("} else {");
            afficher_ast(A->droite);
            printf("} fi");
            break;

        case N_WHILE:
            printf("tanque (");
            afficher_ast(A->gauche);
            printf(") faire {");
            afficher_ast(A->droite);
            printf("} fait");
            break;

        case OPERATION:
            printf("(");
            afficher_ast(A->gauche);
            aff_operateur(A->operateur);
            afficher_ast(A->droite);
            printf(")");
            break;

        case N_IDF:
            printf("%s", A->ident);
            break;
        case VALEUR:
            printf("%d", A->valeur);
            break;
        default:
            printf("Type noeud inconnu affichage: %d\n", A->nature);
    }
}

int evaluation(Ast expr) {
    if (expr == NULL) {
        printf("ERREUR EVALUATION: AST NULL.\n");
        exit(1);
    }

    int val_gauche, val_droite;

    switch(expr->nature) {
        case OPERATION:
            val_gauche = evaluation(expr->gauche);
            val_droite = evaluation(expr->droite);
            switch (expr->operateur) {
                case N_PLUS:  return val_gauche + val_droite;
                case N_MOINS: return val_gauche - val_droite;
                case N_MUL:   return val_gauche * val_droite;
                case N_DIV:
                    if (val_droite == 0) {
                        printf("ERREUR EVALUATION : division par 0 !\n");
                        exit(1);
                    }
                    return val_gauche / val_droite;
                case N_EQ:    return val_gauche == val_droite;
                case N_NEQ:   return val_gauche != val_droite;
                case N_LT:    return val_gauche < val_droite;
                case N_LE:    return val_gauche <= val_droite;
                case N_GT:    return val_gauche > val_droite;
                case N_GE:    return val_gauche >= val_droite;
                default:
                    printf("ERREUR EVALUATION: Opérateur inconnu dans OPERATION: %d\n", expr->operateur);
                    exit(2);
            }
            break;

        case VALEUR:
            return expr->valeur;
        case N_IDF:
            return get_valeur_symbole(expr->ident);

        default:
             printf("ERREUR EVALUATION: Type noeud AST non évaluable comme expression: %d\n", expr->nature);
             exit(2);
    }
     // Cette ligne ne devrait jamais être atteinte à cause des exit() et return dans les cas.
     // Mais pour éviter un warning "control reaches end of non-void function" :
     return 0;
}

void interpreter_aff(Ast A) {
    if (A == NULL || A->nature != N_AFF || A->gauche == NULL || A->gauche->nature != N_IDF || A->droite == NULL) {
        printf("ERREUR INTERPRETATION: Noeud N_AFF invalide.\n");
        exit(1);
    }
    char *idf = A->gauche->ident;
    int valeur = evaluation(A->droite);
    if (!ajouter_symbole(idf, valeur)) {
         exit(1);
    }
     printf("INTERP: %s <- %d\n", idf, valeur);
}

void interpreter_lire(Ast A) {
     if (A == NULL || A->nature != N_LIRE || A->gauche == NULL || A->gauche->nature != N_IDF) {
        printf("ERREUR INTERPRETATION: Noeud N_LIRE invalide.\n");
        exit(1);
    }
    char *idf = A->gauche->ident;
    int valeur_lue;
    printf("Entrez une valeur entière pour %s : ", idf);
    fflush(stdout);
    if (scanf("%d", &valeur_lue) != 1) {
         printf("\nERREUR INTERPRETATION: Entrée invalide pour lire(%s).\n", idf);
         int c; while ((c = getchar()) != '\n' && c != EOF);
         exit(1);
    }
     int c; while ((c = getchar()) != '\n' && c != EOF);
    if (!ajouter_symbole(idf, valeur_lue)) {
        exit(1);
    }
    printf("INTERP: lire(%s) -> %d\n", idf, valeur_lue);
}

void interpreter_ecrire(Ast A) {
     if (A == NULL || A->nature != N_ECRIRE || A->gauche == NULL) {
        printf("ERREUR INTERPRETATION: Noeud N_ECRIRE invalide.\n");
        exit(1);
    }
    int valeur = evaluation(A->gauche);
    printf("%d\n", valeur);
}

static void interpreter_si_alors_sinon(Ast A) {
     if (A == NULL || A->nature != N_IF || A->gauche == NULL) {
        printf("ERREUR INTERPRETATION: Noeud N_IF invalide.\n");
        exit(1);
    }
    int condition_result = evaluation(A->gauche);

    if (condition_result != 0) {
        printf("INTERP: Condition IF vraie, exécution THEN\n");
        interpreter(A->milieu);
    } else {
        printf("INTERP: Condition IF fausse, exécution ELSE\n");
        if (A->droite != NULL) {
            interpreter(A->droite);
        } else {
            printf("INTERP: (Pas de branche ELSE à exécuter)\n");
        }
    }
     printf("INTERP: Fin bloc IF\n");
}

void interpreter(Ast A) {
    if (A == NULL) {
        return;
    }

    switch (A->nature) {
        case N_SEPINST:
            interpreter(A->gauche);
            interpreter(A->droite);
            break;
        case N_AFF:
            interpreter_aff(A);
            break;
        case N_LIRE:
            interpreter_lire(A);
            break;
        case N_ECRIRE:
            interpreter_ecrire(A);
            break;
        case N_IF:
            interpreter_si_alors_sinon(A);
            break;
        case N_WHILE:
            printf("INTERP: Entrée boucle TANQUE\n");
            while (evaluation(A->gauche)) {
                printf("INTERP: Condition TANQUE vraie, exécution du corps\n");
                interpreter(A->droite);
            }
            printf("INTERP: Condition TANQUE fausse, sortie de boucle\n");
            break;

        case OPERATION:
        case VALEUR:
        case N_IDF:
             printf("ERREUR INTERPRETATION: Noeud AST de type expression (%d) rencontré au niveau instruction.\n", A->nature);
             exit(1);
        default:
             printf("ERREUR INTERPRETATION: Type de noeud AST inconnu pour l'interprétation: %d\n", A->nature);
             exit(1);
    }
}