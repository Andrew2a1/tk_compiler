#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "variable_type.h"

struct FunctionInfo
{
    std::vector<std::pair<std::string, Type>> arguments;
    std::optional<Type> return_type = std::nullopt;
};
