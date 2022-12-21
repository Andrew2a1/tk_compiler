#include "symbol_table.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

std::string SymbolEntry::as_operand() const
{
    switch (symbol_type)
    {
        case SymbolType::Constant:
            return "#" + std::to_string(value);
        case SymbolType::Variable:
            return std::to_string(offset);
    }
    return "";
}

int SymbolTable::find_symbol(const std::string &id) const
{
    const auto iter = std::find_if(symbols.cbegin(), symbols.cend(), [&id](const SymbolEntry &entry) { return entry.id == id; });
    if (iter == symbols.cend())
    {
        return -1;
    }
    return iter - symbols.cbegin();
}

void SymbolTable::create_variables(const std::vector<std::string> &variable_ids, VariableType var_type)
{
    for (const auto &id : variable_ids)
    {
        const SymbolEntry new_entry{id, var_type, SymbolType::Variable, -1, global_offset};
        symbols.push_back(new_entry);
        global_offset += type_size(var_type);
    }
}

int SymbolTable::type_size(VariableType var_type) const
{
    switch (var_type)
    {
        case VariableType::Integer:
            return 4;
    }
    throw std::runtime_error("Cannot determine type size");
}

int SymbolTable::add_constant(VariableType var_type, int value)
{
    const SymbolEntry new_entry{"-", var_type, SymbolType::Constant, value, -1};
    symbols.push_back(new_entry);
    return symbols.size() - 1;
}

int SymbolTable::add_tmp(VariableType var_type)
{
    const SymbolEntry new_entry{"$t" + std::to_string(tmp_var_count), var_type, SymbolType::Variable, -1, global_offset};
    symbols.push_back(new_entry);

    global_offset += type_size(var_type);
    tmp_var_count += 1;

    return symbols.size() - 1;
}
