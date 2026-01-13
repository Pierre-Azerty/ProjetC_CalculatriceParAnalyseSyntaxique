CFLAGS = -g -Wall -I.
CC = gcc

ifndef FLEX

	# --- Règles de compilation lorsque l'on n'utilise pas flex ---

	# Cible principale renommée en "interpreteur", ajout de table_symbole.o
interpreteur: calculette.o analyse_syntaxique.o analyse_lexicale.o ast_construction.o ast_parcours.o lecture_caracteres.o table_symbole.o
	$(CC) $(CFLAGS) -o interpreteur calculette.o analyse_syntaxique.o analyse_lexicale.o ast_construction.o ast_parcours.o lecture_caracteres.o table_symbole.o

test_lexeme: analyse_lexicale.o lecture_caracteres.o test_lexeme.o
	$(CC) $(CFLAGS) -o test_lexeme analyse_lexicale.o lecture_caracteres.o test_lexeme.o

	# Règles de compilation spécifiques au mode manuel
analyse_lexicale.o: analyse_lexicale.c analyse_lexicale.h lecture_caracteres.h
	$(CC) $(CFLAGS) -c analyse_lexicale.c

lecture_caracteres.o: lecture_caracteres.h lecture_caracteres.c
	$(CC) $(CFLAGS) -c lecture_caracteres.c

clean:
	# Nettoyage: remplace calculette par interpreteur, ajoute table_symbole.o
	rm -f interpreteur test_lexeme *.o core*

else

	# --- Règles de compilation lorsque l'on utilise flex ---

	# Cible principale renommée en "interpreteur", ajout de table_symbole.o
interpreteur: calculette.o analyse_syntaxique.o analyse_lexicale.o ast_construction.o ast_parcours.o table_symbole.o
	# Pas besoin de lecture_caracteres.o ici
	$(CC) $(CFLAGS) -o interpreteur calculette.o analyse_syntaxique.o analyse_lexicale.o ast_construction.o ast_parcours.o table_symbole.o

test_lexeme: analyse_lexicale.o test_lexeme.o
	# Pas besoin de lecture_caracteres.o ici
	$(CC) $(CFLAGS) -o test_lexeme test_lexeme.o analyse_lexicale.o

	# Règles de compilation spécifiques au mode Flex
analyse_lexicale.o: analyse_lexicale.c analyse_lexicale.h
	$(CC) $(CFLAGS) -c analyse_lexicale.c

	# Règle pour générer l'analyseur lexical avec Flex
	# Assurez-vous que le fichier .l s'appelle bien calculette.l ou adaptez
analyse_lexicale.c : calculette.l analyse_lexicale.h
	flex -o analyse_lexicale.c calculette.l

clean:
	# Nettoyage: remplace calculette par interpreteur, ajoute table_symbole.o
	# Conserve la suppression des fichiers générés par flex
	rm -f interpreteur test_lexeme analyse_lexicale.c lex.yy.c *.o core*

endif


ast_construction.o: ast_construction.c ast_construction.h type_ast.h
	$(CC) $(CFLAGS) -c ast_construction.c

ast_parcours.o: ast_parcours.c ast_parcours.h type_ast.h
	$(CC) $(CFLAGS) -c ast_parcours.c

calculette.o: calculette.c analyse_syntaxique.h table_symbole.h type_ast.h
	$(CC) $(CFLAGS) -c calculette.c

test_lexeme.o: test_lexeme.c analyse_lexicale.h
	$(CC) $(CFLAGS) -c test_lexeme.c

analyse_syntaxique.o: analyse_syntaxique.c analyse_syntaxique.h analyse_lexicale.h ast_construction.h type_ast.h ast_parcours.h table_symbole.h
	$(CC) $(CFLAGS) -c analyse_syntaxique.c

table_symbole.o: table_symbole.c table_symbole.h analyse_lexicale.h
	$(CC) $(CFLAGS) -c table_symbole.c

.PHONY: clean all

# all: interpreteur