#pragma once

#include <string>
#include <vector>

#include "symbol_table_entry.h"
#include "variable_type.h"

class SymbolTable
{
public:
    std::vector<SymbolTableEntry> symbols;
    int label_count = 0;

private:
    bool is_local = false;
    int global_offset = 0;
    int tmp_var_count = 0;

public:
    explicit SymbolTable(int label_count = 0, bool is_local = false);
    void create_variables(const std::vector<std::string> &variable_ids, const Type &type);

    int create_variable(const std::string &variable_name, const Type &type, int offset);
    int create_function(const std::string &function_name);

    int add_constant(int value, VariableType var_type = VariableType::Integer);
    int add_constant(double value, VariableType var_type = VariableType::Real);

    int add_tmp(VariableType var_type, bool is_ref = false);
    int add_label();

    int to_ref(int symbol, VariableType var_type);

    int find_symbol(const std::string &id) const;
    int type_size(const Type &var_type) const;
    int type_size(VariableType var_type) const;

    int get_symbol_offset(int symbol) const;
    int get_total_var_size() const;

    void set_var_offset(int new_offset) { global_offset = new_offset; };
};
