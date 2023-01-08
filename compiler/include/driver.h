#pragma once

#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "parser.h"
#include "scanner.h"
#include "symbol_table.h"

class Driver
{
public:
    SymbolTable symbol_table;
    yy::location location;

private:
    std::ostream& output_stream;
    std::istream& input_stream;
    std::string loc_filename;

    bool trace_scanning = false;
    bool trace_parsing = false;

public:
    Driver(std::ostream& output, std::istream& input);
    int parse();

    void set_debug(bool trace_scanning = false, bool trace_parsing = false);
    void set_location_filename(const std::string& filename);

    void gencode(const std::string& code);
    void gencode(const std::string& code, int op1, bool generate_instr_postfix = true);
    void gencode(const std::string& code, int op1, int op2, bool generate_instr_postfix = true);
    void gencode(const std::string& code, int op1, int op2, int op3, bool generate_instr_postfix = true);

    int gencode_conversions(const std::string& code, int op1, int op2);
    int gencode_relop(const std::string& relop_code, int op1, int op2);
};
