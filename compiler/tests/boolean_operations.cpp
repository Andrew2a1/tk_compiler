#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseBooleanOperations : public ::testing::Test
{
};

TEST_F(ParseBooleanOperations, ParseOrAndOperators)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "begin\n"
        "x:=5;\n"
        "y:=x and 1 or x\n"
        "end.\n");

    const std::string expected =
        "mov.i #5,0\n"
        "and.i 0,#1,8\n"
        "or.i 8,0,12\n"
        "mov.i 12,4\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseBooleanOperations, ParseNotOperator)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "begin\n"
        "x:=1 and not y\n"
        "end.\n");

    const std::string expected =
        "not.i 4,8\n"
        "and.i #1,8,12\n"
        "mov.i 12,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseBooleanOperations, NotOperatorIsAlwaysForTypeInt)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: real;\n"
        "begin\n"
        "x:=1 and not y\n"
        "end.\n");

    const std::string expected =
        "not.i 8,16\n"
        "and.i #1,16,20\n"
        "inttoreal.i 20,24\n"
        "mov.r 24,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
