all: parser.tab.c lex.yy.c minic.exe

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

minic.exe: parser.tab.c lex.yy.c main.c
	gcc -o minic.exe parser.tab.c lex.yy.c main.c -lfl

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h minic.exe
