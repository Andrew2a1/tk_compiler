#include "driver.h"

#include "parser.h"

Driver::Driver(bool trace_scanning, bool trace_parsing) : trace_scanning(trace_scanning), trace_parsing(trace_parsing) {}

int Driver::parse(const std::string& input_filename, const std::string& output_filename)
{
    filename = input_filename;
    location.initialize(&filename);

    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    return res;
}
