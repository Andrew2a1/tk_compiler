#include "variable_type.h"

std::string instr_postfix(VariableType variable_type)
{
    switch (variable_type)
    {
        case VariableType::Integer:
            return ".i";
        case VariableType::Real:
            return ".r";
    }
    return "";
}