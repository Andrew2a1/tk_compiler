#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseRelationOperations : public ::testing::Test
{
};

TEST_F(ParseRelationOperations, ParsesEqualsOperator)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "begin\n"
        "y := x=1\n"
        "end.\n");

    const std::string expected =
        "je.i 0,#1,#L0\n"
        "mov.i #0,8\n"
        "jump.i #L1\n"
        "L0:\n"
        "mov.i #1,8\n"
        "L1:\n"
        "mov.i 8,4\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRelationOperations, ParsesEqualsOperatorWithRealNumbers)  // No conversion, bug in web compiler?
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "var y: real;\n"
        "begin\n"
        "x := y=x\n"
        "end.\n");

    const std::string expected =
        "je.r 4,0,#L0\n"
        "mov.i #0,12\n"
        "jump.i #L1\n"
        "L0:\n"
        "mov.i #1,12\n"
        "L1:\n"
        "mov.i 12,0\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRelationOperations, ParsesLessEqualsOperator)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "var g: real;\n"
        "begin\n"
        "g := x<=4\n"
        "end.\n");

    const std::string expected =
        "jle.i 0,#4,#L0\n"
        "mov.i #0,12\n"
        "jump.i #L1\n"
        "L0:\n"
        "mov.i #1,12\n"
        "L1:\n"
        "inttoreal.i 12,16\n"
        "mov.r 16,4\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseRelationOperations, ParsesNotEqualOperatorForRealNumbers)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: real;\n"
        "begin\n"
        "y := x<>4.0\n"
        "end.\n");

    const std::string expected =
        "jne.r 0,#4,#L0\n"
        "mov.i #0,16\n"
        "jump.i #L1\n"
        "L0:\n"
        "mov.i #1,16\n"
        "L1:\n"
        "inttoreal.i 16,20\n"
        "mov.r 20,8\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}