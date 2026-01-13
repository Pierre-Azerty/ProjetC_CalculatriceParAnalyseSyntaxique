#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lecture_caracteres.h"
#include "analyse_lexicale.h"

Lexeme lexeme_en_cours;

#define NB_MOTS_CLES 9
const char mots_cles[NB_MOTS_CLES][20] = {
    "lire", "ecrire", "if", "then", "else", "fi",
    "tanque", "faire", "fait"
};

void verifier_mot_cle();
void ajouter_caractere (char *s, char c);
Nature_Caractere nature_caractere (char c);
int est_separateur(char c );
int est_chiffre(char c );
int est_symbole(char c );
int est_idf(char c);
void reconnaitre_lexeme();

void demarrer(char *nom_fichier) {
    demarrer_car(nom_fichier);
    avancer();
}

void avancer() {
    reconnaitre_lexeme();
}

Lexeme lexeme_courant() {
    return lexeme_en_cours;
}

int fin_de_sequence() {
    return lexeme_en_cours.nature == FIN_SEQUENCE;
}

void arreter() {
    arreter_car();
}

void verifier_mot_cle() {
    if (lexeme_en_cours.nature == IDF) {
        for (int i = 0; i < NB_MOTS_CLES; i++) {
            if (strcmp(lexeme_en_cours.chaine, mots_cles[i]) == 0) {
                switch(i) {
                    case 0: lexeme_en_cours.nature = LIRE; break;
                    case 1: lexeme_en_cours.nature = ECRIRE; break;
                    case 2: lexeme_en_cours.nature = IF; break;
                    case 3: lexeme_en_cours.nature = THEN; break;
                    case 4: lexeme_en_cours.nature = ELSE; break;
                    case 5: lexeme_en_cours.nature = FI; break;
                    case 6: lexeme_en_cours.nature = TANQUE; break;
                    case 7: lexeme_en_cours.nature = FAIRE; break;
                    case 8: lexeme_en_cours.nature = FAIT; break;
                    default: break;
                }
                return;
            }
        }
    }
}

void reconnaitre_lexeme() {
    typedef enum {
        E_INIT, E_ENTIER, E_IDENTIFICATEUR,
        E_AFF_OU_EQ, E_NEQ, E_LT_OU_LE, E_GT_OU_GE,
        E_FIN
    } Etat_Automate;
    Etat_Automate etat = E_INIT;
    char car_cour;

    while (est_separateur(caractere_courant())) {
        avancer_car();
    }

    lexeme_en_cours.chaine[0] = '\0';
    lexeme_en_cours.ligne = numero_ligne();
    lexeme_en_cours.colonne = numero_colonne();

    while (etat != E_FIN) {
        car_cour = caractere_courant();

        switch (etat) {
            case E_INIT:
                lexeme_en_cours.ligne = numero_ligne();
                lexeme_en_cours.colonne = numero_colonne();

                switch(nature_caractere(car_cour)) {
                    case C_FIN_SEQUENCE:
                        lexeme_en_cours.nature = FIN_SEQUENCE;
                        etat = E_FIN;
                        break;

                    case CHIFFRE:
                        lexeme_en_cours.nature = ENTIER;
                        ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                        lexeme_en_cours.valeur = car_cour - '0';
                        etat = E_ENTIER;
                        avancer_car();
                        break;

                    case SYMBOLE:
                        ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                        switch (car_cour) {
                            case '+': lexeme_en_cours.nature = PLUS; etat = E_FIN; avancer_car(); break;
                            case '-': lexeme_en_cours.nature = MOINS; etat = E_FIN; avancer_car(); break;
                            case '*': lexeme_en_cours.nature = MUL; etat = E_FIN; avancer_car(); break;
                            case '/': lexeme_en_cours.nature = DIV; etat = E_FIN; avancer_car(); break;
                            case '(': lexeme_en_cours.nature = PARO; etat = E_FIN; avancer_car(); break;
                            case ')': lexeme_en_cours.nature = PARF; etat = E_FIN; avancer_car(); break;
                            case ';': lexeme_en_cours.nature = SEPINST; etat = E_FIN; avancer_car(); break;
                            case '=': etat = E_AFF_OU_EQ; avancer_car(); break;
                            case '!': etat = E_NEQ; avancer_car(); break;
                            case '<': etat = E_LT_OU_LE; avancer_car(); break;
                            case '>': etat = E_GT_OU_GE; avancer_car(); break;
                            default:
                                printf("Erreur Lexicale Symbole Inconnu (ligne %u, colonne %u): '%c'\n", lexeme_en_cours.ligne, lexeme_en_cours.colonne, car_cour);
                                exit(1);
                        }
                        break;

                    case IDENTIFICATEUR:
                        lexeme_en_cours.nature = IDF;
                        ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                        etat = E_IDENTIFICATEUR;
                        avancer_car();
                        break;

                    default:
                        printf("Erreur Lexicale Caractere Inattendu (ligne %u, colonne %u): '%c'\n", lexeme_en_cours.ligne, lexeme_en_cours.colonne, car_cour);
                        exit(1);
                }
                break;

            case E_AFF_OU_EQ:
                if (car_cour == '=') {
                    lexeme_en_cours.nature = EQ;
                    ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                    avancer_car();
                } else {
                    lexeme_en_cours.nature = AFF;
                }
                etat = E_FIN;
                break;

            case E_NEQ:
                if (car_cour == '=') {
                    lexeme_en_cours.nature = NEQ;
                    ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                    avancer_car();
                } else {
                    printf("Erreur Lexicale: '!' doit être suivi de '=' (ligne %u, colonne %u)\n", lexeme_en_cours.ligne, lexeme_en_cours.colonne);
                    exit(1);
                }
                etat = E_FIN;
                break;

            case E_LT_OU_LE:
                if (car_cour == '=') {
                    lexeme_en_cours.nature = LE;
                    ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                    avancer_car();
                } else {
                    lexeme_en_cours.nature = LT;
                }
                etat = E_FIN;
                break;

            case E_GT_OU_GE:
                if (car_cour == '=') {
                    lexeme_en_cours.nature = GE;
                    ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                    avancer_car();
                } else {
                    lexeme_en_cours.nature = GT;
                }
                etat = E_FIN;
                break;

            case E_ENTIER:
                if (est_chiffre(car_cour)) {
                    if (strlen(lexeme_en_cours.chaine) < sizeof(lexeme_en_cours.chaine) - 1) {
                        ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                        lexeme_en_cours.valeur = lexeme_en_cours.valeur * 10 + (car_cour - '0');
                    } else {
                        printf("Erreur Lexicale: Entier trop long (ligne %u, colonne %u)\n", lexeme_en_cours.ligne, lexeme_en_cours.colonne);
                        exit(1);
                    }
                    avancer_car();
                } else {
                    etat = E_FIN;
                }
                break;

            case E_IDENTIFICATEUR:
                if (est_idf(car_cour) || est_chiffre(car_cour)) {
                    if (strlen(lexeme_en_cours.chaine) < sizeof(lexeme_en_cours.chaine) - 1) {
                        ajouter_caractere(lexeme_en_cours.chaine, car_cour);
                    } else {
                        printf("Erreur Lexicale: Identifiant/Mot-clé trop long (ligne %u, colonne %u)\n", lexeme_en_cours.ligne, lexeme_en_cours.colonne);
                        exit(1);
                    }
                    avancer_car();
                } else {
                    verifier_mot_cle();
                    etat = E_FIN;
                }
                break;

            case E_FIN:
                break;
        }
    }
}

void ajouter_caractere (char *s, char c) {
    int l;
    l = strlen(s);
    if (l < sizeof(lexeme_en_cours.chaine) - 1) {
        s[l] = c;
        s[l+1] = '\0';
    }
}

Nature_Caractere nature_caractere (char c) {
    if (fin_de_sequence_car(c)) return C_FIN_SEQUENCE;
    if (est_chiffre(c)) return CHIFFRE;
    if (est_idf(c)) return IDENTIFICATEUR;
    if (est_symbole(c)) return SYMBOLE;
    return ERREUR_CAR;
}

int est_separateur(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int est_chiffre(char c) {
    return c >= '0' && c <= '9';
}

int est_idf(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int est_symbole(char c) {
    switch (c) {
        case '+': case '-': case '*': case '/':
        case '(': case ')': case ';':
        case '=': case '!': case '<': case '>':
            return 1;
        default:
            return 0;
    }
}

char *Nature_vers_Chaine (Nature_Lexeme nature) {
    switch (nature) {
        case ENTIER: return "ENTIER";
        case IDF: return "IDF";
        case LIRE: return "LIRE";
        case ECRIRE: return "ECRIRE";
        case IF: return "IF";
        case THEN: return "THEN";
        case ELSE: return "ELSE";
        case FI: return "FI";
        case EQ: return "EQ (==)";
        case NEQ: return "NEQ (!=)";
        case LT: return "LT (<)";
        case LE: return "LE (<=)";
        case GT: return "GT (>)";
        case GE: return "GE (>=)";
        case TANQUE: return "TANQUE";
        case FAIRE: return "FAIRE";
        case FAIT: return "FAIT";
        case PLUS: return "PLUS";
        case MOINS: return "MOINS";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case AFF: return "AFF (=)";
        case SEPINST: return "SEPINST (;)";
        case PARO: return "PARO";
        case PARF: return "PARF";
        case FIN_SEQUENCE: return "FIN_SEQUENCE";
        case ERREUR: return "ERREUR_LEXICALE";
        default: return "ERREUR_NATURE_INCONNUE";
    };
}

void afficher(Lexeme l) {
    switch (l.nature) {
        case FIN_SEQUENCE:
            break;
        default:
            printf("(ligne %d, ", l.ligne);
            printf("colonne %d) : ", l.colonne);
            printf("[");
            printf("nature = %s", Nature_vers_Chaine(l.nature));
            printf(", chaine = '%s'", l.chaine);
            if (l.nature == ENTIER) {
                printf(", valeur = %d", l.valeur);
            }
            printf("]");
    };
}