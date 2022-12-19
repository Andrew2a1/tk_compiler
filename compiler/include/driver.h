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
    std::string result;

    bool trace_scanning = false;
    bool trace_parsing = false;

public:
    virtual ~Driver();
    int parse(const std::string& filename);

private:
    void scan_begin();
    void scan_end();
};