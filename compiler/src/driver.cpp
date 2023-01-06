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
    output_stream << code << symbol1.instr_type_postfix() << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, int op3)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    const auto& symbol3 = symbol_table.symbols[op3];

    assert(symbol1.var_type == symbol2.var_type);
    assert(symbol2.var_type == symbol3.var_type);

    output_stream << code << symbol1.instr_type_postfix() << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << ',' << symbol3.as_operand()
                  << std::endl;
}

int Driver::gencode_conversions(const std::string& code, int op1, int op2)
{
    const auto& expr = symbol_table.symbols[op1];
    const auto& term = symbol_table.symbols[op2];

    if (expr.var_type != term.var_type)
    {
        const int conversion_var = symbol_table.add_tmp(VariableType::Real);
        const int result_var = symbol_table.add_tmp(VariableType::Real);

        if (expr.var_type == VariableType::Integer && term.var_type == VariableType::Real)
        {
            gencode("inttoreal", op1, conversion_var);
            gencode(code, conversion_var, op2, result_var);
        }
        else if (expr.var_type == VariableType::Real && term.var_type == VariableType::Integer)
        {
            gencode("inttoreal", op2, conversion_var);
            gencode(code, op1, conversion_var, result_var);
        }
        return result_var;
    }
    const int result_var = symbol_table.add_tmp(expr.var_type);
    gencode(code, op1, op2, result_var);
    return result_var;
}

bool Driver::assert_int_operands(const std::string& expr, int op1, int op2) const
{
    const auto& s1 = symbol_table.symbols[op1];
    const auto& s2 = symbol_table.symbols[op2];

    if (s1.var_type != VariableType::Integer || s2.var_type != VariableType::Integer)
    {
        throw std::runtime_error("Invalid operand type for '" + expr + "' expression.");
    }
}
