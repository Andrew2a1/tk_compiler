#include "driver.h"

#include "parser.h"
#include "scanner.h"

Driver::Driver(std::ostream& output, std::istream& input)
    : scanner(*this), parser(scanner, *this), output_stream(&output), input_stream(input), global_output(output)
{
}

int Driver::parse()
{
    scanner.switch_streams(&input_stream, output_stream);
    scanner.set_debug(trace_scanning);
    parser.set_debug_level(trace_parsing);
    return parser();
}

SymbolTable& Driver::symbol_table()
{
    if (subprogram_symbol_index > 0) return local_symbol_table;
    return global_symbol_table;
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

void Driver::error(const std::string& message)
{
    const yy::parser::syntax_error syntax_error{location, message};
    parser.error(syntax_error);
    throw std::runtime_error(message);
}

void Driver::gencode(const std::string& code) { *output_stream << code << std::endl; }

void Driver::gencode(const std::string& code, int op1, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table().symbols[op1];

    geninstr(code, symbol1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << symbol1.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table().symbols[op1];
    const auto& symbol2 = symbol_table().symbols[op2];

    geninstr(code, symbol1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, int op1, int op2, int op3, bool generate_instr_postfix)
{
    const auto& symbol1 = symbol_table().symbols[op1];
    const auto& symbol2 = symbol_table().symbols[op2];
    const auto& symbol3 = symbol_table().symbols[op3];

    geninstr(code, symbol1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << symbol1.as_operand() << ',' << symbol2.as_operand() << ',' << symbol3.as_operand() << std::endl;
}

void Driver::enter_function_mode(int function_entry_idx)
{
    subprogram_symbol_index = function_entry_idx;
    local_output.str(std::string());  // Clear local output
    local_symbol_table = SymbolTable{true};

    const auto& function_entry = global_symbol_table.symbols[function_entry_idx].function_info;
    int offset = -4 * (function_entry.arguments.size() + (function_entry.return_type.has_value() ? 1 : 0) + 1);
    for (const auto& [arg_name, arg_type] : function_entry.arguments)
    {
        if (local_symbol_table.find_symbol(arg_name) > 0)
        {
            error("Symbol: '" + arg_name + "' has already been declarated.");
        }

        auto type = arg_type;
        type.is_reference = true;
        local_symbol_table.create_variable(arg_name, type, offset);
        offset += 4;
    }

    if (function_entry.return_type.has_value())
    {
        const auto& return_type = function_entry.return_type.value();
        const auto& function_name = global_symbol_table.symbols[function_entry_idx].id;

        if (local_symbol_table.find_symbol(function_name) > 0)
        {
            error("Symbol: '" + function_name + "' has already been declarated.");
        }

        auto type = return_type;
        type.is_reference = true;

        local_symbol_table.create_variable(function_name, type, offset);
    }

    local_symbol_table.set_var_offset(4);
    output_stream = &local_output;
}

void Driver::leave_function_mode()
{
    gencode("leave");
    gencode("return");

    subprogram_symbol_index = -1;
    output_stream = &global_output;

    const int local_var_size_const = symbol_table().add_constant(local_symbol_table.get_total_var_size());
    gencode("enter", local_var_size_const);

    *output_stream << local_output.str();
}

void Driver::genlabel(int label) { *output_stream << global_symbol_table.symbols[label].id << ':' << std::endl; }

int Driver::convert_if_needed(int argument, VariableType target_type)
{
    if (symbol_table().symbols[argument].var_type.type == VariableType::Integer && target_type == VariableType::Real)
    {
        const int conversion_tmp = symbol_table().add_tmp(VariableType::Real);
        gencode("inttoreal", argument, conversion_tmp);
        return conversion_tmp;
    }
    else if (symbol_table().symbols[argument].var_type.type == VariableType::Real && target_type == VariableType::Integer)
    {
        const int conversion_tmp = symbol_table().add_tmp(VariableType::Integer);
        gencode("realtoint", argument, conversion_tmp);
        return conversion_tmp;
    }
    return argument;
}

int Driver::genfunc_call(int function_idx, const std::vector<int>& arguments)
{
    {
        const auto& function_entry = symbol_table().symbols[function_idx];
        const unsigned expected_arg_count = function_entry.function_info.arguments.size();
        if (function_entry.symbol_type != SymbolType::Function)
        {
            error("Identifier: '" + function_entry.id + "' is not a function.");
        }
        else if (expected_arg_count != arguments.size())
        {
            error("Invalid number of arguments: '" + std::to_string(arguments.size()) + "' (expected: '" + std::to_string(expected_arg_count) + "')");
        }
    }

    for (unsigned i = 0; i < arguments.size(); ++i)
    {
        const auto arg = symbol_table().symbols[arguments[i]];
        const auto function_arg_type = symbol_table().symbols[function_idx].function_info.arguments[i].second;
        int arg_address;

        if (function_arg_type.is_array() && arg.var_type.is_array())
        {
            if (!(function_arg_type == arg.var_type))
            {
                error("Invalid parameter. Array types are different at pos: " + std::to_string(i));
            }
            arg_address = symbol_table().add_constant(symbol_table().get_symbol_offset(arguments[i]));
        }
        else if (!function_arg_type.is_array() && !arg.var_type.is_array())
        {
            if (arg.symbol_type == SymbolType::Constant)
            {
                const int arg_const = symbol_table().add_constant(arg.value, function_arg_type.type);  // Ensure constant is of right type
                const int tmp_var = symbol_table().add_tmp(function_arg_type.type);
                gencode("mov", arg_const, tmp_var);
                arg_address = symbol_table().add_constant(symbol_table().get_symbol_offset(tmp_var));
            }
            else
            {
                const int converted_arg = convert_if_needed(arguments[i], function_arg_type.type);
                arg_address = symbol_table().add_constant(symbol_table().get_symbol_offset(converted_arg));
            }
        }
        else
        {
            error("Invalid parameter type at position: " + std::to_string(i));
        }
        gencode("push", arg_address);
    }

    int result_var = -1;
    int pushed_size = arguments.size() * 4;

    const auto& function_info = symbol_table().symbols[function_idx].function_info;
    if (function_info.return_type.has_value())
    {
        result_var = symbol_table().add_tmp(function_info.return_type.value().type);
        const int result_address = symbol_table().add_constant(symbol_table().get_symbol_offset(result_var));
        gencode("push", result_address);
        pushed_size += 4;
    }

    const int pushed_size_const = symbol_table().add_constant(pushed_size);
    gencode("call", function_idx);

    if (pushed_size > 0)
    {
        gencode("incsp", pushed_size_const);
    }

    return result_var;
}

int Driver::gencode_conversions(const std::string& code, int op1, int op2)
{
    const auto s1_type = symbol_table().symbols[op1].var_type.type;
    const auto s2_type = symbol_table().symbols[op2].var_type.type;

    if (s1_type != s2_type)
    {
        const int conversion_var = symbol_table().add_tmp(VariableType::Real);
        const int result_var = symbol_table().add_tmp(VariableType::Real);

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
    const int result_var = symbol_table().add_tmp(s1_type);
    gencode(code, op1, op2, result_var);
    return result_var;
}

int Driver::gencode_relop(const std::string& relop_code, int op1, int op2)
{
    const int true_label = global_symbol_table.add_label();
    const int end_label = global_symbol_table.add_label();
    const int result_var = symbol_table().add_tmp(VariableType::Integer);
    const int zero_const = symbol_table().add_constant(0);
    const int one_const = symbol_table().add_constant(1);

    gencode(relop_code, op1, op2, true_label);
    gencode("mov", zero_const, result_var);
    gencode("jump", end_label);
    genlabel(true_label);
    gencode("mov", one_const, result_var);
    genlabel(end_label);

    return result_var;
}

void Driver::geninstr(const std::string& code, VariableType instr_type, bool generate_instr_postfix)
{
    *output_stream << code;
    if (generate_instr_postfix)
    {
        *output_stream << instr_postfix(instr_type);
    }
}
