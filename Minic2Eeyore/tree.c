#include "tree.h"

int label = 0;		  // jump labell numbers
int tmp_id = 0;			// temparory variable numbers
extern Scope *cur_sp;
Var params[100];
int param_top = 0;
extern ORIGINID* func_ck;
extern int func_num;
extern int rflag;

TreeNode* create_node(int type, char* name, TreeNode* lchild, TreeNode* rchild)
{
	TreeNode* ans = (TreeNode*)malloc(sizeof(TreeNode));
	ans->type = type;
	ans->name = strdup(name);
	ans->lchild = lchild;
	ans->rchild = rchild;
	return ans;
}

void destroy_tree(TreeNode* p)
{
	if(p == NULL) return;
	destroy_tree(p->lchild);
	destroy_tree(p->rchild);
	free(p);
}

Var translate_tree(TreeNode* p)
{	
	Var zero; zero.type = zero.index = zero.flag = 0;
	if(!p) return zero;

	switch(p->type)
	{
	case 1:{
				Var ans; ans.type = 2; ans.index = tmp_id++; ans.flag = 0;

				Var tmp1 = translate_tree(p->lchild);
				if(p->rchild)
				{
					Var tmp2 = translate_tree(p->rchild);
					printf("var "); print_var(ans); printf("\n");
					print_var(ans); printf(" = ");
					print_var(tmp1); printf(" %s ", p->name); print_var(tmp2);
				}
				else
				{
					if(tmp1.type == 4)
					{
						Var tmp2; tmp2.type = 2; tmp2.index = tmp_id++; tmp2.flag = 0;
						printf("var "); print_var(tmp2); printf("\n");
						print_var(tmp2); printf(" = %d\n", tmp1.index);
						tmp1 = tmp2;
					}
					printf("var "); print_var(ans); printf("\n");
					print_var(ans); printf(" = ");
					printf("%s", p->name); print_var(tmp1);
				}
				printf("\n");
				return ans;
		   }
	case 2:{
				Var tmp1 = translate_tree(p->lchild);
				Var tmp2 = translate_tree(p->rchild);
				print_var(tmp1); printf(" = "); print_var(tmp2);
				printf("\n");
				return tmp1;
		   }
	case 3:{
				Var ans; ans.type = 2; ans.index = tmp_id++; ans.flag = 0;
				
				Var tmp1 = translate_tree(p->lchild);
				Var tmp2 = translate_tree(p->rchild);

				Var tmp3; tmp3.type = 2; tmp3.index = tmp_id++; tmp3.flag = 0;
				printf("var "); print_var(tmp3); printf("\n");
				print_var(tmp3); printf(" = "); print_var(tmp2); printf(" * 4\n");

				printf("var "); print_var(ans); printf("\n");
				print_var(ans); printf(" = "); print_var(tmp1);
				printf("["); print_var(tmp3); printf("]\n");
				return ans;
		   }
	case 4:{
				Var ans; ans.type = 2; ans.index = tmp_id++; ans.flag = 0;
				printf("var "); print_var(ans); printf("\n");

				param_top = 0;
				if(p->rchild)
				{
					translate_tree(p->rchild);
					for(int i = 0; i < param_top; ++i)
					{
						printf("param ");
						print_var(params[i]);
						printf("\n");
					}
				}

				check_funcu(p->lchild->name, param_top);
				print_var(ans); printf(" = ");
				printf("call f_%s\n", p->lchild->name);
				return ans;
		   }
	case 5:{
				Var ans; ans.type = 4; ans.index = p->val; ans.flag = 0;
				return ans;
		   }
	case 6:{
				Var ans = find_symbol(cur_sp, p->name);
				return ans;
		   }
/*	case 7:{
				Var tmp1 = translate_tree(p->lchild);
				printf("if "); print_var(tmp1);
				int now_label = label++;
				printf(" == 0 goto l%d\n", now_label);

				if(p->rchild && p->rchild->type == 13)
				{
					translate_tree(p->rchild->lchild);
					int tmp_label = label++;
					printf("goto l%d\n", tmp_label);
					printf("l%d:\n", now_label);
					translate_tree(p->rchild->rchild);
					printf("l%d:\n", tmp_label);
				}
				else
				{
					translate_tree(p->rchild);
					printf("l%d:\n", now_label);
				}
				return zero;
		   }
	case 8:{
				Var tmp1 = translate_tree(p->lchild);
				int now_label1 = label++;
				int now_label2 = label++;
				printf("l%d:\n", now_label1);
				printf("if "); print_var(tmp1);
				printf(" == 0 goto l%d\n", now_label2);
				translate_tree(p->rchild);
				printf("goto l%d\n", now_label1);
				printf("l%d\n", now_label2);
		   }
 */

	case 9:{
				Var tmp = translate_tree(p->lchild);
				printf("return "); print_var(tmp); printf("\n");
				rflag = 1;
				return tmp;
		   }
	case 10:{
				if(p->lchild->type == 3)
				{
					printf("var %d ", p->lchild->rchild->val * 4);
					Var tmp = translate_tree(p->lchild->lchild);
					print_var(tmp);
					printf("\n");
				}
				else
				{
					Var tmp = translate_tree(p->lchild);
					printf("var "); print_var(tmp); printf("\n");
				}
				return zero;
			}
	case 11:{
				Var ans; ans.flag = 1;
				
				Var tmp1 = translate_tree(p->lchild);
				Var tmp2 = translate_tree(p->rchild);

				Var tmp3; tmp3.type = 2; tmp3.index = tmp_id++; tmp3.flag = 0;
				printf("var "); print_var(tmp3); printf("\n");
				print_var(tmp3); printf(" = "); print_var(tmp2); printf(" * 4\n");

				ans.type = tmp1.type; ans.index = tmp1.index;
				ans.type2 = tmp3.type; ans.index2 = tmp3.index;
				return ans;
			}
	case 13:{
				return zero;
			}
	case 14:{
				if(p->lchild && p->lchild->type != 14)
				{
					Var tmp1 = translate_tree(p->lchild);
					if(tmp1.type == 4)
					{
						Var ans; ans.type = 2; ans.index = tmp_id++; ans.flag = 0;
						printf("var "); print_var(ans); printf("\n");
						print_var(ans); printf(" = %d\n", tmp1.index);
						params[param_top++] = ans;
					}
					else 
						params[param_top++] = tmp1;
				}
				else
					translate_tree(p->lchild);

				Var tmp2 = translate_tree(p->rchild);
				if(tmp2.type != 0)
				{
					if(tmp2.type == 4)
					{
						Var ans; ans.type = 2; ans.index = tmp_id++; ans.flag = 0;
						printf("var "); print_var(ans); printf("\n");
						print_var(ans); printf(" = %d\n", tmp2.index);
						params[param_top++] = ans;
					}
					else
						params[param_top++] = tmp2;
				}
				return zero;
			}
	}
	return zero;
}

int trans_ifwhile(TreeNode* p)
{
	switch(p->type)
	{
		case 7:{
					Var tmp = translate_tree(p->lchild);
					printf("if "); print_var(tmp);
					int now_label = label++;
					printf(" == 0 goto l%d\n", now_label);
					return now_label;
			   }
		case 8:{
					int now_label1 = label++;
					printf("l%d:\n", now_label1);
					Var tmp = translate_tree(p->lchild);
					int now_label2 = label++;
					printf("if "); print_var(tmp);
					printf(" == 0 goto l%d\n", now_label2);
					return now_label1;
			   }
	}
	return -1;
}

int gettag()
{
	return label++;
}

void print_var(Var t)
{
	if(t.flag)
	{	
		switch(t.type)
		{
			case 1: printf("T%d", t.index); break;
			case 2: printf("t%d", t.index); break;
			case 3: printf("p%d", t.index); break;
			case 4: printf("%d", t.index); break;
		}
		printf("[");

		switch(t.type2)
		{
			case 1: printf("T%d", t.index2); break;
			case 2: printf("t%d", t.index2); break;
			case 3: printf("p%d", t.index2); break;
			case 4: printf("%d", t.index2); break;
		}
		printf("]");
		return;
	}
	switch(t.type)
	{
		case 1: printf("T%d", t.index); break;
		case 2: printf("t%d", t.index); break;
		case 3: printf("p%d", t.index); break;
		case 4: printf("%d", t.index); break;
	}
}

void check_funcu(char *name, int num)
{
	for(int i = 0; i < func_num; ++i)
	{
		if(strcmp(func_ck[i].name, name) == 0)
		{
			if(func_ck[i].param != num)
			{
				printf("\nerror! function %s needs correct paramter numbers", name);
				exit(1);
			}
			else if(func_ck[i].index != 1)
			{
				if(strcmp(name, "getint") == 0 || strcmp(name, "getchar") == 0 || strcmp(name, "putint") == 0 || strcmp(name, "putchar") == 0)
					return;
				printf("\nerror! function %s is not defined\n", name);
				exit(1);
			}
			return;
		}
	}
	printf("\nerror! function %s is not defined\n", name);
	exit(1);
}
