#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "type_ast.h"
#include "ast_construction.h"
#include "ast_parcours.h"
#include "analyse_lexicale.h"
#include "table_symbole.h"

Ast rec_seq_inst();
Ast rec_suite_seq_inst(Ast first_inst);
Ast rec_inst();
Ast rec_eag();
Ast seq_terme();
Ast suite_seq_terme(Ast first_term);
Ast terme();
Ast seq_facteur();
Ast suite_seq_facteur(Ast first_factor);
Ast facteur();
int op1(TypeOperateur *Op);
int op2(TypeOperateur *Op);
int opcomp(TypeOperateur *Op);
Ast rec_condition();
TypeOperateur OperateurArithmetique(Nature_Lexeme nature);
TypeOperateur OperateurComparaison(Nature_Lexeme nature);
void erreur_syntaxique(const char *message);

void erreur_syntaxique(const char *message) {
    printf("ERREUR SYNTAXIQUE (ligne %u, colonne %u): %s. Lexeme courant: %s ('%s')\n",
           lexeme_courant().ligne, lexeme_courant().colonne, message,
           Nature_vers_Chaine(lexeme_courant().nature), lexeme_courant().chaine);
    exit(1);
}

TypeOperateur OperateurArithmetique(Nature_Lexeme nature) {
    switch(nature) {
        case PLUS : return N_PLUS;
        case MOINS : return N_MOINS;
        case MUL: return N_MUL;
        case DIV : return N_DIV;
        default:
            printf("Erreur: Nature %s n'est pas un opérateur arithmétique\n", Nature_vers_Chaine(nature));
            exit(1);
    }
}

TypeOperateur OperateurComparaison(Nature_Lexeme nature) {
    switch(nature) {
        case EQ: return N_EQ;
        case NEQ: return N_NEQ;
        case LT: return N_LT;
        case LE: return N_LE;
        case GT: return N_GT;
        case GE: return N_GE;
        default:
            printf("Erreur: Nature %s n'est pas un opérateur de comparaison\n", Nature_vers_Chaine(nature));
            exit(1);
    }
}

int op1(TypeOperateur *Op) {
    Nature_Lexeme nature = lexeme_courant().nature;
    if (nature == PLUS || nature == MOINS) {
        *Op = OperateurArithmetique(nature);
        avancer();
        return 1;
    }
    return 0;
}

int op2(TypeOperateur *Op) {
    Nature_Lexeme nature = lexeme_courant().nature;
    if (nature == MUL || nature == DIV) {
        *Op = OperateurArithmetique(nature);
        avancer();
        return 1;
    }
    return 0;
}

int opcomp(TypeOperateur *Op) {
    Nature_Lexeme nature = lexeme_courant().nature;
    switch(nature) {
        case EQ:
        case NEQ:
        case LT:
        case LE:
        case GT:
        case GE:
            *Op = OperateurComparaison(nature);
            avancer();
            return 1;
        default:
            return 0;
    }
}

Ast rec_condition() {
    Ast Ag, Ad, Acond;
    TypeOperateur op;

    Ag = rec_eag();

    if (opcomp(&op)) {
        Ad = rec_eag();
        Acond = creer_operation(op, Ag, Ad);
    } else {
        erreur_syntaxique("Opérateur de comparaison (==, !=, <, <=, >, >=) attendu dans la condition");
        Acond = NULL;
    }
    return Acond;
}

Ast facteur() {
    Ast A1 = NULL;
    Ast A_zero = NULL;

    switch(lexeme_courant().nature){
        case ENTIER:
            A1 = creer_valeur(lexeme_courant().valeur);
            avancer();
            break;
        case PARO:
            avancer();
            A1 = rec_eag();
            if(lexeme_courant().nature == PARF) {
                avancer();
            } else {
                erreur_syntaxique("Parenthese fermante ')' attendue après l'expression dans les parenthèses");
            }
            break;
        case IDF:
            {
                char nom_var[MAX_IDF_LEN];
                strncpy(nom_var, lexeme_courant().chaine, MAX_IDF_LEN -1);
                nom_var[MAX_IDF_LEN - 1] = '\0';
                A1 = creer_idf(nom_var);
                avancer();
            }
            break;
        case MOINS:
            avancer();
            A_zero = creer_valeur(0);
            A1 = facteur();
            if (A1 == NULL) {
                erreur_syntaxique("Facteur (nombre, variable, expression parenthésée) attendu après le signe '-' unaire");
            }
            A1 = creer_operation(N_MOINS, A_zero, A1);
            break;
        default:
             erreur_syntaxique("Facteur (Entier, Identifiant, '(', ou signe '-') attendu");
    }
    return A1;
}

Ast suite_seq_facteur(Ast A1) {
    Ast A2, A3;
    TypeOperateur op;
    if(op2(&op)) {
        A3 = facteur();
        A2 = creer_operation(op, A1, A3);
        return suite_seq_facteur(A2);
    } else {
        return A1;
    }
}

Ast seq_facteur() {
    Ast A1 = facteur();
    return suite_seq_facteur(A1);
}

Ast terme() {
    return seq_facteur();
}

Ast suite_seq_terme(Ast A1) {
    Ast A2, A3;
    TypeOperateur op;

    if(op1(&op)) {
        A3 = terme();
        A2 = creer_operation(op, A1, A3);
        return suite_seq_terme(A2);
    } else {
        return A1;
    }
}

Ast seq_terme() {
    Ast A1 = terme();
    return suite_seq_terme(A1);
}

Ast rec_eag() {
    return seq_terme();
}

Ast rec_inst() {
    Ast A = NULL;
    Ast condition_ast, then_ast, else_ast;
    Ast body_ast;
    Ast idf_ast, expr_ast;

    switch (lexeme_courant().nature) {
        case IDF:
            {
                char identif[MAX_IDF_LEN];
                strncpy(identif, lexeme_courant().chaine, MAX_IDF_LEN-1);
                identif[MAX_IDF_LEN-1] = '\0';
                idf_ast = creer_idf(identif);
                avancer();

                if (lexeme_courant().nature == AFF) {
                    avancer();
                    expr_ast = rec_eag();
                    A = creer_aff(idf_ast, expr_ast);
                } else {
                    erreur_syntaxique("Symbole d'affectation '=' attendu après l'identifiant");
                }
            }
            break;

        case LIRE:
            {
                avancer();
                if (lexeme_courant().nature == PARO) {
                    avancer();
                    if (lexeme_courant().nature == IDF) {
                        char idf_lu[MAX_IDF_LEN];
                        strncpy(idf_lu, lexeme_courant().chaine, MAX_IDF_LEN -1);
                        idf_lu[MAX_IDF_LEN - 1] = '\0';
                        idf_ast = creer_idf(idf_lu);
                        avancer();

                        if (lexeme_courant().nature == PARF) {
                            avancer();
                            A = creer_lire(idf_ast);
                        } else {
                            erreur_syntaxique("Parenthese fermante ')' attendue apres l'identifiant dans lire");
                        }
                    } else {
                        erreur_syntaxique("Identifiant attendu dans lire()");
                    }
                } else {
                    erreur_syntaxique("Parenthese ouvrante '(' attendue apres lire");
                }
            }
            break;

        case ECRIRE:
            {
                avancer();
                if (lexeme_courant().nature == PARO) {
                    avancer();
                    expr_ast = rec_eag();
                    if (lexeme_courant().nature == PARF) {
                        avancer();
                        A = creer_ecrire(expr_ast);
                    } else {
                        erreur_syntaxique("Parenthese fermante ')' attendue apres l'expression dans ecrire");
                    }
                } else {
                    erreur_syntaxique("Parenthese ouvrante '(' attendue apres ecrire");
                }
            }
            break;

        case IF:
            avancer();
            condition_ast = rec_condition();

            if (lexeme_courant().nature == THEN) {
                avancer();
            } else {
                erreur_syntaxique("Mot-clé 'then' attendu après la condition du 'if'");
            }

            then_ast = rec_seq_inst();
            if (then_ast == NULL && lexeme_courant().nature != ELSE && lexeme_courant().nature != FI) {
                erreur_syntaxique("Séquence d'instructions attendue après 'then'");
            }

            if (lexeme_courant().nature == ELSE) {
                avancer();
                else_ast = rec_seq_inst();
                if (else_ast == NULL && lexeme_courant().nature != FI) {
                    erreur_syntaxique("Séquence d'instructions attendue après 'else'");
                }
            } else {
                else_ast = NULL;
            }

            if (lexeme_courant().nature == FI) {
                avancer();
            } else {
                erreur_syntaxique("Mot-clé 'fi' attendu pour terminer l'instruction 'if'");
            }

            A = creer_if(condition_ast, then_ast, else_ast);
            break;

        case TANQUE:
            avancer();
            condition_ast = rec_condition();

            if (lexeme_courant().nature == FAIRE) {
                avancer();
            } else {
                erreur_syntaxique("Mot-clé 'faire' attendu après la condition du 'tanque'");
            }

            body_ast = rec_seq_inst();

            if (body_ast == NULL && lexeme_courant().nature != FAIT) {
                erreur_syntaxique("Séquence d'instructions ou mot-clé 'fait' attendu après 'faire'");
            }

            if (lexeme_courant().nature == FAIT) {
                avancer();
            } else {
                erreur_syntaxique("Mot-clé 'fait' attendu pour terminer l'instruction 'tanque'");
            }

            A = creer_while(condition_ast, body_ast);
            break;

        default:
            if (lexeme_courant().nature != FIN_SEQUENCE) {
                erreur_syntaxique("Début d'instruction (identifiant, 'lire', 'ecrire', 'if', 'tanque') attendu");
            }
            break;
    }
    return A;
}

Ast rec_suite_seq_inst(Ast first_inst) {
    if (lexeme_courant().nature == SEPINST) {
        avancer();

        if (lexeme_courant().nature == FIN_SEQUENCE || lexeme_courant().nature == FI || lexeme_courant().nature == ELSE || lexeme_courant().nature == FAIT) {
            return creer_seqinst(first_inst, NULL);
        }

        Ast next_seq_ast = rec_seq_inst();
        if (next_seq_ast == NULL && lexeme_courant().nature != FIN_SEQUENCE && lexeme_courant().nature != FI && lexeme_courant().nature != ELSE && lexeme_courant().nature != FAIT) {
            erreur_syntaxique("Instruction ou fin de bloc (fi, else, fait) attendue après ';'");
        }
        return creer_seqinst(first_inst, next_seq_ast);

    } else {
        return first_inst;
    }
}

Ast rec_seq_inst() {
    if (lexeme_courant().nature == IDF || lexeme_courant().nature == LIRE || lexeme_courant().nature == ECRIRE || lexeme_courant().nature == IF || lexeme_courant().nature == TANQUE) {
        Ast first_inst_ast = rec_inst();

        if (first_inst_ast == NULL) {
            if (lexeme_courant().nature != FIN_SEQUENCE && lexeme_courant().nature != FI && lexeme_courant().nature != ELSE && lexeme_courant().nature != FAIT) {
                erreur_syntaxique("Erreur interne ou lexème inattendu pendant l'analyse d'instruction");
            }
             return NULL;
        }
        return rec_suite_seq_inst(first_inst_ast);
    }
    else if (lexeme_courant().nature == FIN_SEQUENCE || lexeme_courant().nature == FI || lexeme_courant().nature == ELSE || lexeme_courant().nature == FAIT) {
         return NULL;
    }
    else {
        erreur_syntaxique("Début d'instruction (identifiant, 'lire', 'ecrire', 'if', 'tanque') ou fin de bloc/séquence (fi, else, fait) attendu");
        return NULL;
    }
}

int analyser(char* nomFichier, Ast *arbre_final) {
    Ast A = NULL;
    demarrer(nomFichier);
    init_table();

    A = rec_seq_inst();

    if (lexeme_courant().nature != FIN_SEQUENCE) {
        erreur_syntaxique("Caractères inattendus après la fin du programme");
    }

    arreter();

    if (A != NULL) {
        printf("\nAnalyse syntaxique et construction de l'AST terminées avec succès.\n");
        *arbre_final = A;
        return 1;
    } else {
         if (fin_de_sequence()) {
             printf("\nAnalyse terminée: Fichier vide ou ne contient aucune instruction valide.\n");
             *arbre_final = NULL;
             return 1;
         } else {
             printf("\nAnalyse échouée (AST final NULL mais pas fin de séquence).\n");
              *arbre_final = NULL;
             return 0;
         }
    }
}