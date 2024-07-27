/* file lexeur.l
 * compilation: flex lexeur.l
 * result: lex.yy.c
 */

%{
	#include <stdio.h>
	#include <math.h>
	#include <string.h>
	#include "parseur.tab.h"
%}

%%

([0-9]+([.][0-9]*)?|[.][0-9]+) {
	printf("lex:: token NUMBER %s\n",yytext);
	yylval.number=atof(yytext);
	return NUMBER; }

(([0-9]+([.][0-9]*)?|[.][0-9]+))([eE][-+]?[0-9]+) {
	printf("lex:: token NUMBER %s scientific notation\n",yytext);
	int current = 0;
	while(yytext[current] != 'e' && yytext[current] != 'E') {
		current++;
	}

	yytext[current] ='\0';
	int exp = atoi(yytext+current+1);
	double number = atof(yytext);
	printf("%lf et %d \n",number,exp);
    
	int pow(int exposant) {
		int result = 1;
		for(int i = 0; i < exposant; i++)
			result *= 10;
		return result;
	}

	if(!exp)
		yylval.number = (number == 0) ? 0 : 1;
	else if(exp < 0)
		yylval.number = number / pow(-exp);
	else if(exp > 0)
		yylval.number = number * pow(exp);

	return NUMBER;
}

"NaN" {
	printf("lex:: token NUMBER NaN\n");
	yylval.number=NAN;
	return NUMBER; }

"True"|"False" {
	printf("lex:: token BOOLEAN %s\n",yytext);
	yylval.boolean= yytext[0] == 'T' ? 1 : 0;
	return BOOLEAN; }

"<=" {
	printf("lex:: token INF_EQ\n");
	return INF_EQ; }

"==" {
	printf("lex:: token EQUALS\n");
	return EQUALS; }

"&&" {
	printf("lex:: token AND\n");
	return AND;
}

"import" {
	printf("lex:: token IMPORT\n");
	return IMPORT; }

"If" {
	printf("lex:: token IF\n");
	return IF; }
"Else" {
	printf("lex:: token ELSE\n");
	return ELSE; }

"Do" {
	printf("lex:: token DO\n");
	return DO; }

"While" {
	printf("lex:: token WHILE\n");
	return WHILE; }

([a-z]|[A-Z]|[_])([a-z]|[A-Z]|[_]|[&]|[0-9])* {
	printf("lex:: token IDENTIFIANT %s\n",yytext);
	strncpy(yylval.string,yytext,256);
	return IDENTIFIANT; }

[/][*]([^*]|([*]+([^*/]|[\r\n])))*[*]+[/]|[ \t\r] { ; }

.	{	printf("lex:: token %s\n",yytext);
		return yytext[0]; }

%%
int yywrap(void) { return 1; }