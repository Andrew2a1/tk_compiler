#pragma once

#include <list>
#include <string>

#include "symbol_table_entry.h"
#include "variable_type.h"

class SymbolTable
{
public:
    std::list<SymbolTableEntry> symbols;
    int label_count = 0;

private:
    bool is_local = false;
    int global_offset = 0;
    int tmp_var_count = 0;

public:
    explicit SymbolTable(int label_count = 0, bool is_local = false);
    void create_variables(const std::vector<std::string> &variable_ids, const Type &type);

    SymbolTableEntry &create_variable(const std::string &variable_name, const Type &type);
    SymbolTableEntry &create_function(const std::string &function_name);

    SymbolTableEntry &add_constant(int value, VariableType var_type = VariableType::Integer);
    SymbolTableEntry &add_constant(double value, VariableType var_type = VariableType::Real);

    SymbolTableEntry &add_tmp(VariableType var_type, bool is_ref = false);
    SymbolTableEntry &add_label();

    SymbolTableEntry &to_ref(SymbolTableEntry &symbol, VariableType new_type);

    SymbolTableEntry *find_symbol(const std::string &id);
    int type_size(const Type &var_type) const;
    int type_size(VariableType var_type) const;

    int get_symbol_offset(const SymbolTableEntry &symbol) const;
    int get_total_var_size() const;

    void set_var_offset(int new_offset) { global_offset = new_offset; };
};
