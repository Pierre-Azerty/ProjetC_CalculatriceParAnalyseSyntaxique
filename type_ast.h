#ifndef __TYPE_AST__
#define __TYPE_AST__

#include "table_symbole.h"

typedef enum {
    OPERATION,
    VALEUR,
    N_IDF,
    N_AFF,
    N_LIRE,
    N_ECRIRE,
    N_SEPINST,
    N_IF,
    N_WHILE
} TypeAst;

typedef enum {
    N_PLUS, N_MOINS, N_MUL, N_DIV,
    N_EQ, N_NEQ, N_LT, N_LE, N_GT, N_GE
} TypeOperateur;

typedef struct noeud {
    TypeAst nature;
    TypeOperateur operateur;
    struct noeud *gauche;
    struct noeud *droite;
    struct noeud *milieu;
    char ident[MAX_IDF_LEN];
    int valeur;
} NoeudAst;

typedef NoeudAst *Ast;

#endif // __TYPE_AST__