all : ../tigger

../tigger : eeyore.lex eeyore.y tree.h tree.cpp graph.h graph.cpp process.h process.cpp
	bison -d eeyore.y
	flex eeyore.lex
	g++ -o ../tigger eeyore.tab.c tree.cpp graph.cpp process.cpp -std=c++11

clean :
	rm ../tigger eeyore.tab.* lex.yy.c
