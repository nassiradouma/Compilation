all: main

main: main.o AST.o list.o parseur.tab.c lex.yy.c
	gcc -Wall -o main main.o parseur.tab.c lex.yy.c AST.o list.o

main.o: main.c
	gcc -Wall -c main.c

parseur.tab.c: parseur.y
	bison -d parseur.y

lex.yy.c:
	flex lexeur.l

list.o: list.c list.h
	gcc -Wall -c list.c

AST.o: AST.c AST.h
	gcc -Wall -c AST.c

clean:
	rm *.o
clear:
	rm main lex.yy.c parseur.tab.* *.o