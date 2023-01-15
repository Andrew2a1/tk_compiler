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

bool Type::operator==(const Type &other) const
{
    if (type != other.type)
    {
        return false;
    }

    if (is_array() && other.is_array())
    {
        const auto &self_type_info = array_info.value();
        const auto &other_type_info = other.array_info.value();
        return (self_type_info.start_index == other_type_info.start_index) && (self_type_info.end_index == other_type_info.end_index);
    }
    else if (is_array() || other.is_array())
    {
        return false;
    }

    return is_reference == other.is_reference;
}
