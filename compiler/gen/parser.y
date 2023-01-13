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

%nterm <int> id
%nterm <int> variable
%nterm <int> expression
%nterm <int> statement
%nterm <int> procedure_statement
%nterm <int> simple_expression
%nterm <int> term
%nterm <int> factor
%nterm <Type> type
%nterm <VariableType> standard_type
%nterm <std::vector<std::string>> identifier_list
%nterm <std::vector<int>> expression_list
%nterm <std::vector<std::pair<std::string, Type>>> parameter_list
%nterm <std::vector<std::pair<std::string, Type>>> arguments
%nterm <std::vector<int>> empty

%%

program:
    PROGRAM ID LPAREN identifier_list RPAREN SEMICOL
    declarations
    subprogram_declarations
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
    standard_type { $$ = Type{$1}; }
    | ARRAY LSBRACKET INT_NUMBER DOT DOT INT_NUMBER RSBRACKET OF standard_type {
        const int start_index = $3;
        const int end_index = $6;
        $$ = Type{$9, ArrayTypeInfo{start_index, end_index}};
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
        drv.enter_function_mode();
    }
    declarations
    compound_statement {
        drv.leave_function_mode();
    }
    ;

subprogram_head:
    FUNCTION ID {
        drv.symbol_table.create_function($2);
    }
    arguments COLON standard_type SEMICOL {
        auto &function_entry = drv.symbol_table.symbols.back();
        function_entry.function_info = FunctionInfo{$4, Type{$6}};
    }
    | PROCEDURE ID {
        drv.symbol_table.create_function($2);
    }
    arguments SEMICOL {
        auto &procedure_entry = drv.symbol_table.symbols.back();
        procedure_entry.function_info = FunctionInfo{$4, std::nullopt};
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
        const auto &var_type = drv.symbol_table.symbols[$1].var_type.type;
        const int dest_var = drv.convert_if_needed($3, var_type);
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
    | IF expression {
        const int else_label = drv.symbol_table.add_label();
        const int zero_const = drv.symbol_table.add_constant(0);
        drv.gencode("je", $2, zero_const, else_label);
        $2 = else_label;
    }
    THEN statement {
        const int endif_label = drv.symbol_table.add_label();
        const int else_label = $2;
        drv.gencode("jump", endif_label);
        drv.genlabel(else_label);
        $5 = endif_label;
    }
    ELSE statement {
        const int endif_label = $5;
        drv.genlabel(endif_label);
    }
    | WHILE empty {
        const int start_label = drv.symbol_table.add_label();
        const int end_label = drv.symbol_table.add_label();
        drv.genlabel(start_label);
        ($2).push_back(start_label);
        ($2).push_back(end_label);
    }
    expression DO {
        const int expression = $4;
        const int end_label = ($2).back();
        const int zero_const = drv.symbol_table.add_constant(0);
        drv.gencode("je", expression, zero_const, end_label);
    }
    statement {
        const int start_label = ($2).front();
        const int end_label = ($2).back();
        drv.gencode("jump", start_label);
        drv.genlabel(end_label);
    }
    | compound_statement { $$ = -1; }
    | procedure_statement {
        $$ = $1;
     }
    ;

procedure_statement:
    id {
        drv.genfunc_call($1, std::vector<int>{});
    }
    | id LPAREN expression_list RPAREN {
        drv.genfunc_call($1, $3);
    }

empty: %empty { $$ = std::vector<int>{}; }

variable:
    id {
        $$ = $1;
    }
    | id LSBRACKET expression RSBRACKET {
        const auto expression_type = drv.symbol_table.symbols[$3].var_type.type;
        int expr = $3;

        if(expression_type == VariableType::Real) {
            const int conversion_var = drv.symbol_table.add_tmp(VariableType::Integer);
            drv.gencode("realtoint", expr, conversion_var);
            expr = conversion_var;
        }

        if(!drv.symbol_table.symbols[$1].var_type.is_array()) {
            drv.error("Id: " + drv.symbol_table.symbols[$1].id + " is not an array");
        }

        const auto array_var_type = drv.symbol_table.symbols[$1].var_type;
        const auto &array_info = std::get<ArrayTypeInfo>(array_var_type.type_info);

        const int address_tmp = drv.symbol_table.add_tmp(VariableType::Integer);
        const int output_ref_var = drv.symbol_table.add_tmp(VariableType::Integer);
        const int start_index_const = drv.symbol_table.add_constant(array_info.start_index);
        const int type_size_const = drv.symbol_table.add_constant(drv.symbol_table.type_size(array_var_type.type));
        const int var_offset_const = drv.symbol_table.add_constant(drv.symbol_table.symbols[$1].offset);

        drv.gencode("sub", expr, start_index_const, address_tmp);
        drv.gencode("mul", address_tmp, type_size_const, address_tmp);
        drv.gencode("add", var_offset_const, address_tmp, output_ref_var);
        drv.symbol_table.to_ref(output_ref_var, array_var_type.type);

        $$ = output_ref_var;
    }
    ;

id:
    ID {
        $$ = drv.symbol_table.find_symbol($1);
        if($$ < 0) {
            drv.error("Variable '" + $1 + "' has not been declarated");
        }
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
        const auto term_type = drv.symbol_table.symbols[$2].var_type.type;
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
    | id LPAREN expression_list RPAREN {
        $$ = drv.genfunc_call($1, $3);
        if($$ < 0) {
            throw std::runtime_error("Function '" + drv.symbol_table.symbols[$1].id + "' does not return value");
        }
    }
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
        const auto factor_type = drv.symbol_table.symbols[$2].var_type.type;
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

void yy::parser::error(const location_type& location, const std::string& message)
{
    std::cerr << "Error: " << location << ": " << message << '\n';
}