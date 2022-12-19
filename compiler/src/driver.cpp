#include "driver.h"

#include "parser.h"

int Driver::parse(const std::string &filename)
{
    this->filename = filename;
    location.initialize(&this->filename);

    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    return res;
}
