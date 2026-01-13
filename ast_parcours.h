// ast_parcours.h (Modification/Ajout)
#ifndef __AST_PARCOURS_H__
#define __AST_PARCOURS_H__

#include "type_ast.h"

void afficher_ast(Ast A);

int evaluation(Ast expr);

void interpreter(Ast A);

#endif