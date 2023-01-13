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

void SymbolTable::create_variables(const std::vector<std::string> &variable_ids, const Type &type)
{
    for (const auto &id : variable_ids)
    {
        symbols.push_back({id, type, SymbolType::Variable, -1, global_offset, {}});
        global_offset += type_size(type);
    }
}

int SymbolTable::create_function(const std::string &function_name)
{
    symbols.push_back({function_name, Type{VariableType::Integer}, SymbolType::Function, -1, -1, {}});
    return symbols.size() - 1;
}

int SymbolTable::type_size(const Type &var_type) const
{
    const int base_size = type_size(var_type.type);
    if (var_type.is_array())
    {
        const auto &array_info = std::get<ArrayTypeInfo>(var_type.type_info);
        return base_size * (array_info.end_index - array_info.start_index + 1);
    }
    if (std::get<StandardTypeInfo>(var_type.type_info).is_reference)
    {
        return type_size(VariableType::Integer);
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
    const Type type{var_type, StandardTypeInfo{is_ref}};
    symbols.push_back({"$t" + std::to_string(tmp_var_count), type, SymbolType::Variable, -1, global_offset, {}});

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
    assert(symbols[symbol].var_type.is_array() == false);
    symbols[symbol].var_type = Type{var_type, StandardTypeInfo{/* is_reference = */ true}};
    return symbol;
}
