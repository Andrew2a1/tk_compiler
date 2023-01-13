#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParserParseExpressionsTest : public ::testing::Test
{
};

TEST_F(ParserParseExpressionsTest, CreatesTemporaryVariablesInSymbolTable)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:integer;\n"
        "begin\n"
        "read(x,y);\n"
        "h:=1;\n"
        "g:=x+y*h;\n"
        "write(g)\n"
        "end.\n");

    Driver driver(std::cout, input);
    driver.parse();
    ASSERT_EQ(driver.symbol_table.symbols.size(), 8);
}
