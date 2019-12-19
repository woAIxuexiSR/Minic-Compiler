#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "tree.h"
#include "graph.h"

void process_tree(TreeNode *root);
void func_varmap(TreeNode *node);
void init_usedef(TreeNode *node);
void cal_inout(TreeNode *node);
void dfs_addedge(TreeNode *node, ConflictGraph &cfg);
void translate(TreeNode *node);
int deal_rvalue(TreeNode *node, int h);
string show_reg(int index);

#endif
