%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.5"

%defines
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%locations
%define parse.trace
%define parse.error verbose

%param { Scanner& scanner }
%param { Driver& drv }

%code requires {
    #include "symbol_table.h"
    #include <stdexcept>

    class Scanner;
    class Driver;
}

%code {
    #include "driver.h"

    static yy::parser::symbol_type yylex(Scanner &scanner, Driver &) {
        return scanner.get_next_token();
    }
}

%define api.token.prefix {TOK_}
%token
    EOF  0    "END OF FILE"
    PROGRAM   "program"
    INT       "integer"
    FUNCTION  "function"
    PROCEDURE "procedure"
    REAL      "real"
    DIV       "div"
    MOD       "mod"
    NOT       "not"
    IF        "if"
    THEN      "then"
    ELSE      "else"
    EQ        "="
    NE        "<>"
    LE        "<="
    GE        ">="
    LO        "<"
    GR        ">"
    SEMICOL   ";"
    COLON     ":"
    LPAREN    "("
    RPAREN    ")"
    ASSIGN    ":="
    PLUS      "+"
    STAR      "*"
    MINUS     "-"
    SLASH     "/"
    DOT       "."
    COMMA     ","
    VAR       "var"
    OR        "or"
    AND       "and"
    BEGIN     "begin"
    END       "end"
    READ      "read"
    WRITE     "write"
;

%token <std::string> ID
%token <int> INT_NUMBER
%token <double> REAL_NUMBER

%nterm <std::vector<std::string>> identifier_list
%nterm <VariableType> type
%nterm <VariableType> standard_type
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
    DOT { drv.gencode("exit"); }
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
    standard_type { $$ = $1; }
    ;

standard_type:
    INT { $$ = VariableType::Integer; }
    | REAL { $$ = VariableType::Real; }
    ;

// subprogram_declarations:
//     subprogram_declarations subprogram_declaration SEMICOL
//     | %empty
//     ;

// subprogram_declaration:
    // subprogram_head declarations compound_statement
    // ;

// subprogram_head:
//     FUNCTION ID arguments COLON standard_type SEMICOL
//     | PROCEDURE ID arguments SEMICOL
//     ;

// arguments:
//     LPAREN parameter_list RPAREN
//     | %empty
//     ;

// parameter_list:
//     identifier_list COLON type
//     | parameter_list SEMICOL identifier_list : type
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
    variable ASSIGN expression  {
        const auto var_type = drv.symbol_table.symbols[$1].var_type;
        const auto expr_type = drv.symbol_table.symbols[$3].var_type;
        int dest_var = $3;

        if(var_type != expr_type) {
            dest_var = drv.symbol_table.add_tmp(var_type);
            if(expr_type == VariableType::Integer && var_type == VariableType::Real) {
                drv.gencode("inttoreal", $3, dest_var);
            } else if (expr_type == VariableType::Real && var_type == VariableType::Integer) {
                drv.gencode("realtoint", $3, dest_var);
            }
        }
        drv.gencode("mov", dest_var, $1);
    }
    | READ LPAREN identifier_list RPAREN {
        for(const auto &id: $3)
        {
            drv.gencode("read", drv.symbol_table.find_symbol(id));
        }
    }
    | WRITE LPAREN expression_list RPAREN {
        for(const auto &id: $3)
        {
            drv.gencode("write", id);
        }
    }
    | IF expression THEN statement ELSE statement // if_stmt
    // | procedure_statement
    // | compound_statement
    // | WHILE expression DO statement
    ;

// if_stmt:
//     IF expression THEN if_matched_stmt optional_tail
//     | statement
//     ;

// if_matched_stmt:
//     IF expression THEN if_matched_stmt ELSE if_matched_stmt
//     | statement
//     ;

// optional_tail:
//     ELSE tail
//     | %empty
//     ;

// tail:
//     IF expression THEN tail
//     | statement

variable:
    ID {
        $$ = drv.symbol_table.find_symbol($1);
        if($$ < 0) {
            drv.error("Variable '" + $1 + "' has not been declarated");
        }
    }
    // | ID '[' expression ']'
    ;

expression_list:
    expression {
        $$.push_back($1);
    }
    | expression_list COMMA expression {
        $$ = $1;
        $$.push_back($3);
    }
    ;

expression:
    simple_expression
    | simple_expression EQ simple_expression {
        $$ = drv.gencode_relop("je", $1, $3);
    }
    | simple_expression NE simple_expression {
        $$ = drv.gencode_relop("jne", $1, $3);
    }
    | simple_expression LE simple_expression {
        $$ = drv.gencode_relop("jle", $1, $3);
    }
    | simple_expression GE simple_expression {
        $$ = drv.gencode_relop("jge", $1, $3);
    }
    | simple_expression LO simple_expression {
        $$ = drv.gencode_relop("jl", $1, $3);
    }
    | simple_expression GR simple_expression {
        $$ = drv.gencode_relop("jg", $1, $3);
    }
    ;

simple_expression:
    term
    | MINUS term {
        const auto term_type = drv.symbol_table.symbols[$2].var_type;
        const int zero_const = drv.symbol_table.add_constant(0, term_type);
        const int result_var = drv.symbol_table.add_tmp(term_type);
        drv.gencode("sub", zero_const, $2, result_var);
        $$ = result_var;
    }
    | PLUS term {
        $$ = $2;
    }
    | simple_expression MINUS term {
        $$ = drv.gencode_conversions("sub", $1, $3);
    }
    | simple_expression PLUS term {
        $$ = drv.gencode_conversions("add", $1, $3);
    }
    | simple_expression OR term {
        $$ = drv.gencode_conversions("or", $1, $3);
    }
    ;

term:
    factor
    | term STAR factor {
        $$ = drv.gencode_conversions("mul", $1, $3);
    }
    | term SLASH factor {
        $$ = drv.gencode_conversions("div", $1, $3);
    }
    | term DIV factor {
        $$ = drv.gencode_conversions("div", $1, $3);
    }
    | term MOD factor {
        $$ = drv.gencode_conversions("mod", $1, $3);
    }
    | term AND factor {
        $$ = drv.gencode_conversions("and", $1, $3);
    }
    ;

factor:
    variable
    // | ID LPAREN expression_list RPAREN
    | INT_NUMBER {
        $$ = drv.symbol_table.add_constant($1);
    }
    | REAL_NUMBER {
        $$ = drv.symbol_table.add_constant($1);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    | NOT factor {
        const auto factor_type = drv.symbol_table.symbols[$2].var_type;
        int factor = $2;

        if(factor_type == VariableType::Real) {
            const int conversion_var = drv.symbol_table.add_tmp(VariableType::Integer);
            drv.gencode("realtoint", $2, conversion_var);
            factor = conversion_var;
        }

        const int result_var = drv.symbol_table.add_tmp(VariableType::Integer);
        drv.gencode("not.i", factor, result_var, false);
        $$ = result_var;
    }
    ;
%%

void yy::parser::error (const location_type& location, const std::string& message)
{
    std::cerr << "Error: " << location << ": " << message << '\n';
}