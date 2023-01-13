#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParserParseDeclarationsTest : public ::testing::Test
{
};

TEST_F(ParserParseDeclarationsTest, EmptyProgramShouldLeaveEmptySymbolTable)
{
    std::istringstream input(
        "program example(input);\n"
        "begin\n"
        "end.\n");

    Driver driver(std::cout, input);
    ASSERT_EQ(driver.symbol_table.symbols.size(), 0);

    driver.parse();
    ASSERT_EQ(driver.symbol_table.symbols.size(), 1);
}

TEST_F(ParserParseDeclarationsTest, EmptyProgramWithOneVariableDeclaration)
{
    std::istringstream input(
        "program ex(input, output);\n"
        "var a: integer;\n"
        "begin\n"
        "end.\n");

    Driver driver(std::cout, input);
    driver.parse();
    ASSERT_EQ(driver.symbol_table.symbols.size(), 2);
}

TEST_F(ParserParseDeclarationsTest, EmptyProgramWithFourVariableDeclaration)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "var g,h,j:integer;\n"
        "begin\n"
        "end.\n");

    Driver driver(std::cout, input);
    driver.parse();
    ASSERT_EQ(driver.symbol_table.symbols.size(), 5);
}