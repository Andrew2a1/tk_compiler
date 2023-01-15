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
    #include <algorithm>

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
    ARRAY     "array"
    OF        "of"
    WHILE     "while"
    DO        "do"
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
    LSBRACKET "["
    RSBRACKET "]"
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

%nterm <const SymbolTableEntry*> id
%nterm <const SymbolTableEntry*> variable
%nterm <const SymbolTableEntry*> statement
%nterm <const SymbolTableEntry*> procedure_statement
%nterm <const SymbolTableEntry*> simple_expression
%nterm <const SymbolTableEntry*> expression
%nterm <const SymbolTableEntry*> factor
%nterm <const SymbolTableEntry*> term
%nterm <const SymbolTableEntry*> subprogram_head
%nterm <const SymbolTableEntry*> declarations
%nterm <Type> type
%nterm <VariableType> standard_type
%nterm <std::vector<std::string>> identifier_list
%nterm <std::vector<std::pair<std::string, Type>>> arguments
%nterm <std::vector<std::pair<std::string, Type>>> parameter_list
%nterm <std::vector<const SymbolTableEntry*>> expression_list
%nterm <std::vector<const SymbolTableEntry*>> empty

%%

program:
    PROGRAM ID LPAREN identifier_list RPAREN SEMICOL
    declarations {
        auto &main_entrypoint = drv.symbol_table().add_label();
        drv.gencode("jump", main_entrypoint);
        $7 = &main_entrypoint;
    }
    subprogram_declarations {
        const auto &main_entrypoint = $7;
        if(main_entrypoint != nullptr)
        {
            drv.genlabel(*main_entrypoint);
        }
    }
    compound_statement
    DOT { drv.gencode("exit"); }
    ;

identifier_list:
    ID { $$.push_back($1); }
    | identifier_list COMMA ID { $$ = $1; $$.push_back($3); }
    ;

declarations:
    declarations VAR identifier_list COLON type SEMICOL { drv.symbol_table().create_variables($3, $5); }
    | %empty { $$ = nullptr; }
    ;

type:
    standard_type { $$ = Type{$1}; }
    | ARRAY LSBRACKET INT_NUMBER DOT DOT INT_NUMBER RSBRACKET OF standard_type {
        const int start_index = $3;
        const int end_index = $6;
        $$ = Type{$9, /*is_ref = */ false, ArrayTypeInfo{start_index, end_index}};
    }
    ;

standard_type:
    INT { $$ = VariableType::Integer; }
    | REAL { $$ = VariableType::Real; }
    ;

subprogram_declarations:
    subprogram_declarations subprogram_declaration SEMICOL
    | %empty
    ;

subprogram_declaration:
    subprogram_head {
        const auto &function_entry = *($1);
        drv.genlabel(function_entry);
        drv.enter_function_mode(function_entry);
    }
    declarations
    compound_statement {
        drv.leave_function_mode();
    }
    ;

subprogram_head:
    FUNCTION ID arguments COLON standard_type SEMICOL {
        auto &function_entry = drv.symbol_table().create_function($2);
        function_entry.function_info = FunctionInfo{$3, Type{$5}};
        $$ = &function_entry;
    }
    | PROCEDURE ID arguments SEMICOL {
        auto &procedure_entry = drv.symbol_table().create_function($2);
        procedure_entry.function_info = FunctionInfo{$3, std::nullopt};
        $$ = &procedure_entry;
    }
    ;

arguments:
    LPAREN parameter_list RPAREN { $$ = $2; }
    | %empty { $$ = {}; }
    ;

parameter_list:
    identifier_list COLON type {
        std::transform(($1).cbegin(), ($1).cend(), std::back_inserter($$), [&](const std::string &name){ return std::make_pair(name, $3); });
    }
    | parameter_list SEMICOL identifier_list COLON type {
        $$ = $1;
        std::transform(($3).cbegin(), ($3).cend(), std::back_inserter($$), [&](const std::string &name){ return std::make_pair(name, $5); });
    }
    ;

compound_statement:
    BEGIN optional_statements END
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
        const auto &var_entry = *($1);
        auto &expr_entry = *($3);

        if(var_entry.symbol_type != SymbolType::Variable) {
            drv.error(var_entry.id + " is not valid target for assignment.");
        }

        const auto &var_type = var_entry.var_type.type;
        const auto &dest_var = drv.convert_if_needed(expr_entry, var_type);
        drv.gencode("mov", dest_var, var_entry);
    }
    | READ LPAREN expression_list RPAREN {
        for(const auto &expr: $3)
        {
            drv.gencode("read", *expr);
        }
    }
    | WRITE LPAREN expression_list RPAREN {
        for(const auto &expr: $3)
        {
            drv.gencode("write", *expr);
        }
    }
    | IF expression {
        const auto &expr_entry = *($2);
        auto &else_label = drv.symbol_table().add_label();
        const auto &zero_const = drv.symbol_table().add_constant(0);
        drv.gencode("je", expr_entry, zero_const, else_label);
        $2 = &else_label;
    }
    THEN statement {
        auto &endif_label = drv.symbol_table().add_label();
        const auto &else_label = *($2);
        drv.gencode("jump", endif_label);
        drv.genlabel(else_label);
        $5 = &endif_label;
    }
    ELSE statement {
        const auto &endif_label = $5;
        drv.genlabel(*endif_label);
    }
    | WHILE empty {
        auto &start_label = drv.symbol_table().add_label();
        auto &end_label = drv.symbol_table().add_label();
        drv.genlabel(start_label);
        ($2).push_back(&start_label);
        ($2).push_back(&end_label);
    }
    expression DO {
        const auto &expression = *($4);
        const auto &end_label = *($2).back();
        const auto &zero_const = drv.symbol_table().add_constant(0);
        drv.gencode("je", expression, zero_const, end_label);
    }
    statement {
        const auto &start_label = *($2).front();
        const auto &end_label = *($2).back();
        drv.gencode("jump", start_label);
        drv.genlabel(end_label);
    }
    | compound_statement { $$ = nullptr; }
    | procedure_statement {
        $$ = $1;
     }
    ;

procedure_statement:
    id {
        $$ = drv.genfunc_call(*($1), {});
    }
    | id LPAREN expression_list RPAREN {
        $$ = drv.genfunc_call(*($1), $3);
    }

empty: %empty { $$ = {}; }

variable:
    id {
        $$ = $1;
    }
    | id LSBRACKET expression RSBRACKET {
        $$ = &drv.genarray_get(*($1), *($3));
    }
    ;

id:
    ID {
        auto *symbol = drv.find_symbol($1);
        if(symbol == nullptr) {
            drv.error("Variable '" + $1 + "' has not been declarated");
        }
        $$ = symbol;
    }

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
        $$ = &drv.genrelop("je", *($1), *($3));
    }
    | simple_expression NE simple_expression {
        $$ = &drv.genrelop("jne", *($1), *($3));
    }
    | simple_expression LE simple_expression {
        $$ = &drv.genrelop("jle", *($1), *($3));
    }
    | simple_expression GE simple_expression {
        $$ = &drv.genrelop("jge", *($1), *($3));
    }
    | simple_expression LO simple_expression {
        $$ = &drv.genrelop("jl", *($1), *($3));
    }
    | simple_expression GR simple_expression {
        $$ = &drv.genrelop("jg", *($1), *($3));
    }
    ;

simple_expression:
    term
    | MINUS term {
        const auto &term = *($2);
        const auto &term_type = term.var_type.type;
        const auto &zero_const = drv.symbol_table().add_constant(0, term_type);
        const auto &result_var = drv.symbol_table().add_tmp(term_type);
        drv.gencode("sub", zero_const, term, result_var);
        $$ = &result_var;
    }
    | PLUS term {
        $$ = $2;
    }
    | simple_expression MINUS term {
        $$ = &drv.genop_with_conversions("sub", *($1), *($3));
    }
    | simple_expression PLUS term {
        $$ = &drv.genop_with_conversions("add", *($1), *($3));
    }
    | simple_expression OR term {
        $$ = &drv.genop_with_conversions("or", *($1), *($3));
    }
    ;

term:
    factor
    | term STAR factor {
        $$ = &drv.genop_with_conversions("mul", *($1), *($3));
    }
    | term SLASH factor {
        $$ = &drv.genop_with_conversions("div", *($1), *($3));
    }
    | term DIV factor {
        $$ = &drv.genop_with_conversions("div", *($1), *($3));
    }
    | term MOD factor {
        $$ = &drv.genop_with_conversions("mod", *($1), *($3));
    }
    | term AND factor {
        $$ = &drv.genop_with_conversions("and", *($1), *($3));
    }
    ;

factor:
    variable
    | id LPAREN expression_list RPAREN {
        const auto *func_call = drv.genfunc_call(*($1), $3);
        if(func_call == nullptr) {
            throw std::runtime_error("Function '" + ($1)->id + "' does not return value");
        }
        $$ = func_call;
    }
    | INT_NUMBER {
        $$ = &drv.symbol_table().add_constant($1);
    }
    | REAL_NUMBER {
        $$ = &drv.symbol_table().add_constant($1);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    | NOT factor {
        const auto *factor = $2;
        const auto &factor_type = factor->var_type.type;

        if(factor_type == VariableType::Real) {
            const auto &conversion_var = drv.symbol_table().add_tmp(VariableType::Integer);
            drv.gencode("realtoint", *factor, conversion_var);
            factor = &conversion_var;
        }

        const auto &result_var = drv.symbol_table().add_tmp(VariableType::Integer);
        drv.gencode("not.i", *factor, result_var, false);
        $$ = &result_var;
    }
    ;
%%

void yy::parser::error(const location_type& location, const std::string& message)
{
    std::cerr << "Error: " << location << ": " << message << '\n';
}