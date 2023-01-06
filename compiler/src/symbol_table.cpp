#include "symbol_table.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string SymbolEntry::as_operand() const
{
    std::stringstream ss;
    switch (symbol_type)
    {
        case SymbolType::Constant:
            ss << value;
            return "#" + ss.str();
        case SymbolType::Variable:
            ss << offset;
            return ss.str();
    }
    return "";
}

std::string SymbolEntry::instr_type_postfix() const
{
    switch (var_type)
    {
        case VariableType::Integer:
            return ".i";
        case VariableType::Real:
            return ".r";
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
        symbols.push_back(SymbolEntry{id, var_type, SymbolType::Variable, -1, global_offset});
        global_offset += type_size(var_type);
    }
}

int SymbolTable::type_size(VariableType var_type) const
{
    switch (var_type)
    {
        case VariableType::Integer:
            return 4;
        case VariableType::Real:
            return 8;
    }
    throw std::runtime_error("Cannot determine type size");
}

int SymbolTable::add_constant(int value, VariableType var_type)
{
    symbols.push_back(SymbolEntry{"-", var_type, SymbolType::Constant, static_cast<double>(value), -1});
    return symbols.size() - 1;
}

int SymbolTable::add_constant(double value, VariableType var_type)
{
    symbols.push_back(SymbolEntry{"-", var_type, SymbolType::Constant, value, -1});
    return symbols.size() - 1;
}

int SymbolTable::add_tmp(VariableType var_type)
{
    symbols.push_back(SymbolEntry{"$t" + std::to_string(tmp_var_count), var_type, SymbolType::Variable, -1, global_offset});

    global_offset += type_size(var_type);
    tmp_var_count += 1;

    return symbols.size() - 1;
}
