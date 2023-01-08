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

void Driver::gencode(const std::string& code, int op1, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table.symbols[op1];

    geninstr(code, symbol1.var_type, generate_instr_postfix);
    output_stream << ' ' << symbol1.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];

    geninstr(code, symbol1.var_type, generate_instr_postfix);
    output_stream << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, int op3, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table.symbols[op1];
    const auto& symbol2 = symbol_table.symbols[op2];
    const auto& symbol3 = symbol_table.symbols[op3];

    geninstr(code, symbol1.var_type, generate_instr_postfix);
    output_stream << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << ',' << symbol3.as_operand() << std::endl;
}

int Driver::gencode_conversions(const std::string& code, int op1, int op2)
{
    const auto s1_type = symbol_table.symbols[op1].var_type;
    const auto s2_type = symbol_table.symbols[op2].var_type;

    if (s1_type != s2_type)
    {
        const int conversion_var = symbol_table.add_tmp(VariableType::Real);
        const int result_var = symbol_table.add_tmp(VariableType::Real);

        if (s1_type == VariableType::Integer && s2_type == VariableType::Real)
        {
            gencode("inttoreal", op1, conversion_var);
            gencode(code, conversion_var, op2, result_var);
        }
        else if (s1_type == VariableType::Real && s2_type == VariableType::Integer)
        {
            gencode("inttoreal", op2, conversion_var);
            gencode(code, op1, conversion_var, result_var);
        }
        return result_var;
    }
    const int result_var = symbol_table.add_tmp(s1_type);
    gencode(code, op1, op2, result_var);
    return result_var;
}

int Driver::gencode_relop(const std::string& relop_code, int op1, int op2)
{
    const int true_label = symbol_table.add_label();
    const int end_label = symbol_table.add_label();
    const int result_var = symbol_table.add_tmp(VariableType::Integer);
    const int zero_const = symbol_table.add_constant(0);
    const int one_const = symbol_table.add_constant(1);

    gencode(relop_code, op1, op2, true_label);
    gencode("mov", zero_const, result_var);
    gencode("jump", end_label);
    gencode(symbol_table.symbols[true_label].id + ":");
    gencode("mov", one_const, result_var);
    gencode(symbol_table.symbols[end_label].id + ":");

    return result_var;
}

void Driver::geninstr(const std::string& code, VariableType instr_var_type, bool generate_instr_postfix)
{
    output_stream << code;
    if (generate_instr_postfix)
    {
        output_stream << instr_postfix(instr_var_type);
    }
}
