/* file main.c 
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "AST.h"
#include "list.h"

int main(int argc, char *argv[]) {
	LST_comm rez = new_queue();
	extern FILE* yyin;
	yyin = fopen(argv[1],"r");
	if(!yyparse(&rez)) {
		printf("\nParsing:: expression reconnue\n");
	}
	code_queue(rez);
	free_queue(rez);
	return 0;
}
