%{
#include "parser.h"
#define NONE -1

extern int insert (char s[], int tok);
extern int lookup (char s[]);

int lineno = 1;
int tokenval = NONE;
%}

%option noyywrap nounput noinput batch

id  [a-zA-Z_][a-zA-Z0-9_]*

%%
\n { ++lineno; }
[ \t] {}

%%
