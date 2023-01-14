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
    if (is_in_local_mode) return local_symbol_table;
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

SymbolTableEntry* Driver::find_symbol(const std::string& name)
{
    if (is_in_local_mode)
    {
        const auto& local_symbol = local_symbol_table.find_symbol(name);
        if (local_symbol != nullptr)
        {
            return local_symbol;
        }
    }
    return global_symbol_table.find_symbol(name);
}

void Driver::gencode(const std::string& code) { *output_stream << code << std::endl; }

void Driver::gencode(const std::string& code, const SymbolTableEntry& op1, bool generate_instr_postfix)
{
    geninstr(code, op1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << op1.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, bool generate_instr_postfix)
{
    geninstr(code, op1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << op1.as_operand() << ',' << op2.as_operand() << std::endl;
}

void Driver::gencode(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2, const SymbolTableEntry& op3,
                     bool generate_instr_postfix)
{
    geninstr(code, op1.var_type.type, generate_instr_postfix);
    *output_stream << ' ' << op1.as_operand() << ',' << op2.as_operand() << ',' << op3.as_operand() << std::endl;
}

void Driver::gencode_push(SymbolTableEntry& op)
{
    assert(op.symbol_type == SymbolType::Variable);
    if (op.var_type.is_reference)
    {
        op.var_type.is_reference = false;
        gencode("push", op);
        op.var_type.is_reference = true;
    }
    else
    {
        auto& offset_entry = symbol_table().add_constant(op.offset);
        offset_entry.is_local = op.is_local;
        gencode("push", offset_entry);
    }
}

void Driver::enter_function_mode(const SymbolTableEntry& function_entry_idx)
{
    is_in_local_mode = true;

    // Clear local output
    local_output.str(std::string());
    local_symbol_table = SymbolTable{global_symbol_table.label_count, true};

    const auto& function_info = function_entry_idx.function_info;
    int offset = -4 * (function_info.arguments.size() + (function_info.return_type.has_value() ? 1 : 0) + 1);
    for (const auto& [arg_name, arg_type] : function_info.arguments)
    {
        if (local_symbol_table.find_symbol(arg_name) != nullptr)
        {
            error("Symbol: '" + arg_name + "' has already been declarated.");
        }

        auto type = arg_type;
        type.is_reference = true;
        local_symbol_table.create_variable(arg_name, type, offset);
        offset += 4;
    }

    if (function_info.return_type.has_value())
    {
        const auto& return_type = function_info.return_type.value();
        const auto& function_name = function_entry_idx.id;

        if (local_symbol_table.find_symbol(function_name) != nullptr)
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

    is_in_local_mode = false;
    output_stream = &global_output;

    const auto& local_var_size_const = symbol_table().add_constant(local_symbol_table.get_total_var_size());
    gencode("enter", local_var_size_const);

    global_symbol_table.label_count = local_symbol_table.label_count;
    *output_stream << local_output.str();
}

void Driver::genlabel(const SymbolTableEntry& label) { *output_stream << label.id << ':' << std::endl; }

SymbolTableEntry& Driver::convert_if_needed(SymbolTableEntry& argument, VariableType target_type)
{
    if (argument.var_type.type == VariableType::Integer && target_type == VariableType::Real)
    {
        auto& conversion_tmp = symbol_table().add_tmp(VariableType::Real);
        gencode("inttoreal", argument, conversion_tmp);
        return conversion_tmp;
    }
    else if (argument.var_type.type == VariableType::Real && target_type == VariableType::Integer)
    {
        auto& conversion_tmp = symbol_table().add_tmp(VariableType::Integer);
        gencode("realtoint", argument, conversion_tmp);
        return conversion_tmp;
    }
    return argument;
}

SymbolTableEntry* Driver::genfunc_call(const SymbolTableEntry& function_entry, const std::vector<SymbolTableEntry*>& arguments)
{
    const SymbolTableEntry* valid_entry = &function_entry;
    {
        const unsigned expected_arg_count = function_entry.function_info.arguments.size();
        if (function_entry.symbol_type != SymbolType::Function)
        {
            const auto* found_valid_entry = global_symbol_table.find_symbol(function_entry.id);
            if (found_valid_entry == nullptr)
            {
                error("Identifier: '" + function_entry.id + "' is not a function.");
            }
            valid_entry = found_valid_entry;
        }
        else if (expected_arg_count != arguments.size())
        {
            error("Invalid number of arguments: '" + std::to_string(arguments.size()) + "' (expected: '" + std::to_string(expected_arg_count) + "')");
        }
    }

    int pos = 0;
    auto function_arg_iter = (*valid_entry).function_info.arguments.begin();
    for (auto arguments_iter = arguments.begin(); arguments_iter != arguments.end(); ++arguments_iter, ++function_arg_iter, ++pos)
    {
        auto& arg = *(*arguments_iter);
        const auto& function_arg_type = (*function_arg_iter).second;

        if (function_arg_type.is_array() && arg.var_type.is_array())
        {
            if (!(function_arg_type == arg.var_type))
            {
                error("Invalid parameter. Array types are different at pos: " + std::to_string(pos));
            }
            gencode_push(arg);
        }
        else if (!function_arg_type.is_array() && !arg.var_type.is_array())
        {
            if (arg.symbol_type == SymbolType::Constant)
            {
                const auto& arg_const = symbol_table().add_constant(arg.value, function_arg_type.type);  // Ensure constant is of right type
                auto& tmp_var = symbol_table().add_tmp(function_arg_type.type);
                gencode("mov", arg_const, tmp_var);
                gencode_push(tmp_var);
            }
            else
            {
                auto& converted_arg = convert_if_needed(arg, function_arg_type.type);
                gencode_push(converted_arg);
            }
        }
        else
        {
            error("Invalid parameter type at position: " + std::to_string(pos));
        }
    }

    SymbolTableEntry* result_var = nullptr;
    int pushed_size = arguments.size() * 4;

    const auto& function_info = (*valid_entry).function_info;
    if (function_info.return_type.has_value())
    {
        result_var = &symbol_table().add_tmp(function_info.return_type.value().type);
        gencode_push(*result_var);
        pushed_size += 4;
    }

    const auto& pushed_size_const = symbol_table().add_constant(pushed_size);
    gencode("call", (*valid_entry));

    if (pushed_size > 0)
    {
        gencode("incsp", pushed_size_const);
    }

    return result_var;
}

SymbolTableEntry& Driver::genarray_get(SymbolTableEntry& function_entry, const SymbolTableEntry& expr)
{
    const auto& expression_type = expr.var_type.type;
    std::reference_wrapper<const SymbolTableEntry> array_idx = expr;

    if (expression_type == VariableType::Real)
    {
        const auto& conversion_var = symbol_table().add_tmp(VariableType::Integer);
        gencode("realtoint", expr, conversion_var);
        array_idx = conversion_var;
    }

    if (!function_entry.var_type.is_array())
    {
        error("Id: " + function_entry.id + " is not an array");
    }

    const auto& array_var_type = function_entry.var_type;
    const auto& array_info = array_var_type.type_info.value();

    const auto& address_tmp = symbol_table().add_tmp(VariableType::Integer);
    auto& output_ref_var = symbol_table().add_tmp(VariableType::Integer);
    const auto& start_index_const = symbol_table().add_constant(array_info.start_index);
    const auto& type_size_const = symbol_table().add_constant(symbol_table().type_size(array_var_type.type));

    const bool is_reference_type = function_entry.var_type.is_reference;
    std::reference_wrapper<const SymbolTableEntry> var_offset = function_entry;

    if (is_reference_type)
        function_entry.var_type.is_reference = false;
    else
        var_offset = symbol_table().add_constant(symbol_table().get_symbol_offset(function_entry));

    gencode("sub", array_idx, start_index_const, address_tmp);
    gencode("mul", address_tmp, type_size_const, address_tmp);
    gencode("add", var_offset, address_tmp, output_ref_var);

    symbol_table().to_ref(output_ref_var, array_var_type.type);
    function_entry.var_type.is_reference = is_reference_type;
    return output_ref_var;
}

SymbolTableEntry& Driver::gencode_conversions(const std::string& code, const SymbolTableEntry& op1, const SymbolTableEntry& op2)
{
    const auto& s1_type = op1.var_type.type;
    const auto& s2_type = op2.var_type.type;

    if (s1_type != s2_type)
    {
        const auto& conversion_var = symbol_table().add_tmp(VariableType::Real);
        auto& result_var = symbol_table().add_tmp(VariableType::Real);

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
    auto& result_var = symbol_table().add_tmp(s1_type);
    gencode(code, op1, op2, result_var);
    return result_var;
}

SymbolTableEntry& Driver::gencode_relop(const std::string& relop_code, const SymbolTableEntry& op1, const SymbolTableEntry& op2)
{
    const auto& true_label = symbol_table().add_label();
    const auto& end_label = symbol_table().add_label();
    auto& result_var = symbol_table().add_tmp(VariableType::Integer);
    const auto& zero_const = symbol_table().add_constant(0);
    const auto& one_const = symbol_table().add_constant(1);

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
