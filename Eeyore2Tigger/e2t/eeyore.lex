%{
#include "eeyore.tab.h"
#include <stdio.h>
#include <string.h>
%}

comment "//"([^\n])*
mathop	"+"|"-"|"*"|"/"|"%"
logicop "!"|"||"|"&&"|"=="|"!="|">"|"<"
number	([0-9])+
ws		([" "\t\n])+
fname	[a-zA-Z_]([a-zA-Z0-9_])*
func	f_{fname}
tid		t{number}
pid		p{number}
Tid		T{number}
label	l{number}

%%


{comment}		{ /*fprintf(note_fp, "comment ");*/ }
"var"			{ /*fprintf(note_fp, "var ");*/ return VAR; }
"["				{ /*fprintf(note_fp, "[ ");*/ return LFKH; }
"]"				{ /*fprintf(note_fp, "] ");*/ return RFKH; }
"="				{ /*fprintf(note_fp, "= ");*/ return EQ; }
"if"			{ /*fprintf(note_fp, "if ");*/ return IF; }
"goto"			{ /*fprintf(note_fp, "goto ");*/ return GOTO; }
":"				{ /*fprintf(note_fp, "colon ");*/ return COLON; }
"param"			{ /*fprintf(note_fp, "param ");*/ return PARAM; }
"return"		{ /*fprintf(note_fp, "return ");*/ return RETURN; }
"call"			{ /*fprintf(note_fp, "call ");*/ return CALL; }
"end"			{ /*fprintf(note_fp, "end ");*/ return END; }
{mathop}		{ /*fprintf(note_fp, "mathop ");*/ yylval.name = strdup(yytext); return MATHOP; }
{logicop}		{ /*fprintf(note_fp, "logicop ");*/ yylval.name = strdup(yytext); return LOGICOP; }
{tid}			{ /*fprintf(note_fp, "tid ");*/ yylval.name = strdup(yytext); return tID; }
{pid}			{ /*fprintf(note_fp, "pid ");*/ yylval.name = strdup(yytext); return pID; }
{Tid}			{ /*fprintf(note_fp, "Tid ");*/ yylval.name = strdup(yytext); return TID; }
{label}			{ /*fprintf(note_fp, "label ");*/ yylval.name = strdup(yytext); return LABEL; }
{number}		{ /*fprintf(note_fp, "num ");*/ yylval.val = atoi(yytext); return NUM; }
{func}			{ /*fprintf(note_fp, "func ");*/ yylval.name = strdup(yytext); return FUNC; }
{ws}			{ /*if(strstr(yytext, "\n") != NULL) fprintf(note_fp, "\n");*/ }
.				{ /*fprintf(note_fp, "error");*/ exit(1); }

%%

int yywrap()
{
	return 1;
}
