#pragma once

#include <string>

enum class VariableType
{
    Integer,
    Real
};

std::string instr_postfix(VariableType variable_type);