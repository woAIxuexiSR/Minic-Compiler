%{
#include <stdio.h>
#include "symbol.h"
#include "tree.h"

void yyerror(char* msg);

extern FILE* yyin;
//FILE *note_fp;
Scope *main_scope, *cur_sp;
TreeNode* root;
int tagstack[MAXSTACK];
int top = 0;
extern int param_num; 
ORIGINID* func_ck;
int func_num = 0;
int rflag = 0;
%}

%union
{
	char* name;
	struct TreeNode* node;
	int val;
}
%token COMMENT WS SEMI COMMA LYKH RYKH LFKH RFKH LDKH RDKH
%token <name> ID INT RETURN IF ELSE WHILE MAIN ASSIGN
%token <name> ADDSUB MULDIVMOD EQUN AND OR CMP NOT
%token <val> NUM

%type <node> VarDefn
%type <node> Statement FuncHead
%type <node> Expression Variable ParamList Identifier
%type <val> VarDecList

%right ASSIGN
%left OR
%left AND
%left EQUN
%left CMP
%left ADDSUB
%left MULDIVMOD
%right NOT

%%

Goal : BeforeMain MainFunc	{
		/*fprintf(note_fp, "\nSymbol Table:\n");
		show_symtab(note_fp, cur_sp->st);
		fprintf(note_fp, "\n"); */
	 }
	 ;

BeforeMain : BeforeMain FuncDefn	{
		   
		   }
		   | BeforeMain VarDefn		{

		   }
		   | BeforeMain FuncDecl	{

		   }
		   | {

		   }
		   ;

VarDefn : INT Identifier SEMI	{
			$$ = create_node(10, "int", $2, NULL);

			insert_symtab(cur_sp->st, $2->name, 1);

			translate_tree($$);
		}
		| INT Identifier LFKH NUM RFKH SEMI {
			TreeNode* tmp1 = create_node(5, "num", NULL, NULL);
			tmp1->val = $4;
			TreeNode* tmp2 = create_node(3, "[]", $2, tmp1);
			$$ = create_node(10, "int", tmp2, NULL);

			insert_symtab(cur_sp->st, $2->name, 1);

			translate_tree($$);
		}
		;

VarDecl : INT Identifier {
			insert_symtab(cur_sp->st, $2->name, 3);
		}
		| INT Identifier LFKH RFKH {
			insert_symtab(cur_sp->st, $2->name, 3);
		}
		| INT Identifier LFKH NUM RFKH {
			insert_symtab(cur_sp->st, $2->name, 3);
		}
		;

VarDecList : VarDecList COMMA VarDecl	{ $$ = $1 + 1; }
		   | VarDecl	{ $$ = 1; }
		   | { $$ = 0; }
		   ;

FuncDefn : FuncHead {
			printf("f_%s [%d]\n", $1->name, $1->val);
			check_funcd($1->name, $1->val, 1);
			rflag = 0;
		 } LDKH StateList RDKH {
			/*fprintf(note_fp, "\nSymbol Table:\n");
			show_symtab(note_fp, cur_sp->st);
			fprintf(note_fp, "\n");*/

			param_num = 0;
			cur_sp = cur_sp->pre;
			destroy_scope(cur_sp->nxt);
			cur_sp->nxt = NULL;

			printf("end f_%s\n", $1->name);

			if(rflag == 0)
			{
				printf("\nerror! you need return something\n");
				exit(1);
			}
		 }
		 ;

FuncDecl : FuncHead SEMI {
			/*fprintf(note_fp, "\nSymbol Table:\n");
			show_symtab(note_fp, cur_sp->st);
			fprintf(note_fp, "\n");*/
			check_funcd($1->name, $1->val, 0);

			param_num = 0;
			cur_sp = cur_sp->pre;
			destroy_scope(cur_sp->nxt);
			cur_sp->nxt = NULL;
		 }
		 ;

FuncHead : INT Identifier LYKH {
			cur_sp->nxt = new_scope(cur_sp);
			cur_sp = cur_sp->nxt;
		 } VarDecList RYKH	{
			$$ = create_node(4, $2->name, NULL, NULL);
			$$->val = $5;
		 }
		 ;

MainFunc : INT MAIN LYKH RYKH LDKH {
			printf("f_main [0]\n");

			cur_sp->nxt = new_scope(cur_sp);
			cur_sp = cur_sp->nxt;
			rflag = 0;
		 } StateList RDKH {
			/*fprintf(note_fp, "\nSymbol Table:\n");
			show_symtab(note_fp, cur_sp->st);
			fprintf(note_fp, "\n");*/

			cur_sp = cur_sp->pre;
			destroy_scope(cur_sp->nxt);
			cur_sp->nxt = NULL;

			printf("end f_main\n");
			if(rflag == 0)
			{
				printf("\nerror! you need return something\n");
				exit(1);
			}
		 }
		 ;

Statement : LDKH {
			cur_sp->nxt = new_scope(cur_sp);
			cur_sp = cur_sp->nxt;
		  } StateList RDKH	{
			/*fprintf(note_fp, "\nSymbol Table:\n");
			show_symtab(note_fp, cur_sp->st);
			fprintf(note_fp, "\n");*/

			cur_sp = cur_sp->pre;
			destroy_scope(cur_sp->nxt);
			cur_sp->nxt = NULL;
		  }
		  | IfHead Statement {
			tagstack[top++] = gettag();
			printf("goto l%d\n", tagstack[top - 1]);
			printf("l%d:\n", tagstack[top - 2]);

		  } ELSE Statement	{
			printf("l%d:\n", tagstack[top - 1]);
			top -= 2;

		  }
		  | IfHead Statement	{
			printf("l%d:\n", tagstack[top - 1]);
			top--;
		  }
		  | WHILE LYKH Expression RYKH {
			TreeNode* tmp = create_node(8, "if", $3, NULL);
			tagstack[top++] = trans_ifwhile(tmp);

		  } Statement {
			printf("goto l%d\n", tagstack[top - 1]);
			printf("l%d:\n", tagstack[top - 1] + 1);
			top--;

		  }
		  | Identifier ASSIGN Expression SEMI	{
			$$ = create_node(2, "=", $1, $3);

			translate_tree($$);
		  }
		  | Identifier LFKH Expression RFKH ASSIGN Expression SEMI {
			TreeNode *tmp = create_node(11, "[]", $1, $3);
			$$ = create_node(2, "=", tmp, $6);

			translate_tree($$);
		  }
		  | FuncDecl	{

		  }
		  | VarDefn		{
			$$ = $1;

		  }
		  | RETURN Expression SEMI	{
			$$ = create_node(9, "return", $2, NULL);

			translate_tree($$);
		  }
		  | Expression SEMI	{
			$$ = $1;

			translate_tree($$);
		  }
		  ;

IfHead : IF LYKH Expression RYKH {
			TreeNode* tmp = create_node(7, "if", $3, NULL);
			tagstack[top++] = trans_ifwhile(tmp);
	   }
	   ;

StateList : StateList Statement	{

		  }
		  | Statement	{

		  }
		  | {

		  }
		  ;

Expression : Expression OR Expression	{
			$$ = create_node(1, $2, $1, $3);
		   }
		   | Expression AND Expression	{
			$$ = create_node(1, $2, $1, $3);
		   }
		   | Expression EQUN Expression {
			$$ = create_node(1, $2, $1, $3);
		   }
		   | Expression CMP Expression {
			$$ = create_node(1, $2, $1, $3);
		   }
		   | Expression ADDSUB Expression {
			$$ = create_node(1, $2, $1, $3);
		   }
		   | Expression MULDIVMOD Expression {
			$$ = create_node(1, $2, $1, $3);
		   }
		   | ADDSUB Expression		{
			$$ = create_node(1, $1, $2, NULL);
		   }
		   | NOT Expression		{
			$$ = create_node(1, $1, $2, NULL);
		   }
		   | LYKH Expression RYKH	{
			$$ = $2;
		   }
		   | Variable	{
			$$ = $1;
		   }
		   ;

Variable : Identifier	{
			$$ = $1;
		 }
		 | NUM	{
			$$ = create_node(5, "num", NULL, NULL);
			$$->val = $1;
		 }
		 | Identifier LFKH Expression RFKH	{
			$$ = create_node(3, "[]", $1, $3);
		 }
		 | Identifier LYKH ParamList RYKH	{
			$$ = create_node(4, "()", $1, $3);
		 }
		 ;

ParamList : ParamList COMMA Expression	{
			$$ = create_node(14, ",", $1, $3);
		  }
		  | Expression	{
			$$ = create_node(14, ",", $1, NULL);
		  }
		  | {
			$$ = NULL;
		  }
		  ;

Identifier : ID {
			$$ = create_node(6, $1, NULL, NULL);
		   }
		   ;

%%

int main(int argc, char** argv)
{
	/*if(argc != 2)
	{
		printf("you shoule input one minic file\n");
		exit(1);
	}
	FILE *fp_in = fopen(argv[1], "r");
	if(!fp_in)
	{
		printf("can't open file %s\n", argv[1]);
		exit(1);
	}
	yyin = fp_in;
	//note_fp = fopen("./note_file.txt", "w");
	freopen("./out.e", "w", stdout);*/

	func_ck = (ORIGINID*)malloc(MAXFUNC * sizeof(ORIGINID));
	main_scope = new_scope(NULL);
	cur_sp = main_scope;

	yyparse();

	destroy_tree(root);
	destroy_scope(main_scope);
	return 0;
}

void yyerror(char* msg)
{
	printf("error message %s\n", msg);
}
