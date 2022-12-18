%{
#include "global.h"
#include "parser.h"
#include "symbol.h"
#define NONE -1

int insert (char s[], int tok);
int lookup(char s[]);

int lineno = 1;
int tokenval = NONE;
%}

%option noyywrap

id  [a-zA-Z_][a-zA-Z0-9_]*

%%
\n { ++lineno; }
[ \t] {}
div { tokenval=NONE; return DIV; }
mod { tokenval=NONE; return MOD; }
[0-9]+ { sscanf(yytext, "%d", &tokenval); return NUM;}
{id} { int p = 0; return symtable[p].token;}
. { tokenval = NONE; return *yytext; }

%%