%{
void yyerror(char const *s);
int yylex();
%}

%token NUM 256
%token DIV 257
%token MOD 258
%token ID 259

%%

input:
    %empty
    ;
%%

// void yyerror (char const *error_description) {
//   extern char *yytext;
//   extern int tokenval;
//   extern int lineno;
//   fprintf (stderr, "ERROR: %s: token: \"%s\" value: \"%d\" on line %d\n", error_description, yytext, tokenval, lineno);
//   exit(1);
// }