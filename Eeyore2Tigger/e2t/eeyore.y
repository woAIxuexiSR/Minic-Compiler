%{
#include <stdio.h>
#include "tree.h"
#include "graph.h"
#include "process.h"
#include "lex.yy.c"
#include <string>

void yyerror(string msg);

extern FILE* yyin;
FILE* note_fp;
TreeNode *root;

extern vector<TreeNode*> label_pos;

%}

%union
{
	char* name;
	int val;
	TreeNode *node;
}

%token VAR LFKH RFKH EQ IF GOTO COLON PARAM END
%token RETURN CALL

%token<name> MATHOP LOGICOP tID pID TID FUNC LABEL
%token<val> NUM

%type<node> Goal Declaration FunctionDecl RightValue StateList Statement Variable Number

%%

Root : Goal {
		root = $1;
	 }
	 ;

Goal : Declaration Goal {
		$1->nxt = $2;
		$$ = $1;
	 }
	 | FunctionDecl Goal {
		$1->nxt = $2;
		$$ = $1;
	 }
	 | FunctionDecl {
		$$ = $1;
	 }
	 ;

Declaration : VAR Number Variable {
				$$ = new TreeNode($3, $2, NULL, ARRDECL_TYPE);
			}
			| VAR Variable {
				$$ = new TreeNode($2, NULL, NULL, VARDECL_TYPE);
			}
			;

FunctionDecl : FUNC LFKH NUM RFKH StateList END FUNC {
				$$ = new TreeNode($5, NULL, NULL, FUNC_TYPE);
				$$->set_namevalue($1, $3);
			 }
			 ;

RightValue : Variable {
				$$ = $1;
		   }
		   | Number {
				$$ = $1;
		   }
		   ;

StateList : Statement StateList {
				$1->nxt = $2;
				$$ = $1;
		  }
		  | Declaration StateList {
				$1->nxt = $2;
				$$ = $1;
		  }
		  | {	$$ = NULL;	}
		  ;

Statement  : Variable EQ RightValue MATHOP RightValue {
				TreeNode *exp = new TreeNode($3, $5, NULL, MATHOP_TYPE);
				exp->set_namevalue($4, -1);
				$$ = new TreeNode($1, exp, NULL, ASSIGN_TYPE);
		   }
		   | Variable EQ RightValue LOGICOP RightValue {
				TreeNode *exp = new TreeNode($3, $5, NULL, LOGICOP_TYPE);
				exp->set_namevalue($4, -1);
				$$ = new TreeNode($1, exp, NULL, ASSIGN_TYPE);
		   }
		   | Variable EQ MATHOP RightValue {
				TreeNode *exp = new TreeNode($4, NULL, NULL, MATHOP_TYPE);
				exp->set_namevalue($3, -1);
				$$ = new TreeNode($1, exp, NULL, ASSIGN_TYPE);
		   }
		   | Variable EQ LOGICOP RightValue {
				TreeNode *exp = new TreeNode($4, NULL, NULL, LOGICOP_TYPE);
				exp->set_namevalue($3, -1);
				$$ = new TreeNode($1, exp, NULL, ASSIGN_TYPE);
		   }
		   | Variable EQ RightValue {
				$$ = new TreeNode($1, $3, NULL, ASSIGN_TYPE);
		   }
		   | Variable LFKH RightValue RFKH EQ RightValue {
				TreeNode *lf = new TreeNode($1, $3, NULL, ARRGET_TYPE);
				$$ = new TreeNode(lf, $6, NULL, ASSIGN_TYPE);
		   }
		   | Variable EQ Variable LFKH RightValue RFKH {
				TreeNode *rg = new TreeNode($3, $5, NULL, ARRGET_TYPE);
				$$ = new TreeNode($1, rg, NULL, ASSIGN_TYPE);
		   }
		   | IF RightValue LOGICOP RightValue GOTO LABEL {
				TreeNode *exp = new TreeNode($2, $4, NULL, LOGICOP_TYPE);
				exp->set_namevalue($3, -1);
				$$ = new TreeNode(exp, NULL, NULL, IF_TYPE);
				$$->set_namevalue("l", atoi($6+1));
		   }
		   | GOTO LABEL {
				$$ = new TreeNode(NULL, NULL, NULL, GOTO_TYPE);
				$$->set_namevalue("l", atoi($2+1));
		   }
		   | LABEL COLON {
				$$ = new TreeNode(NULL, NULL, NULL, LABEL_TYPE);
				$$->set_namevalue("l", atoi($1+1));
				label_pos[$$->value] = $$;
		   }
		   | PARAM RightValue {
				$$ = new TreeNode($2, NULL, NULL, PARAM_TYPE);
		   }
		   | Variable EQ CALL FUNC {
				TreeNode *callf = new TreeNode(NULL, NULL, NULL, CALL_TYPE);
				callf->set_namevalue($4, -1);
				$$ = new TreeNode($1, callf, NULL, ASSIGN_TYPE);
		   }
		   | RETURN RightValue {
				$$ = new TreeNode($2, NULL, NULL, RETURN_TYPE);
		   }
		   ;

Variable : tID {
				$$ = new TreeNode(NULL, NULL, NULL, VAR_TYPE);
				$$->set_namevalue($1, -1);
		 }
		 | pID {
				$$ = new TreeNode(NULL, NULL, NULL, VAR_TYPE);
				$$->set_namevalue($1, -1);
				
		 }
		 | TID {
				$$ = new TreeNode(NULL, NULL, NULL, VAR_TYPE);
				$$->set_namevalue($1, -1);
		 }
		 ;

Number : NUM {
				$$ = new TreeNode(NULL, NULL, NULL, NUM_TYPE);
				$$->set_namevalue("", $1);
	   }
	   ;

%%

int main(int argc, char** argv)
{
	/*if(argc != 2)
	{
		printf("you should input one eeyore file\n");
		exit(1);
	}
	FILE *fp_in = fopen(argv[1], "r");
	yyin = fp_in;*/
	//note_fp = fopen("note_file.txt", "w");
	//freopen("./out_file.txt", "w", stdout);

	yyparse();

	//root->dfs_show();
	process_tree(root);

	root->delete_node();
	delete root;
	return 0;
}

void yyerror(string msg)
{
	cout << "error message " << msg << endl;
}
