#include "cli_utils.h"

#include <sstream>
#include <tabulate/table.hpp>

std::string var_type_to_str(const Type &var_type)
{
    std::stringstream ss;
    switch (var_type.type)
    {
        case VariableType::Integer:
            ss << "integer";
            break;
        case VariableType::Real:
            ss << "real";
            break;
        default:
            ss << "UNKNOWN";
            break;
    }

    if (var_type.is_array())
    {
        const auto &array_info = std::get<ArrayTypeInfo>(var_type.type_info);
        ss << '[' << array_info.start_index << ".." << array_info.end_index << ']';
    }
    else
    {
        const auto &array_info = std::get<StandardTypeInfo>(var_type.type_info);
        if (array_info.is_reference)
        {
            ss << '*';
        }
    }

    return ss.str();
}

std::string symbol_type_str(const SymbolTableEntry &entry)
{
    std::stringstream ss;
    switch (entry.symbol_type)
    {
        case SymbolType::Constant:
            return "const";
        case SymbolType::Variable:
            return "var";
        case SymbolType::Label:
            return "label";
        case SymbolType::Function:
            ss << "function(";
            for (const auto &[arg_name, arg_type] : entry.function_info.arguments)
            {
                ss << arg_name << ": " << var_type_to_str(arg_type) << ", ";
            }
            ss << ')';
            if (entry.function_info.return_type.has_value())
            {
                ss << ": " << var_type_to_str(entry.function_info.return_type.value());
            }
            return ss.str();
    }
    return "UNKNOWN";
}

void print_symbol_table(const SymbolTable &table)
{
    tabulate::Table symtable;
    symtable.add_row({"ID", "Type", "Var Type", "Value", "Offset"});

    for (const auto &symbol : table.symbols)
    {
        std::string value_str;
        if (symbol.var_type.type == VariableType::Integer)
        {
            value_str = (symbol.symbol_type == SymbolType::Constant) ? std::to_string(static_cast<int>(symbol.value)) : "-";
        }
        else
        {
            value_str = (symbol.symbol_type == SymbolType::Constant) ? std::to_string(symbol.value) : "-";
        }

        const std::string var_type_str = (symbol.symbol_type != SymbolType::Function) ? var_type_to_str(symbol.var_type) : "-";
        const std::string offset_str = (symbol.symbol_type == SymbolType::Variable) ? std::to_string(symbol.offset) : "-";

        symtable.add_row({symbol.id, symbol_type_str(symbol), var_type_str, value_str, offset_str});
    }

    std::cout << symtable << std::endl;
}

bool use_console(const std::string &filename)
{
    if (filename.empty() || filename == "-")
    {
        return true;
    }
    return false;
}
