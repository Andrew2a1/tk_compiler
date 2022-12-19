#pragma once

#include <cstdio>
#include <fstream>
#include <map>
#include <string>

#include "parser.h"

#define YY_DECL yy::parser::symbol_type yylex(Driver& drv)
YY_DECL;

class Driver
{
public:
    yy::location location;
    std::string filename;

    bool trace_scanning = false;
    bool trace_parsing = false;

public:
    explicit Driver(bool trace_scanning = false, bool trace_parsing = false);
    virtual ~Driver();

    int parse(const std::string& input_filename = "-", const std::string& output_filename = "-");

private:
    void scan_begin();
    void scan_end();
};