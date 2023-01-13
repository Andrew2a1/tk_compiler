#pragma once

#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"
#include "scanner.h"
#include "symbol_table.h"

class Driver
{
public:
    SymbolTable symbol_table;
    yy::location location;

private:
    std::ostream* output_stream;
    std::istream& input_stream;

    std::ostringstream local_output;
    std::ostream* global_output;

    Scanner scanner;
    yy::parser parser;

    std::string loc_filename;

    bool trace_scanning = false;
    bool trace_parsing = false;

    int subprogram_symbol_index = -1;

public:
    Driver(std::ostream& output, std::istream& input);
    int parse();

    void set_debug(bool trace_scanning = false, bool trace_parsing = false);
    void set_location_filename(const std::string& filename);
    void error(const std::string& message);

    void enter_function_mode(int function_entry_idx);
    void leave_function_mode();

    void gencode(const std::string& code);
    void gencode(const std::string& code, int op1, bool generate_instr_postfix = true);
    void gencode(const std::string& code, int op1, int op2, bool generate_instr_postfix = true);
    void gencode(const std::string& code, int op1, int op2, int op3, bool generate_instr_postfix = true);

    void genlabel(int label);
    int genfunc_call(int function_idx, const std::vector<int>& arguments);

    int convert_if_needed(int argument, VariableType target_type);
    int gencode_conversions(const std::string& code, int op1, int op2);
    int gencode_relop(const std::string& relop_code, int op1, int op2);

private:
    void geninstr(const std::string& code, VariableType instr_type, bool generate_instr_postfix);
};
