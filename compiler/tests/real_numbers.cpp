#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseRealNumbersTest : public ::testing::Test
{
};

TEST_F(ParseRealNumbersTest, SupportsRealVariableDeclaration)
{
    std::istringstream input(
        "program example(input);\n"
        "var a, b: real;"
        "begin\n"
        "end.\n");

    const std::string expected = "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(driver.symbol_table.symbols.size(), 2);
    ASSERT_EQ(driver.symbol_table.symbols.front().var_type, VariableType::Real);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRealNumbersTest, GeneratesAssignmentCode)
{
    std::istringstream input(
        "program example(input);\n"
        "var a, b: real;"
        "begin\n"
        "a:=1.0\n"
        "end.\n");

    const std::string expected =
        "mov.r #1,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}