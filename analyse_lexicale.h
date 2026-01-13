#ifndef __ANALYSE_LEXICALE_H__
#define __ANALYSE_LEXICALE_H__

typedef enum {
    ERREUR_CAR,
    CHIFFRE,
    SYMBOLE,
    IDENTIFICATEUR,
    C_FIN_SEQUENCE
} Nature_Caractere;

typedef enum {
    ENTIER,
    IDF,
    PLUS,
    MOINS,
    MUL,
    DIV,
    AFF,
    SEPINST,
    PARO,
    PARF,
    LIRE,
    ECRIRE,
    IF,
    THEN,
    ELSE,
    FI,
    EQ,
    NEQ,
    LT,
    LE,
    GT,
    GE,
    TANQUE,
    FAIRE,
    FAIT,
    FIN_SEQUENCE,
    ERREUR
} Nature_Lexeme;

typedef struct {
    Nature_Lexeme nature;
    unsigned int ligne;
    unsigned int colonne;
    char chaine[256];
    int valeur;
} Lexeme;

void demarrer(char *nom_fichier);
void avancer();
Lexeme lexeme_courant();
int fin_de_sequence();
void arreter();

void afficher(Lexeme l);
char *Nature_vers_Chaine (Nature_Lexeme nature);

#endif // __ANALYSE_LEXICALE_H__