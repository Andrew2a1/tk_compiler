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
    yy::parser::symbol_type make_NUMBER (const std::string &s, const yy::parser::location_type& loc);
%}

%{
    // Code run each time a pattern is matched.
    # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%
%{
    // Code run each time yylex is called.
    yy::location& loc = drv.location;
    loc.step ();
%}

{blank}+  { loc.step(); }
\n+       { loc.lines(yyleng); loc.step(); }
div       { return yy::parser::make_DIV(loc) ; }
mod       { return yy::parser::make_MOD(loc); }
\+        { return yy::parser::make_PLUS(loc); }
\-        { return yy::parser::make_MINUS(loc); }
\*        { return yy::parser::make_STAR(loc); }
\/        { return yy::parser::make_SLASH(loc); }
;         { return yy::parser::make_SEMICOL(loc); }
\(        { return yy::parser::make_LPAREN(loc); }
\)        { return yy::parser::make_RPAREN(loc); }
{int}     { return make_NUMBER(yytext, loc);}
{id}      { return yy::parser::make_IDENTIFIER(yytext, loc); }
<<EOF>>   return yy::parser::make_END(loc);
%%

yy::parser::symbol_type make_NUMBER (const std::string &s, const yy::parser::location_type& loc)
{
    errno = 0;
    long n = strtol (s.c_str(), NULL, 10);
    if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    {
        throw yy::parser::syntax_error (loc, "integer is out of range: " + s);
    }
    return yy::parser::make_NUMBER ((int) n, loc);
}

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