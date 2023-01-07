#pragma once

#include <string>
#include <vector>

#include "symbol_table_entry.h"
#include "variable_type.h"

class SymbolTable
{
public:
    std::vector<SymbolTableEntry> symbols;

private:
    int global_offset = 0;
    int tmp_var_count = 0;

public:
    void create_variables(const std::vector<std::string> &variable_ids, VariableType var_type);

    int add_constant(int value, VariableType var_type = VariableType::Integer);
    int add_constant(double value, VariableType var_type = VariableType::Real);

    int add_tmp(VariableType var_type);

    int find_symbol(const std::string &id) const;
    int type_size(VariableType var_type) const;
};
