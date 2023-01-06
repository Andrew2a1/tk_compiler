#include "driver.h"

#include "parser.h"
#include "scanner.h"

Driver::Driver(std::ostream& output, std::istream& input) : output_stream(output), input_stream(input) {}

int Driver::parse()
{
    Scanner scanner(*this);
    scanner.switch_streams(&input_stream, &output_stream);
    scanner.set_debug(trace_scanning);

    yy::parser parse(scanner, *this);
    parse.set_debug_level(trace_parsing);
    return parse();
}

void Driver::set_debug(bool trace_scanning, bool trace_parsing)
{
    this->trace_parsing = trace_parsing;
    this->trace_scanning = trace_scanning;
}

void Driver::set_location_filename(const std::string& filename)
{
    loc_filename = filename;
    location.initialize(&loc_filename);
}

void Driver::gencode(const std::string& code) { output_stream << code << std::endl; }

void Driver::gencode(const std::string& code, int op1)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    output_stream << code << symbol1.instr_type_postfix() << ' ' << symbol1.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    output_stream << code << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, int op3)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    const auto& symbol3 = symbol_table.symbols[op3];
    output_stream << code << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << ',' << symbol3.as_operand() << std::endl;
}
