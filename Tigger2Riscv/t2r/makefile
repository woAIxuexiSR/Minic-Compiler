all : ../riscv64

../riscv64 : tigger.y tigger.lex
	bison -d tigger.y
	flex tigger.lex
	g++ -o ../riscv64 tigger.tab.c -std=c++11

clean :
	rm ../riscv64 tigger.tab.* lex.yy.c
