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

TEST_F(ParseRealNumbersTest, GeneratesValidCodeForUnaryMinusOperator)
{
    std::istringstream input(
        "program example(input);\n"
        "var a, b: real;\n"
        "begin\n"
        "a:=-b\n"
        "end.\n");

    const std::string expected =
        "sub.r #0,8,16\n"
        "mov.r 16,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRealNumbersTest, GeneratesConversionsForSubtraction)
{
    std::istringstream input(
        "program example(input);\n"
        "var a: real;\n"
        "var b: integer;\n"
        "begin\n"
        "a:=12.5-b\n"
        "end.\n");

    const std::string expected =
        "inttoreal.i 8,12\n"
        "sub.r #12.5,12,20\n"
        "mov.r 20,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRealNumbersTest, GeneratesConversionsForMultiplicationAndAddition)
{
    std::istringstream input(
        "program example(input);\n"
        "var a, b: integer;\n"
        "begin\n"
        "a:=2.5 + 12.5*b\n"
        "end.\n");

    const std::string expected =
        "inttoreal.i 4,8\n"
        "mul.r #12.5,8,16\n"
        "add.r #2.5,16,24\n"
        "realtoint.r 24,32\n"
        "mov.i 32,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRealNumbersTest, FullySupportsRealNumbers)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:real;\n"
        "\n"
        "begin\n"
        "read(x,y);\n"
        "h:=1.5;\n"
        "g:=x+y*h;\n"
        "write(g)\n"
        "end.\n");

    const std::string expected =
        "read.i 0\n"
        "read.i 4\n"
        "mov.r #1.5,16\n"
        "inttoreal.i 4,24\n"
        "mul.r 24,16,32\n"
        "inttoreal.i 0,40\n"
        "add.r 40,32,48\n"
        "mov.r 48,8\n"
        "write.r 8\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}