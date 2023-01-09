#pragma once

#include <string>
#include <variant>

enum class VariableType
{
    Integer,
    Real
};

std::string instr_postfix(VariableType variable_type);

struct StandardTypeInfo
{
    bool is_reference = false;
};

struct ArrayTypeInfo
{
    int start_index;
    int end_index;
};

struct Type
{
    VariableType type;
    std::variant<StandardTypeInfo, ArrayTypeInfo> type_info = StandardTypeInfo{};
    bool is_array() const { return std::holds_alternative<ArrayTypeInfo>(type_info); }
};
