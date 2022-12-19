%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5"
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
    EOF  0  "END OF FILE"
    PROGRAM "program"
    INT     "integer"
    DIV     "div"
    MOD     "mod"
    SEMICOL ";"
    COLON   ":"
    LPAREN  "("
    RPAREN  ")"
    ASSIGN  ":="
    PLUS    "+"
    STAR    "*"
    MINUS   "-"
    SLASH   "/"
    DOT     "."
    COMMA   ","
    VAR     "var"
    BEGIN   "begin"
    END     "end"
    READ    "read"
    WRITE   "write"
;

%token <std::string> ID
%token <int> NUMBER

%%

program:
    PROGRAM ID LPAREN identifier_list RPAREN SEMICOL
    declarations
    // subprogram_declarations
    compound_statement
    DOT
    ;

identifier_list:
    ID
    | identifier_list COMMA ID
    ;

declarations:
    declarations VAR identifier_list COLON type SEMICOL
    | %empty
    ;

type:
    INT
    ;

// subprogram_declarations:
//     subprogram_declarations subprogram_declaration SEMICOL
//     | %empty
//     ;

compound_statement:
    BEGIN
    optional_statements
    END
    ;

optional_statements:
    statement_list
    | %empty
    ;

statement_list:
    statement
    | statement_list SEMICOL statement
    ;

statement:
    variable ASSIGN expression
    | READ LPAREN identifier_list RPAREN
    | WRITE LPAREN expression_list RPAREN
    // | procedure_statement
    // | compound_statement
    // | IF expression THEN statement ELSE statement
    // | WHILE expression DO statement
    ;

variable:
    ID
    // | ID '[' expression ']'
    ;

expression_list:
    expression
    | expression_list COMMA expression
    ;

expression:
    simple_expression
    // | simple_expression relop simple_expression
    ;

simple_expression:
    term
    | MINUS term
    | PLUS term
    | simple_expression MINUS term
    | simple_expression PLUS term
    // | simple_expression OR term
    ;

term:
    factor
    | term STAR factor
    | term SLASH factor
    | term DIV factor
    | term MOD factor
    // | term AND factor
    ;

factor:
    variable
    | ID LPAREN expression_list RPAREN
    | NUMBER
    | LPAREN expression RPAREN
    // | NOT factor
    ;
%%

void yy::parser::error (const location_type& location, const std::string& message)
{
    std::cerr << "Error: " << location << ": " << message << '\n';
}