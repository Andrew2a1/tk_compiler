#pragma once

#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "parser.h"
#include "scanner.h"
#include "symbol_table.h"

class Driver
{
public:
    SymbolTable symbol_table;
    yy::location location;
    std::string filename;

    bool trace_scanning = false;
    bool trace_parsing = false;

private:
    std::ostream& output_stream;
    std::istream& input_stream;

public:
    explicit Driver(std::ostream& output, std::istream& input);
    virtual ~Driver() = default;

    int parse();

    void set_debug_output(bool trace_scanning = false, bool trace_parsing = false);
    void set_location_filename(std::string& filename);

    void gencode(const std::string& code);
    void gencode(const std::string& code, int op1);
    void gencode(const std::string& code, int op1, int op2);
    void gencode(const std::string& code, int op1, int op2, int op3);
};
