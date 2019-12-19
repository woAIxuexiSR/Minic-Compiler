#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

#define INITSIZE 20
#define MAXSTACK 100
#define MAXFUNC 100

struct Var;
typedef struct Var Var;

typedef struct ORIGINID
{
	char* name;
	int index;
	int param; // 3 param, 1 not param
}ORIGINID;

typedef struct Symtab
{
	ORIGINID* ids;
	int size, maxsize;
}Symtab;

typedef struct Scope
{
	Symtab* st;
	struct Scope* pre;
	struct Scope* nxt;
}Scope;

void init_symtab(Symtab* p);
void insert_symtab(Symtab* p, char* name, int param);
void destroy_symtab(Symtab* p);
void show_symtab(FILE* fp, Symtab* p);

Scope* new_scope(Scope* pre);
void destroy_scope(Scope* p);

Var find_symbol(Scope* p, char* name);

void check_funcd(char* name, int num, int def);
#endif
