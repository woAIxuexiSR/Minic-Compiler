#include "process.h"

#define MAXLABEL 100

extern FILE* note_fp;

extern vector<VarStore> var_store;

vector<TreeNode*> label_pos(MAXLABEL);
map<string, int> global_var;
map<string, int> var_index;
map<string, int> is_arr;
int var_num, global_num, param_num;
vector<bool> first_use;	// global + paramter

int reg, mem;
int param_index;
int I, II, III, IV, V;
bool call_flag;

void process_tree(TreeNode *root)
{
	global_var.clear();
	global_num = 0;
	is_arr.clear();
	
	TreeNode *node = root;
	while(node != NULL)
	{
		if(node->type == FUNC_TYPE)
		{
			var_index.clear();
			var_num = 0;
			param_num = node->value;
			param_index = 0;
			call_flag = 0;
			V = 0;

			func_varmap(node->lchild);
	
			/***************    show var_index
			map<string, int>::iterator it;
			for(it = var_index.begin(); it != var_index.end(); it++)
				fprintf(note_fp, "%s %d\n", it->first.c_str(), it->second);
			fprintf(note_fp, "%d\n\n", var_num);
			******************/

			init_usedef(node->lchild);

			cal_inout(node->lchild);

			/**************     show use def
			TreeNode *tmp = node->lchild;
			int ccnt = 0;
			while(tmp != NULL)
			{
				fprintf(note_fp, "%d : ", ccnt++);
				fprintf(note_fp, "%d %d\n", (int)tmp->in[30], (int)tmp->in[63]);
				tmp = tmp->nxt;
			}
			**************/

			var_store.resize(var_num);
			for(int i = 0;i < var_num; ++i)
				var_store[i].type = -1;

			ConflictGraph cfgraph(var_num);
			dfs_addedge(node->lchild, cfgraph);
			cfgraph.allocate_reg();

			/******************   show reg allocation
			for(int i = 0; i < var_num; ++i)
			{
				fprintf(note_fp, "%d %d %d\n", i, var_store[i].type, var_store[i].index);
			}
			fprintf(note_fp, "\n");
			******************/

			first_use.resize(global_num + param_num);
			for(int i = 0; i < global_num + param_num; ++i)
				first_use[i] = 0;
			
			I = mem;
			II = I + (reg > 7 ? reg - 7 : 0);
			III = II + param_num;
			IV = III + (reg > 7 ? 7 : reg);
			if(!call_flag) IV = III;
			V += IV;
			//fprintf(note_fp, "\n%d %d\n", mem, reg);
			//fprintf(note_fp, "%d %d %d %d\n\n", I, II, III, IV);

			translate(node);
		}
		else
		{
			if(node->type == ARRDECL_TYPE)
			{
				printf("v%d = malloc %d\n", global_num, node->rchild->value);
				is_arr[node->lchild->name] = -1;
			}
			else
				printf("v%d = 0\n", global_num);
			global_var[node->lchild->name] = global_num++;
		}

		node = node->nxt;
	}

	/***************** show global
	map<string, int>::iterator it;
	for(it = global_var.begin(); it != global_var.end(); it++)
		cout << it->first << " " << it->second << endl;
	cout << global_num << endl;
	***************/
}

void func_varmap(TreeNode *node)
{
	if(!node) return;

	if(node->type == VAR_TYPE)
	{
		map<string, int>::iterator it;
		it = var_index.find(node->name);
		if(it == var_index.end())
			var_index[node->name] = var_num++;
	}
	else if(node->type == CALL_TYPE)
		call_flag = 1;
	else if(node->type == ARRDECL_TYPE)
	{
		is_arr[node->lchild->name] = V;
		V += node->rchild->value / 4;
	}
	func_varmap(node->lchild);
	func_varmap(node->rchild);
	func_varmap(node->nxt);
}

void init_usedef(TreeNode *node)
{
	if(!node) return;

	node->use.resize(var_num);
	node->def.resize(var_num);
	node->in.resize(var_num);
	node->out.resize(var_num);

	switch(node->type)
	{
	case ARRDECL_TYPE: {
							node->set_def(node->lchild);
							break;
					   };
	case VARDECL_TYPE: {
							node->set_def(node->lchild);
							break;
					   };
	case ASSIGN_TYPE: {
							if(node->lchild->type == ARRGET_TYPE)
							{
								node->set_use(node->lchild->lchild);
								node->set_use(node->lchild->rchild);
							}
							else
								node->set_def(node->lchild);

							switch(node->rchild->type)
							{
							case MATHOP_TYPE:
							case LOGICOP_TYPE: {
													node->set_use(node->rchild->lchild);
													if(node->rchild->rchild)
														node->set_use(node->rchild->rchild);
													break;
											   }
							case NUM_TYPE: break;
							case VAR_TYPE: {
													node->set_use(node->rchild);
													break;
										   }
							case ARRGET_TYPE: {
													node->set_use(node->rchild->lchild);
													node->set_use(node->rchild->rchild);
													break;
											  }
							case CALL_TYPE: break;
							}

							break;
					  }
	case IF_TYPE: {
						node->set_use(node->lchild->lchild);
						node->set_use(node->lchild->rchild);
						break;
				  }
	case GOTO_TYPE: break;
	case LABEL_TYPE: break;
	case PARAM_TYPE: {
						node->set_use(node->lchild);
						break;
					 }
	case RETURN_TYPE: {
							node->set_use(node->lchild);
					  }
	}

	init_usedef(node->nxt);
}

void cal_inout(TreeNode *node)
{
	while(1)
	{
		bool flag = 0;
		TreeNode *p = node;
		while(p != NULL)
		{
			TreeNode *n = NULL;
			if(p->type == GOTO_TYPE || p->type == IF_TYPE)
				n = label_pos[p->value];
			flag |= p->set_out(n);
			flag |= p->set_in();
			p = p->nxt;
		}

		if(!flag) break;
	}
}

void dfs_addedge(TreeNode *node, ConflictGraph &cfg)
{
	if(!node) return;
	for(int i = 0; i < var_num; ++i)
		if(node->in[i])
		{
			for(int j = i + 1; j < var_num; ++j)
				if(node->in[j])
					cfg.add_edge(i, j);
		}
	dfs_addedge(node->nxt, cfg);
}

/*****************************
		var memory area
  ***************************
		s reg area
  ***************************
		param save area
  ***************************
		t reg area
  ***************************
		local arr area
  ***************************/

void translate(TreeNode *node)
{
	if(!node) return;

	switch(node->type)
	{
	case FUNC_TYPE: {
						printf("%s [%d] [%d]\n", node->name.c_str(), param_num, V);
						for(int i = I; i < II; ++i)
							printf("store s%d %d\n", i - I, i);
						for(int i = II; i < III; ++i)
							printf("store a%d %d\n", i - II, i);
						translate(node->lchild);
						printf("end %s\n", node->name.c_str());
						return;
					}
	case ARRDECL_TYPE: {
							break;
					   }
	case VARDECL_TYPE: {
						//if(node->lchild->name == "t4") exit(1);
						break;
					   }
	case ASSIGN_TYPE: {
						map<string, int>::iterator it;
						bool gflag = 0;
						string left;
						if(node->lchild->type != ARRGET_TYPE)
						{
							it = global_var.find(node->lchild->name);
							if(it != global_var.end())
							{
								gflag = 1;
								left = "s9 = ";
							}
							else
							{
								it = var_index.find(node->lchild->name);
								if(var_store[it->second].type == 1 && node->out[it->second])
									left = show_reg(var_store[it->second].index) + " = ";
								else
									left = "s9 = ";
							}
						}
						else 
						{
							int x = deal_rvalue(node->lchild->lchild, 0);
							int y = deal_rvalue(node->lchild->rchild, 1);
							printf("s9 = %s + %s\n", show_reg(x).c_str(), show_reg(y).c_str());
							left = "s9[0] = ";
						}

						switch(node->rchild->type)
						{
						case MATHOP_TYPE:
						case LOGICOP_TYPE:{
											if(node->rchild->rchild)
											{
												int x = deal_rvalue(node->rchild->lchild, 0);
												int y = deal_rvalue(node->rchild->rchild, 1);
												printf("%s", left.c_str());
												printf("%s", show_reg(x).c_str());
												printf(" %s ", node->rchild->name.c_str());
												printf("%s\n", show_reg(y).c_str());
											}
											else
											{
												int x = deal_rvalue(node->rchild->lchild, 0);
												printf("%s", left.c_str());
												printf("%s ", node->rchild->name.c_str());
												printf("%s\n", show_reg(x).c_str());
											}
											break;
										  }
						case VAR_TYPE:
						case NUM_TYPE: {
											int x = deal_rvalue(node->rchild, 0);
											printf("%s%s\n", left.c_str(), show_reg(x).c_str());
											break;
									   }
						case ARRGET_TYPE:{
											int x = deal_rvalue(node->rchild->lchild, 0);
											int y = deal_rvalue(node->rchild->rchild, 1);
											printf("s10 = %s + %s\n", show_reg(x).c_str(), show_reg(y).c_str());
											printf("%ss10[0]\n", left.c_str());
											break;
										 }
						case CALL_TYPE: {
											bool f = (node->rchild->name == "f_getint") || (node->rchild->name == "f_getchar") ||
													(node->rchild->name == "f_putint") || (node->rchild->name == "f_putchar");
											if(!f)
												for(int i = III; i < IV; ++i)
													printf("store t%d %d\n", i - III, i);
											printf("call %s\n", node->rchild->name.c_str());
											if(!f)
												for(int i = III; i < IV; ++i)
													printf("load %d t%d\n", i, i - III);
											printf("%sa0\n", left.c_str());
											param_index = 0;
											break;
										}
						}
						
						if(gflag)
						{
							printf("loadaddr v%d s10\n", it->second);
							printf("s10[0] = s9\n");

							it = var_index.find(node->lchild->name);
							if(var_store[it->second].type == 1 && node->out[it->second])
								printf("%s = s9\n", show_reg(var_store[it->second].index).c_str());
						}
						else if(node->lchild->type != ARRGET_TYPE)
						{
							if(!node->out[it->second]) break;
							if(var_store[it->second].type != 1)
								printf("store s9 %d\n", var_store[it->second].index);
							else if(it->first[0] == 'p')
								printf("store %s %d\n", show_reg(var_store[it->second].index).c_str(), II + atoi(it->first.substr(1).c_str()));
						}
							  
						break;
					  }
	case IF_TYPE: {
						int x = deal_rvalue(node->lchild->lchild, 0);	
						int y = deal_rvalue(node->lchild->rchild, 1);
						printf("if %s ", show_reg(x).c_str());
						printf("%s ", node->lchild->name.c_str());
						printf("%s ", show_reg(y).c_str());
						printf("goto %s%d\n", node->name.c_str(), node->value);
						break;
				  }
	case GOTO_TYPE: {
						printf("goto %s%d\n", node->name.c_str(), node->value);
						break;
					}
	case LABEL_TYPE: {
						printf("%s%d:\n", node->name.c_str(), node->value);
						break;
					 }
	case PARAM_TYPE: {
						int x = deal_rvalue(node->lchild, 0);
						printf("%s = %s\n", show_reg(19 + param_index).c_str(), show_reg(x).c_str());
						param_index++;
						break;
					 }
	case RETURN_TYPE: {
						int x = deal_rvalue(node->lchild, 0);
						printf("a0 = %s\n", show_reg(x).c_str());
						for(int i = I; i < II; ++i)
							printf("load %d s%d\n", i, i - I);
						printf("return\n");
						
						for(int i = 0; i < global_num + param_num; ++i)
							first_use[i] = 0;
						
						break;
					  }
	}
	
	translate(node->nxt);
}

int deal_rvalue(TreeNode *node, int h)		// h = 1 : s11, h = 0 : s10
{
	if(node->type == NUM_TYPE)
	{
		printf("s%d = %d\n", h + 10, node->value);
		return h + 17;
	}	

	map<string, int>::iterator it1, it2;
	it1 = global_var.find(node->name);
	it2 = var_index.find(node->name);

	map<string, int>::iterator it;
	it = is_arr.find(node->name);
	if(it != is_arr.end())
	{
		if(it1 != global_var.end())
			printf("loadaddr v%d s%d\n", it1->second, h + 10);
		else
			printf("loadaddr %d s%d\n", IV + it->second, h + 10);
		return h + 17;
	}

	if(it1 != global_var.end())
	{
		if(var_store[it2->second].type == 1)
		{
			if(!first_use[it1->second])
			{
				first_use[it1->second] = 1;
				printf("load v%d ", it1->second);
				printf("%s", show_reg(var_store[it2->second].index).c_str());
				printf("\n");
			}
			return var_store[it2->second].index;
		}
		else
		{
			printf("load v%d s%d\n", it1->second, h + 10);
			return h + 17;
		}
	}

	if(node->name[0] == 'p')
	{
		int t = atoi(node->name.substr(1).c_str());
		if(!first_use[global_num + t])
		{
			first_use[global_num + t] = 1;
			printf("load %d ", II + t);
			if(var_store[it2->second].type == 1)
			{
				printf("%s\n", show_reg(var_store[it2->second].index).c_str());
				return var_store[it2->second].index;
			}
			else
			{
				printf("s%d\n", h + 10);
				return h + 17;
			}
		}
	}

	if(var_store[it2->second].type == 1)
		return var_store[it2->second].index;
	else
	{
		printf("load %d s%d\n", var_store[it2->second].index, h + 10);
		return h + 17;
	}
}

string show_reg(int index)
{
	if(index <= 6)
		return "t" + to_string(index);
	else if(index <= 18)
		return "s" + to_string(index - 7);
	else if(index <= 26)
		return "a" + to_string(index - 19);
}
