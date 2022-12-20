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
    #include "symbol_table.h"
    #include <stdexcept>
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
%nterm <std::vector<std::string>> identifier_list
%nterm <VariableType> type
%nterm <int> variable
%nterm <int> expression
%nterm <std::vector<int>> expression_list
%nterm <int> simple_expression
%nterm <int> term
%nterm <int> factor

%%

program:
    PROGRAM ID LPAREN identifier_list RPAREN SEMICOL
    declarations
    // subprogram_declarations
    compound_statement
    DOT
    ;

identifier_list:
    ID { $$.push_back($1); }
    | identifier_list COMMA ID { $$ = $1; $$.push_back($3); }
    ;

declarations:
    declarations VAR identifier_list COLON type SEMICOL { drv.symbol_table.create_variables($3, $5); }
    | %empty
    ;

type:
    INT { $$ = VariableType::Integer; }
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
    variable ASSIGN expression  { drv.gencode("mov.i", $3, $1); }
    | READ LPAREN identifier_list RPAREN {
        for(const auto &id: $3)
        {
            drv.gencode("read.i", drv.symbol_table.find_symbol(id));
        }
    }
    | WRITE LPAREN expression_list RPAREN {
        for(const auto &id: $3)
        {
            drv.gencode("write.i", id);
        }
    }
    // | procedure_statement
    // | compound_statement
    // | IF expression THEN statement ELSE statement
    // | WHILE expression DO statement
    ;

variable:
    ID { $$ = drv.symbol_table.find_symbol($1); if($$ == -1) throw std::runtime_error("Variable " + $1 + " has not been declarated"); }
    // | ID '[' expression ']'
    ;

expression_list:
    expression { $$.push_back($1); }
    | expression_list COMMA expression { $$ = $1; $$.push_back($3); }
    ;

expression:
    simple_expression
    // | simple_expression relop simple_expression
    ;

simple_expression:
    term
    | MINUS term {
        const int zero_const = drv.symbol_table.add_constant(VariableType::Integer, 0);
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("sub.i", zero_const, $2, result);
        $$ = result;
        }
    | PLUS term { $$ = $2; }
    | simple_expression MINUS term {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("sub.i", $1, $3, result);
        $$ = result;
        }
    | simple_expression PLUS term {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("add.i", $1, $3, result);
        $$ = result;
        }
    // | simple_expression OR term
    ;

term:
    factor
    | term STAR factor {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("mul.i", $1, $3, result);
        $$ = result;
        }
    | term SLASH factor {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("div.i", $1, $3, result);
        $$ = result;
        }
    | term DIV factor {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("div.i", $1, $3, result);
        $$ = result;
        }
    | term MOD factor {
        const int result = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("mod.i", $1, $3, result);
        $$ = result;
        }
    // | term AND factor
    ;

factor:
    variable
    // | ID LPAREN expression_list RPAREN
    | NUMBER { $$ = drv.symbol_table.add_constant(VariableType::Integer, $1); }
    | LPAREN expression RPAREN { $$ = $2; }
    // | NOT factor
    ;
%%

void yy::parser::error (const location_type& location, const std::string& message)
{
    std::cerr << "Error: " << location << ": " << message << '\n';
}