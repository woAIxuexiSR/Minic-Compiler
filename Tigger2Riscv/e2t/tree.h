#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

enum TreeType
{
	NO_TYPE,				//default
	FUNC_TYPE,				//func
	VARDECL_TYPE,			//var declaration
	ARRDECL_TYPE,			//array declaration
	STATEMENT_TYPE,			//statement
	ASSIGN_TYPE,			//assign
	MATHOP_TYPE,			//mathop
	LOGICOP_TYPE,			//logicop
	ARRGET_TYPE,			//[]
	IF_TYPE,				//if statement
	GOTO_TYPE,				//goto statement
	LABEL_TYPE,				//label declaration
	PARAM_TYPE,				//param declaration
	CALL_TYPE,				//call func
	RETURN_TYPE,			//return
	VAR_TYPE,				//variable
	NUM_TYPE				//number
};

class TreeNode
{
public:
	TreeType type;
	string name;
	int value;
	TreeNode *lchild;
	TreeNode *rchild;
	TreeNode *nxt;

	vector<bool> use;
	vector<bool> def;    //resize
	vector<bool> in;
	vector<bool> out;

	TreeNode();
	TreeNode(TreeNode *lc, TreeNode *rc, TreeNode *n, TreeType t);
	void set_namevalue(string n, int v);
	
	void dfs_show();
	void delete_node();
	bool set_use(TreeNode *node);
	bool set_def(TreeNode *node);
	bool set_in();						// in = (out - def)	+ use
	bool set_out(TreeNode *node);		// out = + nxt_in    node => jump pos
};


#endif
