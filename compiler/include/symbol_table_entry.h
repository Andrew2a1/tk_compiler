#pragma once

#include <string>

#include "symbol_type.h"
#include "variable_type.h"

struct SymbolTableEntry
{
    std::string id;
    VariableType var_type;
    SymbolType symbol_type;
    double value;
    int offset;

    std::string as_operand() const;
    std::string instr_type_postfix() const;
};