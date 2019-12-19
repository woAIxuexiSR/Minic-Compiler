#include "symbol.h"

int var_num = 0;		// user defined variable numbers
int param_num = 0;		// function paramter numbers
extern ORIGINID* func_ck;
extern int func_num;

void init_symtab(Symtab* p)
{
	p->maxsize = INITSIZE;
	p->size = 0;
	p->ids = (ORIGINID*)malloc(p->maxsize * sizeof(ORIGINID));
	//printf("init called %d %d\n", p->maxsize, INITSIZE);
}

void insert_symtab(Symtab* p, char* name, int param)
{
	for(int i = 0; i < p->size; ++i)
	{
		if(strcmp(p->ids[i].name, name) == 0)
		{
			printf("\nerror! variable %s rename\n", name);
			exit(1);
		}
	}

	//printf("%d %d %s\n", p->size, p->maxsize, name);
	if(p->size == p->maxsize)
	{
		p->maxsize += INITSIZE;
		ORIGINID* tmp;
		tmp = (ORIGINID*)malloc(p->maxsize * sizeof(ORIGINID));
		memcpy(tmp, p->ids, p->size * sizeof(ORIGINID));
		free(p->ids);
		p->ids = tmp;
		//printf("size expands\n");
	}
	p->ids[p->size].name = strdup(name);
	p->ids[p->size].param = param;
	if(param == 3)
		p->ids[p->size].index = param_num++;
	else
		p->ids[p->size].index = var_num++;
	//printf("%s\n", p->ids[p->size]);
	p->size++;
}

void destroy_symtab(Symtab* p)
{
	for(int i = 0; i < p->size; ++i)
		free(p->ids[i].name);
	free(p->ids);
}

void show_symtab(FILE* fp, Symtab* p)
{
	for(int i = 0; i < p->size; ++i)
	{
		fprintf(fp, "%s ", p->ids[i].name);
		if(p->ids[i].param == 3)
			fprintf(fp, "p ");
		else if(p->ids[i].param == 1)
			fprintf(fp, "T ");
		fprintf(fp, "%d\n", p->ids[i].index);
	}
}

Scope* new_scope(Scope* prev)
{
	Scope* p = (Scope*)malloc(sizeof(Scope));
	p->nxt = NULL;
	p->pre = prev;
	p->st = (Symtab*)malloc(sizeof(Symtab));
	init_symtab(p->st);
	//printf("%d %d\n", p->st->size, p->st->maxsize);
	return p;
}

void destroy_scope(Scope* p)
{
	destroy_symtab(p->st);
	free(p->st);
	free(p);
}

Var find_symbol(Scope* p, char* name)
{
	Var ans; ans.type = 0; ans.index = -1; ans.flag = 0;

	for(int i = 0; i < p->st->size; ++i)
	{
		if(strcmp(p->st->ids[i].name, name) == 0)
		{
			ans.type = p->st->ids[i].param;
			ans.index = p->st->ids[i].index;
			return ans;
		}
	}
	if(p->pre)
		return find_symbol(p->pre, name);

	printf("\nerror! no vaiable named %s\n", name);
	exit(1);
}

void check_funcd(char* name, int num, int def)
{
	for(int i = 0; i < func_num; ++i)
	{
		if(strcmp(func_ck[i].name, name) == 0)
		{
			if(def == 0 && func_ck[i].index == 1)
			{
				printf("\nerror! can't declare function %s after defined it\n", name);
				exit(1);
			}
			else if(def == func_ck[i].index)
			{
				if(def == 1)
					printf("\nerror! function %s is already defined\n", name);
				else
					printf("\nerror! function %s is already declared\n", name);
				exit(1);
			}
			else
			{
				if(num != func_ck[i].param)
				{
					printf("\nerror! function %s's paramters is not equal to its declarition\n", name);
					exit(1);
				}
				func_ck[i].index = 1;
				return;
			}
		}
	}
	func_ck[func_num].name = strdup(name);
	func_ck[func_num].param = num;
	func_ck[func_num++].index = def;
}
