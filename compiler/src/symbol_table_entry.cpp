#include "symbol_table_entry.h"

#include <sstream>

std::string SymbolTableEntry::as_operand() const
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
        case SymbolType::Label:
            return "#" + id;
    }
    return "";
}

std::string SymbolTableEntry::instr_type_postfix() const
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