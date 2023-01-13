#pragma once

#include <string>

#include "function_info.h"
#include "symbol_type.h"
#include "variable_type.h"

struct SymbolTableEntry
{
    std::string id;
    Type var_type;
    SymbolType symbol_type;
    double value;
    int offset;
    FunctionInfo function_info;

    std::string as_operand() const;
};