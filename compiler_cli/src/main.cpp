#include <driver.h>

#include <CLI/CLI.hpp>
#include <clocale>
#include <filesystem>
#include <tabulate/table.hpp>

void print_symbol_table(const SymbolTable &table);

int main(int argc, char *argv[])
{
    std::filesystem::path input_filename, output_filename;
    bool trace_scanning = false;
    bool trace_parsing = false;

    CLI::App app("Compiler project for subset of Pascal language", argv[0]);
    app.add_option("input_filename", input_filename, "Input filename with code to compile (empty or '-' for stdin)");
    app.add_option("output_filename", output_filename, "Output filename with generated assembly code (empty or '-' for stdout)");
    app.add_flag("-s,--trace_scanning", trace_scanning, "Show debug output from scanner");
    app.add_flag("-p,--trace_parsing", trace_parsing, "Show debug output from parser");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e)
    {
        return app.exit(e);
    }

    Driver driver(trace_scanning, trace_parsing);
    int result = driver.parse(input_filename, output_filename);

    if (result != 0)
    {
        std::cerr << "Compilation failed." << std::endl;
    }
    else
    {
        print_symbol_table(driver.symbol_table);
    }

    std::locale::global(std::locale("C"));
    return result;
}

std::string var_type_to_str(VariableType var_type)
{
    switch (var_type)
    {
        case VariableType::Integer:
            return "integer";
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
    }
    return "UNKNOWN";
}

void print_symbol_table(const SymbolTable &table)
{
    tabulate::Table symtable;
    symtable.add_row({"ID", "Type", "Var Type", "Value", "Offset"});

    for (const auto &symbol : table.symbols)
    {
        const std::string value_str = (symbol.symbol_type == SymbolType::Constant) ? std::to_string(symbol.value) : "-";
        const std::string offset_str = (symbol.symbol_type != SymbolType::Constant) ? std::to_string(symbol.offset) : "-";
        symtable.add_row({symbol.id, symbol_type_to_str(symbol.symbol_type), var_type_to_str(symbol.var_type), value_str, offset_str});
    }

    std::cout << symtable << std::endl;
}