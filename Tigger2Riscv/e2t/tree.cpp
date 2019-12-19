#include "tree.h"

extern map<string, int> var_index;
extern map<string, int> global_var;

TreeNode::TreeNode()
{
	type = NO_TYPE;
	name = "";
	value = -1;
	lchild = rchild = nxt = NULL;
	use.clear(); def.clear();
	in.clear(); out.clear();
}

TreeNode::TreeNode(TreeNode *lc, TreeNode *rc, TreeNode *n, TreeType t)
{
	type = t;
	name = "";
	value = -1;
	lchild = lc; rchild = rc; nxt = n;
	use.clear(); def.clear();
	in.clear(); out.clear();
}

void TreeNode::set_namevalue(string n, int v)
{
	name = n;
	value = v;
}

void TreeNode::dfs_show()
{
	switch(type)
	{
	case NO_TYPE: printf("NO_TYPE "); break;
	case FUNC_TYPE: printf("FUNC_TYPE "); break;
	case VARDECL_TYPE: printf("VARDECL_TYPE "); break;
	case ARRDECL_TYPE: printf("ARRDECL_TYPE "); break;
	case STATEMENT_TYPE: printf("STATEMENT_TYPE "); break;
	case ASSIGN_TYPE: printf("ASSIGN_TYPE "); break;
	case MATHOP_TYPE: printf("MATHOP_TYPE "); break;
	case LOGICOP_TYPE: printf("LOGICOP_TYPE "); break;
	case ARRGET_TYPE: printf("ARRGET_TYPE "); break;
	case IF_TYPE: printf("IF_TYPE "); break;
	case GOTO_TYPE: printf("GOTO_TYPE "); break;
	case LABEL_TYPE: printf("LABEL_TYPE "); break;
	case PARAM_TYPE: printf("PARAM_TYPE "); break;
	case CALL_TYPE: printf("CALL_TYPE "); break;
	case RETURN_TYPE: printf("RETURN_TYPE "); break;
	case VAR_TYPE: printf("VAR_TYPE "); break;
	case NUM_TYPE: printf("NUM_TYPE "); break;
	}
	
	//if(lchild) lchild->dfs_show();
	//if(rchild) rchild->dfs_show();
	printf("\n");
	if(nxt) nxt->dfs_show();
}

void TreeNode::delete_node()
{
	if(lchild)
	{
		lchild->delete_node();
		delete lchild;
	}
	if(rchild)
	{
		rchild->delete_node();
		delete rchild;
	}
	if(nxt)
	{
		nxt->delete_node();
		delete nxt;
	}
}

bool TreeNode::set_use(TreeNode *node)
{
	if(node->type != VAR_TYPE) return false;
	
	use[var_index[node->name]] = 1;
	return true;
}

bool TreeNode::set_def(TreeNode *node)
{
	if(node->type != VAR_TYPE) return false;
	
	def[var_index[node->name]] = 1;
	return true;
}

bool TreeNode::set_in()
{
	bool ans = 0;
	for(int i = 0; i < in.size(); ++i)
	{
		bool t = use[i] ? 1 : (def[i] ? 0 : out[i]);
		if(in[i] != t)
		{
			ans = 1;
			in[i] = t;
		}
	}
	return ans;
}

bool TreeNode::set_out(TreeNode *node)
{
	bool ans = 0;
	if(type == RETURN_TYPE)
	{
		for(int i = 0; i < out.size(); ++i)
			out[i] = 0;
		return ans;
	}
	for(int i = 0; i < out.size(); ++i)
	{
		bool t = nxt ? nxt->in[i] : 0;
		t |= (node ? node->in[i] : 0);
		if(out[i] != t)
		{
			ans = 1;
			out[i] = t;
		}
	}
	return ans;
}
