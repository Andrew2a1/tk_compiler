#pragma once

#include <optional>
#include <string>

enum class VariableType
{
    Integer,
    Real
};

std::string instr_postfix(VariableType variable_type);

struct ArrayTypeInfo
{
    int start_index;
    int end_index;
};

struct Type
{
    VariableType type;
    bool is_reference = false;
    std::optional<ArrayTypeInfo> array_info = std::nullopt;

    bool is_array() const { return array_info.has_value(); }
    bool operator==(const Type &other) const;
};
