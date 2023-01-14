#include "symbol_table.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

int SymbolTable::find_symbol(const std::string &id) const
{
    const auto iter = std::find_if(symbols.cbegin(), symbols.cend(), [&id](const SymbolTableEntry &entry) { return entry.id == id; });
    if (iter == symbols.cend())
    {
        return -1;
    }
    return iter - symbols.cbegin();
}

SymbolTable::SymbolTable(int label_count, bool is_local) : label_count(label_count), is_local(is_local) {}

void SymbolTable::create_variables(const std::vector<std::string> &variable_ids, const Type &type)
{
    for (const auto &id : variable_ids)
    {
        create_variable(id, type, global_offset);
    }
}

int SymbolTable::create_variable(const std::string &variable_name, const Type &type, int offset)
{
    assert(find_symbol(variable_name) < 0);
    symbols.push_back({variable_name, type, SymbolType::Variable, -1, offset, {}, is_local});
    global_offset += type_size(type);
    return symbols.size() - 1;
}

int SymbolTable::create_function(const std::string &function_name)
{
    assert(is_local == false);
    symbols.push_back({function_name, Type{VariableType::Integer}, SymbolType::Function, -1, -1, {}});
    return symbols.size() - 1;
}

int SymbolTable::type_size(const Type &var_type) const
{
    if (var_type.is_reference)
    {
        return type_size(VariableType::Integer);
    }

    const int base_size = type_size(var_type.type);
    if (var_type.is_array())
    {
        const auto &array_info = var_type.type_info.value();
        return base_size * (array_info.end_index - array_info.start_index + 1);
    }

    return base_size;
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
    return -1;
}

int SymbolTable::get_symbol_offset(int symbol) const { return is_local ? -symbols[symbol].offset : symbols[symbol].offset; }

int SymbolTable::get_total_var_size() const
{
    int total_size = 0;
    for (const auto &entry : symbols)
    {
        if (entry.symbol_type == SymbolType::Variable && entry.offset > 0)
        {
            total_size += type_size(entry.var_type);
        }
    }
    return total_size;
}

int SymbolTable::add_constant(int value, VariableType var_type)
{
    symbols.push_back({"-", Type{var_type}, SymbolType::Constant, static_cast<double>(value), -1, {}});
    return symbols.size() - 1;
}

int SymbolTable::add_constant(double value, VariableType var_type)
{
    symbols.push_back({"-", Type{var_type}, SymbolType::Constant, value, -1, {}});
    return symbols.size() - 1;
}

int SymbolTable::add_tmp(VariableType var_type, bool is_ref)
{
    const Type type{var_type, is_ref};
    symbols.push_back({"$t" + std::to_string(tmp_var_count), type, SymbolType::Variable, -1, global_offset, {}, is_local});

    global_offset += type_size(type);
    tmp_var_count += 1;

    return symbols.size() - 1;
}

int SymbolTable::add_label()
{
    symbols.push_back({"L" + std::to_string(label_count), Type{VariableType::Integer}, SymbolType::Label, -1, -1, {}});
    label_count += 1;
    return symbols.size() - 1;
}

int SymbolTable::to_ref(int symbol, VariableType var_type)
{
    assert(symbols[symbol].symbol_type == SymbolType::Variable);
    symbols[symbol].var_type = Type{var_type, true};
    return symbol;
}
