#pragma once

#include <string>
#include <vector>

enum class SymbolType
{
    Variable,
    Constant
};

enum class VariableType
{
    Integer,
};

struct SymbolEntry
{
    std::string id;
    VariableType var_type;
    SymbolType symbol_type;
    int value = -1;
    int offset = -1;

    std::string as_operand() const;
};

class SymbolTable
{
private:
    int global_offset = 0;
    int tmp_var_count = 0;

public:
    std::vector<SymbolEntry> symbols;

    void create_variables(const std::vector<std::string> &variable_ids, VariableType var_type);

    int add_constant(VariableType var_type, int value);
    int add_tmp(VariableType var_type);

    int find_symbol(const std::string &id) const;
    int type_size(VariableType var_type) const;
};
