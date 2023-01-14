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

private:
    SymbolTable global_symbol_table;
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
    SymbolTableEntry* find_symbol(const std::string& name);

    void enter_function_mode(const SymbolTableEntry& function_entry_idx);
    void leave_function_mode();

    void gencode(const std::string& code);
    void gencode(const std::string& code, const SymbolTableEntry& op1, bool generate_instr_postfix = true);
    void gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, bool generate_instr_postfix = true);
    void gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, const SymbolTableEntry& op3,
                 bool generate_instr_postfix = true);

    void genlabel(const SymbolTableEntry& label);
    SymbolTableEntry* genfunc_call(const SymbolTableEntry& function_entry, const std::vector<SymbolTableEntry*>& arguments);
    SymbolTableEntry& genarray_get(SymbolTableEntry& function_entry, const SymbolTableEntry& expr);

    const SymbolTableEntry& convert_if_needed(const SymbolTableEntry& argument, VariableType target_type);
    SymbolTableEntry& gencode_conversions(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2);
    SymbolTableEntry& gencode_relop(const std::string& relop_code, const SymbolTableEntry& op1, const SymbolTableEntry& op2);

private:
    void geninstr(const std::string& code, VariableType instr_type, bool generate_instr_postfix);
};
