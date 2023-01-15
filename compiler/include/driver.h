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
    yy::location location;
    SymbolTable global_symbol_table;

private:
    SymbolTable local_symbol_table;

    Scanner scanner;
    yy::parser parser;

    std::ostream* output_stream;
    std::istream& input_stream;

    std::ostream& global_output;
    std::ostringstream local_output;

    std::string loc_filename;

    bool trace_scanning = false;
    bool trace_parsing = false;
    bool is_in_local_mode = false;

public:
    Driver(std::ostream& output, std::istream& input);
    int parse();

    SymbolTable& symbol_table();

    void set_debug(bool trace_scanning = false, bool trace_parsing = false);
    void set_location_filename(const std::string& filename);

    void error(const std::string& message);
    void enter_local_mode(const SymbolTableEntry& function_entry);
    void leave_local_mode();

    void gencode(const std::string& code);
    void gencode(const std::string& code, const SymbolTableEntry& op1, bool generate_instr_postfix = true);
    void gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, bool generate_instr_postfix = true);
    void gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, const SymbolTableEntry& op3,
                 bool generate_instr_postfix = true);

    void gencode_push(const SymbolTableEntry& op);
    void genlabel(const SymbolTableEntry& label);

    const SymbolTableEntry* find_symbol(const std::string& name);
    const SymbolTableEntry* genfunc_call(const SymbolTableEntry& function_entry, const std::vector<const SymbolTableEntry*>& arguments);
    const SymbolTableEntry& genarray_get(const SymbolTableEntry& array, const SymbolTableEntry& expr);
    const SymbolTableEntry& genop_with_conversions(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2);
    const SymbolTableEntry& genrelop(const std::string& relop_code, const SymbolTableEntry& op1, const SymbolTableEntry& op2);
    const SymbolTableEntry& convert_if_needed(const SymbolTableEntry& argument, VariableType target_type);

private:
    void geninstr(const std::string& code, VariableType instr_type, bool generate_instr_postfix);
    const SymbolTableEntry* check_function(const SymbolTableEntry& function_entry, const std::vector<const SymbolTableEntry*>& arguments);
    void gen_function_argument_passing(const SymbolTableEntry& function_entry, const std::vector<const SymbolTableEntry*>& arguments);
    void init_function_symbol_table(const SymbolTableEntry& function_entry);
};
