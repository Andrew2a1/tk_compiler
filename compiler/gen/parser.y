%{
#include "global.h"
void yyerror(char const *s);
int yylex();
%}

%token NUM
%token DIV
%token MOD
%token ID

%%

input:
    %empty
    | input expr ';'
    ;

expr: term
    | expr '+' term { puts ("+"); }
    | expr '-' term { puts ("-"); }
    ;

term: factor
    | term '*' factor { puts ("*"); }
    | term '/' factor { puts ("/"); }
    | term DIV factor { puts ("DIV"); }
    | term MOD factor { puts ("MOD"); }
    ;

factor: '(' expr ')'
    | NUM { printf ("%d\n", tokenval); }
    | ID { printf ("%s\n", symtable[tokenval].lexptr); }
    ;
%%

void yyerror (char const *error_description) {
  extern char *yytext;
  extern int tokenval;
  extern int lineno;
  fprintf (stderr, "ERROR: %s: token: \"%s\" value: \"%d\" on line %d\n", error_description, yytext, tokenval, lineno);
}