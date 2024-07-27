/* file parseur.y
 * compilation: bison -d parseur.y
 * result = parseur.tab.c parseur.tab.h
 */

%code requires {
	#include <stdio.h>
	#include <math.h>
	#include "AST.h"
	#include "list.h"
	int yylex(void);
	int yyerror(LST_comm *rez,const char*);
}

%parse-param { LST_comm *rez }

%union { AST_expr expr; double number; unsigned int boolean; char string[256]; AST_comm comm; }
%token <number> NUMBER
%type <expr> expression
%type <comm> commande
%token <boolean> BOOLEAN
%token EQUALS
%token INF_EQ
%token AND
%token <string> IDENTIFIANT
%token IMPORT
%token IF
%token ELSE
%token DO
%token WHILE

%start programme

%right '='
%left AND
%left EQUALS
%left INF_EQ '<'
%left '+' '-'
%left '*' '/'
%left '!'
%nonassoc UMOINS

%%

programme :
	programme commande
		{ enqueue(*rez,$2); }
|	%empty
;

commande :
	expression ';'
		{ $$ = new_command(R_EXPR,$1,NULL,NULL,NULL); }
|	IF '(' expression ')' commande ELSE commande
		{ $$ = new_command(R_IF,$3,$5,$7,NULL); }
|	DO commande WHILE '(' expression ')' ';'
		{ $$ = new_command(R_DO,$5,$2,NULL,NULL); }
|	IMPORT IDENTIFIANT ';'
		{ $$ = new_command(R_IMPORT,new_leaf_expr(R_ID,0,0,$2),NULL,NULL,NULL); }
|	'{' programme '}'
		{ $$ = new_command(R_BR,NULL,NULL,NULL,*rez); *rez = new_queue(); }
|	';'
		{ $$ = NULL; }
;

expression:
	expression '+' expression
		{ $$ = new_binary_expr(R_ADDI,$1,$3); }
|	expression '-' expression
		{ $$ = new_binary_expr(R_SOUS,$1,$3); }
|	expression '*' expression
		{ $$ = new_binary_expr(R_MULT,$1,$3); }
|	expression '/' expression
		{ $$ = new_binary_expr(R_DIVI,$1,$3); }
|	'(' expression ')'
		{ $$ = $2; }
|	expression EQUALS expression
		{ $$ = new_binary_expr(R_EQUALS,$1,$3); }
|	expression INF_EQ expression
		{ $$ = new_binary_expr(R_INF_EQ,$1,$3); }
|	expression '<' expression
		{ $$ = new_binary_expr(R_INF,$1,$3); }
|	expression AND expression
		{ $$ = new_binary_expr(R_AND,$1,$3); }
|	'!' expression
		{ $$ = new_unary_expr(R_NOT,$2);}
|	'-' expression %prec UMOINS
		{ $$ = new_unary_expr(R_MOINS,$2); }
|	IDENTIFIANT '=' expression
		{ $$ = new_binary_expr(R_ASSIGN,new_leaf_expr(R_ID,0,0,$1),$3); }
|	NUMBER
		{ $$ = new_leaf_expr(R_NB,$1,0,NULL); }
|	BOOLEAN
		{ $$ = new_leaf_expr(R_BOOL,0,$1,NULL);}
|	IDENTIFIANT
		{ $$ = new_leaf_expr(R_ID,0,0,$1); }
;

%%

int yyerror(LST_comm *rez, const char *msg) {
	printf("Parsing:: syntax error\n");
	return 1;
}