#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

/* type : 1 op 2 assign 3 [] 4 () 5 num
		  6 id 7 if 8 while 9 return 
		  10 int 11 []= 12 func 13 else
		  14 ,			
 */

typedef struct TreeNode
{
	int type;
	char* name;
	struct TreeNode* lchild;
	struct TreeNode* rchild;
	int val;
}TreeNode;

typedef struct Var
{
	int type;		//1 T, 2 t, 3 p, 4 num
	int index;
	int flag;
	int type2;
	int index2;
}Var;

TreeNode* create_node(int type, char* name, TreeNode* lchild, TreeNode* rchild);

void destroy_tree(TreeNode* p);

Var translate_tree(TreeNode* p);

int trans_ifwhile(TreeNode* p);
int gettag();

void print_var(Var t);

void check_funcu(char *name, int num);

#endif
