#include "cli_utils.h"

#include <tabulate/table.hpp>

std::string var_type_to_str(VariableType var_type)
{
    switch (var_type)
    {
        case VariableType::Integer:
            return "integer";
        case VariableType::Real:
            return "real";
    }
    return "UNKNOWN";
}

std::string symbol_type_to_str(SymbolType symbol_type)
{
    switch (symbol_type)
    {
        case SymbolType::Constant:
            return "const";
        case SymbolType::Variable:
            return "var";
        case SymbolType::Label:
            return "label";
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
        if (symbol.var_type == VariableType::Integer)
        {
            value_str = (symbol.symbol_type == SymbolType::Constant) ? std::to_string(static_cast<int>(symbol.value)) : "-";
        }
        else
        {
            value_str = (symbol.symbol_type == SymbolType::Constant) ? std::to_string(symbol.value) : "-";
        }

        const std::string offset_str = (symbol.symbol_type == SymbolType::Variable) ? std::to_string(symbol.offset) : "-";
        symtable.add_row({symbol.id, symbol_type_to_str(symbol.symbol_type), var_type_to_str(symbol.var_type), value_str, offset_str});
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
