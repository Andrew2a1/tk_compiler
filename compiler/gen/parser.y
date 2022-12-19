%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%locations
%define parse.trace
%define parse.error verbose

%param { Driver& drv }

%code requires {
    class Driver;
}

%code {
#include "driver.h"
}

%define api.token.prefix {TOK_}
%token
    END  0  "END OF FILE"
    DIV     "div"
    MOD     "mod"
    SEMICOL ";"
    LPAREN  "("
    RPAREN  ")"
    PLUS    "+"
    STAR    "*"
    MINUS   "-"
    SLASH   "/"
;

%token <std::string> IDENTIFIER
%token <int> NUMBER

%%

input:
    %empty
    | input expr SEMICOL
    ;

expr: term
    | expr PLUS term { drv.result += "+\n"; }
    | expr MINUS term { drv.result += "-\n"; }
    ;

term: factor
    | term STAR factor { drv.result += "*\n"; }
    | term SLASH factor { drv.result += "/\n"; }
    | term DIV factor { drv.result += "DIV\n"; }
    | term MOD factor { drv.result += "MOD\n"; }
    ;

factor: LPAREN expr RPAREN
    | NUMBER { drv.result += std::to_string($1) + "\n"; }
    | IDENTIFIER { drv.result += $1 + "\n"; }
    ;

%%
void yy::parser::error (const location_type& location, const std::string& message)
{
  std::cerr << "Error: " << location << ": " << message << '\n';
  drv.set_error();
}