#include "driver.h"

#include "parser.h"

Driver::Driver(bool trace_scanning, bool trace_parsing) : trace_scanning(trace_scanning), trace_parsing(trace_parsing) {}

int Driver::parse(const std::string& input_filename, const std::string& output_filename)
{
    std::ofstream fout;

    filename = input_filename;
    location.initialize(&filename);

    if (output_filename.empty() || output_filename == "-")
    {
        output = &std::cout;
    }
    else
    {
        fout.open(output_filename, std::ios::out);
        if (fout.bad())
        {
            throw std::runtime_error("Cannot open file: " + output_filename + " for writing");
        }
        output = &fout;
    }

    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    output = nullptr;
    return res;
}

void Driver::gencode(const std::string& code) { *output << code << std::endl; }

void Driver::gencode(const std::string& code, int op1)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    *output << code << ' ' << symbol1.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    *output << code << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, int op3)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    const auto& symbol3 = symbol_table.symbols[op3];
    *output << code << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << ',' << symbol3.as_operand() << std::endl;
}
