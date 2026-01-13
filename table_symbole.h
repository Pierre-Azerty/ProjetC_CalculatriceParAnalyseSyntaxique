#ifndef __TABLE_SYMBOLE__
#define __TABLE_SYMBOLE__

#define MAX_SYMBOLS 100
#define MAX_IDF_LEN 256

typedef struct {
    char nom[MAX_IDF_LEN];
    int valeur;
} Symbole;

typedef struct {
    Symbole table[MAX_SYMBOLS];
    int nb_symboles;
} TableSymboles;

extern TableSymboles table_globale;

void init_table();
Symbole* rechercher_symbole(const char* nom);
int ajouter_symbole(const char* nom, int valeur);
int get_valeur_symbole(const char* nom);
void afficher_table();

#endif // __TABLE_SYMBOLE__