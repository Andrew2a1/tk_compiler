#include "symbol_table_entry.h"

#include <cmath>
#include <sstream>

std::string SymbolTableEntry::as_operand() const
{
    std::stringstream ss;
    int sign = 1;
    switch (symbol_type)
    {
        case SymbolType::Constant:
            ss << '#';
            if (is_local)
            {
                ss << "BP" << (value < 0 ? "+" : "");
                sign = -1;
            }
            if (var_type.type == VariableType::Integer)
                ss << sign * static_cast<int>(value);
            else
                ss << sign * value;
            return ss.str();
        case SymbolType::Variable:
            if (var_type.is_reference)
            {
                ss << '*';
            }
            if (is_local)
            {
                ss << "BP" << (offset < 0 ? '+' : '-');
            }
            ss << std::abs(offset);
            return ss.str();
        case SymbolType::Label:
        case SymbolType::Function:
            return "#" + id;
    }
    return "";
}
