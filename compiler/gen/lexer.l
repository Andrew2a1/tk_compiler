%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <string>
#include "driver.h"
#include "parser.h"
%}

%option noyywrap nounput noinput batch debug

id  [a-zA-Z_][a-zA-Z0-9_]*
int   [0-9]+
blank [ \t\r]

%{
    // Code run each time a pattern is matched.
    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%
%{
    // Code run each time yylex is called.
    yy::location& loc = drv.location;
    loc.step ();
%}

{blank}+  loc.step();
\n+       { loc.lines(yyleng); loc.step(); }
program   return yy::parser::make_PROGRAM(loc);
read      return yy::parser::make_READ(loc);
write     return yy::parser::make_WRITE(loc);
var       return yy::parser::make_VAR(loc);
integer   return yy::parser::make_INT(loc);
begin     return yy::parser::make_BEGIN(loc);
end       return yy::parser::make_END(loc);
div       return yy::parser::make_DIV(loc);
mod       return yy::parser::make_MOD(loc);
or        return yy::parser::make_OR(loc);
and       return yy::parser::make_AND(loc);
:=        return yy::parser::make_ASSIGN(loc);
\+        return yy::parser::make_PLUS(loc);
\-        return yy::parser::make_MINUS(loc);
\*        return yy::parser::make_STAR(loc);
\/        return yy::parser::make_SLASH(loc);
;         return yy::parser::make_SEMICOL(loc);
:         return yy::parser::make_COLON(loc);
,         return yy::parser::make_COMMA(loc);
\.        return yy::parser::make_DOT(loc);
\(        return yy::parser::make_LPAREN(loc);
\)        return yy::parser::make_RPAREN(loc);
{int}     return yy::parser::make_NUMBER(std::stoi(yytext), loc);
{id}      return yy::parser::make_ID(yytext, loc);
<<EOF>>   return yy::parser::make_EOF(loc);
%%

void Driver::scan_begin()
{
    yy_flex_debug = trace_scanning;
    if (filename.empty() || filename == "-")
        yyin = stdin;
    else if (!(yyin = fopen(filename.c_str(), "r")))
    {
        std::cerr << "Cannot open " << filename << ": " << std::strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void Driver::scan_end()
{
    fclose(yyin);
}

Driver::~Driver()
{
    yylex_destroy();
}