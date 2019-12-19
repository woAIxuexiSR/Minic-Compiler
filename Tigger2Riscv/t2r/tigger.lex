%{
#include "tigger.tab.h"
#include <stdio.h>
#include <string.h>
#include <string>

%}

comment	"//"([^\n])*
sreg	"s0"|"s1"|"s2"|"s3"|"s4"|"s5"|"s6"|"s7"|"s8"|"s9"|"s10"|"s11"
areg	"a0"|"a1"|"a2"|"a3"|"a4"|"a5"|"a6"|"a7"
treg	"t0"|"t1"|"t2"|"t3"|"t4"|"t5"|"t6"
reg		"x0"|{sreg}|{areg}|{treg}
num		([0-9])+
ws		([" "\n\t"])+
func	f_[a-zA-Z_]([a-zA-Z0-9_])*
label	l{num}
op		"+"|"-"|"*"|"/"|"%"|"!"|"||"|"&&"|"=="|"!="|">"|"<"
gvar	v{num}

%%

{comment}		{ /*fprintf(note_fp, "comment ");*/ }
"malloc"		{ /*fprintf(note_fp, "malloc ");*/ return MALLOC; }
"end"			{ /*fprintf(note_fp, "end ");*/ return END; }
"="				{ /*fprintf(note_fp, "= ");*/ return ASSIGN; }
"["				{ /*fprintf(note_fp, "[ ");*/ return LFKH; }
"]"				{ /*fprintf(note_fp, "] ");*/ return RFKH; }
"if"			{ /*fprintf(note_fp, "if ");*/ return IF; }
"goto"			{ /*fprintf(note_fp, "goto ");*/ return GOTO; }
"call"			{ /*fprintf(note_fp, "call ");*/ return CALL; }
"store"			{ /*fprintf(note_fp, "store ");*/ return STORE; }
"load"			{ /*fprintf(note_fp, "load ");*/ return LOAD; }
"loadaddr"		{ /*fprintf(note_fp, "loadaddr ");*/ return LOADADDR; }
"return"		{ /*fprintf(note_fp, "return ");*/ return RETURN; }
":"				{ /*fprintf(note_fp, ": ");*/ return COLON; }
{num}			{ /*fprintf(note_fp, "num ");*/ yylval.val = atoi(yytext); return NUM; }
{reg}			{ /*fprintf(note_fp, "reg ");*/ yylval.name = strdup(yytext); return REG; }
{func}			{ /*fprintf(note_fp, "func ");*/ yylval.name = strdup(yytext); return FUNC; }
{label}			{ /*fprintf(note_fp, "label ");*/ yylval.name = strdup(yytext); return LABEL; }
{op}			{ /*fprintf(note_fp, "op ");*/ yylval.name = strdup(yytext); return OP; }
{gvar}			{ /*fprintf(note_fp, "gvar ");*/ yylval.name = strdup(yytext); return GVAR; }
{ws}			{ /*if(strstr(yytext, "\n") != 0) fprintf(note_fp, "\n");*/ }
.				{ exit(1); }

%%

int yywrap()
{
	return 1;
}
