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
        "var a, b: real;\n"
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
        "var a, b: real;\n"
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

TEST_F(ParseRealNumbersTest, ConvertsRealConstantToIntOnAssignment)
{
    std::istringstream input(
        "program example(input);\n"
        "var a: integer;\n"
        "begin\n"
        "a:=1.5\n"
        "end.\n");

    const std::string expected =
        "realtoint.r #1.5,4\n"
        "mov.i 4,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRealNumbersTest, ConvertsIntConstantToRealOnAssignment)
{
    std::istringstream input(
        "program example(input);\n"
        "var a: real;\n"
        "var b: integer;\n"
        "begin\n"
        "a:=b\n"
        "end.\n");

    const std::string expected =
        "inttoreal.i 8,12\n"
        "mov.r 12,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}