#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type_ast.h"
#include "ast_construction.h"

Ast allouer_noeud(TypeAst type) {
    Ast nouveau_noeud = (Ast) malloc(sizeof(NoeudAst));
    if (!nouveau_noeud) {
        perror("Erreur d'allocation mémoire pour noeud AST");
        exit(1);
    }
    nouveau_noeud->nature = type;
    nouveau_noeud->gauche = NULL;
    nouveau_noeud->droite = NULL;
    nouveau_noeud->milieu = NULL;
    nouveau_noeud->ident[0] = '\0';
    return nouveau_noeud;
}

Ast creer_operation(TypeOperateur opr , Ast opde_gauche , Ast opde_droit) {
    Ast expr = allouer_noeud(OPERATION);
    expr->operateur = opr;
    if (opde_gauche == NULL || opde_droit == NULL) {
        printf("ERREUR_CONSTRUCTION: Operandes NULL pour creer_operation (opérateur %d)\n", opr);
        exit(1);
    }
    expr->gauche = opde_gauche;
    expr->droite = opde_droit;
    expr->milieu = NULL;
    return expr;
}

Ast creer_valeur(int val) {
    Ast expr = allouer_noeud(VALEUR);
    expr->valeur = val;
    return expr;
}

Ast creer_idf(const char *ident) {
    Ast idf_node = allouer_noeud(N_IDF);
    strncpy(idf_node->ident, ident, MAX_IDF_LEN - 1);
    idf_node->ident[MAX_IDF_LEN - 1] = '\0';
    return idf_node;
}

Ast creer_aff(Ast idf_node, Ast expr) {
    if (idf_node == NULL || idf_node->nature != N_IDF) {
        printf("ERREUR_CONSTRUCTION: Le fils gauche de creer_aff doit être un N_IDF.\n");
        exit(1);
    }
    if (expr == NULL) {
        printf("ERREUR_CONSTRUCTION: Le fils droit (expression) de creer_aff ne peut pas être NULL.\n");
        exit(1);
    }
    Ast aff_node = allouer_noeud(N_AFF);
    aff_node->gauche = idf_node;
    aff_node->droite = expr;
    return aff_node;
}

Ast creer_lire(Ast idf_node) {
    if (idf_node == NULL || idf_node->nature != N_IDF) {
        printf("ERREUR_CONSTRUCTION: Le fils de creer_lire doit être un N_IDF.\n");
        exit(1);
    }
    Ast lire_node = allouer_noeud(N_LIRE);
    lire_node->gauche = idf_node;
    return lire_node;
}

Ast creer_ecrire(Ast expr) {
    if (expr == NULL) {
        printf("ERREUR_CONSTRUCTION: Le fils (expression) de creer_ecrire ne peut pas être NULL.\n");
        exit(1);
    }
    Ast ecrire_node = allouer_noeud(N_ECRIRE);
    ecrire_node->gauche = expr;
    return ecrire_node;
}

Ast creer_seqinst(Ast inst1, Ast inst2) {
    if (inst1 == NULL) {
        printf("ERREUR_CONSTRUCTION: La première instruction (fils gauche) de creer_seqinst ne peut pas être NULL.\n");
        exit(1);
    }

    Ast seq_node = allouer_noeud(N_SEPINST);
    seq_node->gauche = inst1;
    seq_node->droite = inst2;
    return seq_node;
}

Ast creer_if(Ast cond, Ast seq_then, Ast seq_else) {
    if (cond == NULL) {
        printf("ERREUR_CONSTRUCTION: La condition (fils gauche) de creer_if ne peut pas être NULL.\n");
        exit(1);
    }
    if (seq_then == NULL) {
        printf("ERREUR_CONSTRUCTION: La branche 'then' (fils milieu) de creer_if ne peut pas être NULL.\n");
        exit(1);
    }

    Ast if_node = allouer_noeud(N_IF);
    if_node->gauche = cond;
    if_node->milieu = seq_then;
    if_node->droite = seq_else;

    return if_node;
}

Ast creer_while(Ast cond, Ast body) {
    if (cond == NULL) {
        printf("ERREUR_CONSTRUCTION: La condition (fils gauche) de creer_while ne peut pas être NULL.\n");
        exit(1);
    }
    if (body == NULL) {
         printf("ERREUR_CONSTRUCTION: Le corps (fils droit) de creer_while ne peut pas être NULL (séquence vide non supportée ici).\n");
         exit(1);
    }

    Ast while_node = allouer_noeud(N_WHILE);
    while_node->gauche = cond;
    while_node->droite = body;
    while_node->milieu = NULL;

    return while_node;
}