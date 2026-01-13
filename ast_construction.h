#ifndef __AST_CONSTRUCTION_H__
#define __AST_CONSTRUCTION_H__

#include "type_ast.h"

Ast creer_operation(TypeOperateur opr, Ast opde_gauche, Ast opde_droit);
Ast creer_valeur(int val);
Ast creer_idf(const char *ident);
Ast creer_aff(Ast ident_node, Ast expr);
Ast creer_lire(Ast ident_node);
Ast creer_ecrire(Ast expr);
Ast creer_seqinst(Ast inst1, Ast inst2);
Ast creer_if(Ast cond, Ast seq_then, Ast seq_else);
Ast creer_while(Ast cond, Ast body);

#endif