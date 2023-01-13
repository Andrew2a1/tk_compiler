#include "symbol_table_entry.h"

#include <sstream>

std::string SymbolTableEntry::as_operand() const
{
    std::stringstream ss;
    switch (symbol_type)
    {
        case SymbolType::Constant:
            if (var_type.type == VariableType::Integer)
                ss << '#' << static_cast<int>(value);
            else
                ss << '#' << value;
            return ss.str();
        case SymbolType::Variable:
            if (!var_type.is_array() && std::get<StandardTypeInfo>(var_type.type_info).is_reference)
            {
                ss << '*';
            }
            ss << offset;
            return ss.str();
        case SymbolType::Label:
        case SymbolType::Function:
            return "#" + id;
    }
    return "";
}
