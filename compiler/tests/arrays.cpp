#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseArrays : public ::testing::Test
{
};

TEST_F(ParseArrays, ParsesSyntaxWithArray)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "var y: array[4..2] of integer;\n"
        "var g,h: array[1..10] of real;\n"
        "begin\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanWriteToArrayAtAnyIndex)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[0..10] of integer;\n"
        "begin\n"
        "x[0] := 1\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #0,#0,44\n"
        "mul.i 44,#4,44\n"
        "add.i #0,44,48\n"
        "mov.i #1,*48\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanReadFromArrayAtAnyIndex)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[0..10] of integer;\n"
        "var y: integer;\n"
        "begin\n"
        "y := x[2]\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #2,#0,48\n"
        "mul.i 48,#4,48\n"
        "add.i #0,48,52\n"
        "mov.i *52,44\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanReadFromRealArrayAtAnyIndexWithConversion)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[0..10] of real;\n"
        "var y: integer;\n"
        "begin\n"
        "y := x[2]\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #2,#0,92\n"
        "mul.i 92,#8,92\n"
        "add.i #0,92,96\n"
        "realtoint.r *96,100\n"
        "mov.i 100,88\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanReadFromRealArrayAtAnyIndex)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of real;\n"
        "var y: real;\n"
        "begin\n"
        "y := x[1]\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #1,#1,88\n"
        "mul.i 88,#8,88\n"
        "add.i #0,88,92\n"
        "mov.r *92,80\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanReadIntoTheTable)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "begin\n"
        "read(x[1])\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #1,#1,40\n"
        "mul.i 40,#4,40\n"
        "add.i #0,40,44\n"
        "read.i *44\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, IndexShouldBeConvertedToInteger)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: array[1..10] of integer;\n"
        "begin\n"
        "write(x[-1.0])\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.r #0,#1,40\n"
        "realtoint.r 40,48\n"
        "sub.i 48,#1,52\n"
        "mul.i 52,#4,52\n"
        "add.i #0,52,56\n"
        "write.i *56\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseArrays, CanUseArrayValueAsIndex)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: array[1..10] of integer;\n"
        "begin\n"
        "write(x[y[5]])\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "sub.i #5,#1,80\n"
        "mul.i 80,#4,80\n"
        "add.i #40,80,84\n"
        "sub.i *84,#1,88\n"
        "mul.i 88,#4,88\n"
        "add.i #0,88,92\n"
        "write.i *92\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}