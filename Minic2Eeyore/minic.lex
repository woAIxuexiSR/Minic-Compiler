%{
#include "minic.tab.h"
#include <stdio.h>
#include <string.h>
extern FILE* note_fp;
%}
comment "//"([^\n])*
numbers ([0-9])+
addsub "+"|"-"
muldivmod "*"|"/"|"%"
equn "=="|"!="
and "&&"
or "||"
cmp "<"|">"
not "!"
assign "="

lykh "("
rykh ")"
lfkh "["
rfkh "]"
ldkh "{"
rdkh "}"

ws ([" "\t\n])+
semi ";"
comma ","
id [a-zA-Z_]([a-zA-Z0-9_])*

%%

{comment}	{ /*fprintf(note_fp, "comment ");*/ }
"int"		{ /*fprintf(note_fp, "int ");*/ yylval.name = strdup(yytext); return INT; }
"return"	{ /*fprintf(note_fp, "return ");*/ yylval.name = strdup(yytext); return RETURN; }
"if"		{ /*fprintf(note_fp, "if ");*/ yylval.name = strdup(yytext); return IF; }
"else"		{ /*fprintf(note_fp, "else ");*/ yylval.name = strdup(yytext); return ELSE; }
"while"		{ /*fprintf(note_fp, "while ");*/ yylval.name = strdup(yytext); return WHILE; }
"main"		{ /*fprintf(note_fp, "main ");*/ yylval.name = strdup(yytext); return MAIN; }
{numbers}	{ /*fprintf(note_fp, "num ");*/ yylval.val = atoi(yytext); return NUM; }
{id}		{ /*fprintf(note_fp, "id ");*/	yylval.name = strdup(yytext); return ID; }

{addsub}	{ yylval.name = strdup(yytext); return ADDSUB; }
{muldivmod}	{ yylval.name = strdup(yytext); return MULDIVMOD; }
{equn}		{ yylval.name = strdup(yytext); return EQUN; }
{and}		{ yylval.name = strdup(yytext); return AND; }
{or}		{ yylval.name = strdup(yytext); return OR; }
{cmp}		{ yylval.name = strdup(yytext); return CMP; }
{not}		{ yylval.name = strdup(yytext); return NOT; }

{assign}	{ /*fprintf(note_fp, "assign ");*/ yylval.name = strdup(yytext); return ASSIGN; }

{lykh}		{ /*fprintf(note_fp, "( ");*/ return LYKH; }
{rykh}		{ /*fprintf(note_fp, ") ");*/ return RYKH; }
{lfkh}		{ /*fprintf(note_fp, "[ ");*/ return LFKH; }
{rfkh}		{ /*fprintf(note_fp, "] ");*/ return RFKH; }
{ldkh}		{ /*fprintf(note_fp, "{ ");*/ return LDKH; }
{rdkh}		{ /*fprintf(note_fp, "} ");*/ return RDKH; }

{ws}		{ /*if(strstr(yytext, "\n") != NULL) fprintf(note_fp, "\n");*/ }
{semi}		{ /*fprintf(note_fp, "; ");*/ return SEMI;}
{comma}		{ /*fprintf(note_fp, ", ");*/ return COMMA; }
.			{ /*fprintf(note_fp, "error\n");*/ exit(1); }

%%

int yywrap()
{
	return 1;
}
