#pragma once

#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "parser.h"
#include "symbol_table.h"

#define YY_DECL yy::parser::symbol_type yylex(Driver& drv)
YY_DECL;

class Driver
{
public:
    SymbolTable symbol_table;
    yy::location location;
    std::string filename;

    bool trace_scanning = false;
    bool trace_parsing = false;

private:
    std::ostream* output = nullptr;

public:
    explicit Driver(bool trace_scanning = false, bool trace_parsing = false);
    virtual ~Driver();

    int parse(const std::string& input_filename = "-", const std::string& output_filename = "-");

    void gencode(const std::string& code, int op1);
    void gencode(const std::string& code, int op1, int op2);
    void gencode(const std::string& code, int op1, int op2, int op3);

private:
    void setup_output_stream();

    void scan_begin();
    void scan_end();
};
