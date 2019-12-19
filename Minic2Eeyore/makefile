all : eeyore

eeyore : minic.lex minic.y symbol.c tree.c symbol.h tree.h
	bison -d minic.y
	flex minic.lex
	gcc -o eeyore minic.tab.c lex.yy.c symbol.c tree.c

clean :
	rm eeyore minic.tab.* lex.yy.c
