%{
#include <iostream>
#include <string>
#include "lex.yy.c"
using namespace std;

void yyerror(string msg);

extern FILE *yyin;
int stk;

%}

%union
{
	char* name;
	int val;
}

%token MALLOC END ASSIGN LFKH RFKH IF GOTO
%token CALL STORE LOAD LOADADDR RETURN COLON
%token<val> NUM
%token<name> REG FUNC LABEL OP GVAR

%%

Goal : FuncDecl Goal {}
	 | GVarDecl Goal {}
	 | FuncDecl {}
	 | GVarDecl {}
	 ;

GVarDecl : GVAR ASSIGN NUM {
				printf("\t.global %s\n", $1);
				printf("\t.section .sdata\n");
				printf("\t.align 2\n");
				printf("\t.type %s, @object\n", $1);
				printf("\t.size %s, 4\n", $1);
				printf("%s:\n", $1);
				printf("\t.word %d\n", $3);
		 }
		 | GVAR ASSIGN MALLOC NUM {
				printf("\t.comm %s, %d, 4\n", $1, 4 * $4);
		 }
		 ;

FuncDecl : FUNC LFKH NUM RFKH LFKH NUM RFKH {
				stk = ($6 / 4 + 1) * 16;

				printf("\t.text\n");
				printf("\t.align 2\n");
				printf("\t.global %s\n", $1 + 2);
				printf("\t.type %s, @function\n", $1 + 2);
				printf("%s:\n", $1 + 2);
				printf("\tadd sp, sp, %d\n", -stk);
				printf("\tsw ra, %d(sp)\n", stk - 4);

		 } StateList END FUNC {
				printf("\t.size %s, .-%s\n", $1 + 2, $1 + 2);
		 }
		 ;

StateList : Statement StateList {}
		  | Statement {}
		  ;

Statement : REG ASSIGN REG OP REG {
				switch($4[0])
				{
				case '+': printf("\tadd %s, %s, %s\n", $1, $3, $5); break;
				case '-': printf("\tsub %s, %s, %s\n", $1, $3, $5); break;
				case '*': printf("\tmul %s, %s, %s\n", $1, $3, $5); break;
				case '/': printf("\tdiv %s, %s, %s\n", $1, $3, $5); break;
				case '%': printf("\trem %s, %s, %s\n", $1, $3, $5); break;
				case '<': printf("\tslt %s, %s, %s\n", $1, $3, $5); break;
				case '>': printf("\tsgt %s, %s, %s\n", $1, $3, $5); break;
				case '|': {
							printf("\tor %s, %s, %s\n", $1, $3, $5);
							printf("\tsnez %s, %s\n", $1, $1);
							break;
						  }
				case '&': {
							printf("\tseqz %s, %s\n", $3, $3);
							printf("\tseqz %s, %s\n", $5, $5);
							printf("\tor %s, %s, %s\n", $1, $3, $5);
							printf("\tseqz %s, %s\n", $1, $1);
							break;
						  }
				case '!': {
							printf("\txor %s, %s, %s\n", $1, $3, $5);
							printf("\tsnez %s, %s\n", $1, $1);
							break;
						  }
				case '=': {
							printf("\txor %s, %s, %s\n", $1, $3, $5);
							printf("\tseqz %s, %s\n", $1, $1);
							break;
						  }
				}
		  }
		  | REG ASSIGN REG OP NUM {
				switch($4[0])
				{
				case '+': printf("\tadd %s, %s, %d\n", $1, $3, $5); break;
				case '<': printf("\tslti %s, %s, %d\n", $1, $3, $5); break;
				}
		  }
		  | REG ASSIGN OP REG {
				switch($3[0])
				{
				case '-': printf("\tsub %s, x0, %s\n", $1, $4); break;
				case '!': printf("\tseqz %s, %s\n", $1, $4); break;
				}
		  }
		  | REG ASSIGN REG {
				printf("\tmv %s, %s\n", $1, $3);
		  }
		  | REG ASSIGN NUM {
				printf("\tli %s, %d\n", $1, $3);
		  }
		  | REG LFKH NUM RFKH ASSIGN REG {
				printf("\tsw %s, -%d(%s)\n", $6, $3, $1);
		  }
		  | REG ASSIGN REG LFKH NUM RFKH {
				printf("\tlw %s, -%d(%s)\n", $1, $5, $3);
		  }
		  | IF REG OP REG GOTO LABEL {
				switch($3[0])
				{
				case '<': printf("\tblt %s, %s, .%s\n", $2, $4, $6); break;
				case '>': printf("\tbgt %s, %s, .%s\n", $2, $4, $6); break;
				case '!': printf("\tbne %s, %s, .%s\n", $2, $4, $6); break;
				case '=': printf("\tbeq %s, %s, .%s\n", $2, $4, $6); break;
				}
		  }
		  | GOTO LABEL {
				printf("\tj .%s\n", $2);
		  }
		  | LABEL COLON {
				printf(".%s:\n", $1);
		  }
		  | CALL FUNC {
				printf("\tcall %s\n", $2 + 2);
		  }
		  | STORE REG NUM {
				printf("\tsw %s, %d(sp)\n", $2, 4 * $3);
		  }
		  | LOAD NUM REG {
				printf("\tlw %s, %d(sp)\n", $3, 4 * $2);
		  }
		  | LOAD GVAR REG {
				printf("\tlui %s, %%hi(%s)\n", $3, $2);
				printf("\tlw %s, %%lo(%s)(%s)\n", $3, $2, $3);
		  }
		  | LOADADDR NUM REG {
				printf("\tadd %s, sp, %d\n", $3, 4 * $2);
		  }
		  | LOADADDR GVAR REG {
				printf("\tlui %s, %%hi(%s)\n", $3, $2);
				printf("\tadd %s, %s, %%lo(%s)\n", $3, $3, $2);
		  }
		  | RETURN {
				printf("\tlw ra, %d(sp)\n", stk - 4);
				printf("\tadd sp, sp, %d\n", stk);
				printf("\tjr ra\n");
		  }
		  ;

%%

int main(int argc, char** argv)
{
	/*if(argc != 2)
	{
		printf("you should input one tigger file\n");
	}
	FILE *fp_in = fopen(argv[1], "r");
	yyin = fp_in;
	freopen("./out.r", "w", stdout);*/

	yyparse();


	return 0;
}

void yyerror(string msg)
{
	cout << "error message " << msg << endl;
}
