#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseWhileLoop : public ::testing::Test
{
};

TEST_F(ParseWhileLoop, ParsesWhileLoopWithSingleStatement)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "begin\n"
        "while 0 do x:=0\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "L1:\n"
        "je.i #0,#0,#L2\n"
        "mov.i #0,0\n"
        "jump.i #L1\n"
        "L2:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseWhileLoop, ParsesWhileWithMultipleStatements)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "begin\n"
        "while x<10 do\n"
        "begin\n"
        "  x:=x+1;\n"
        "  write(x)\n"
        "end\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "L0:\n"
        "L1:\n"
        "jl.i 0,#10,#L3\n"
        "mov.i #0,4\n"
        "jump.i #L4\n"
        "L3:\n"
        "mov.i #1,4\n"
        "L4:\n"
        "je.i 4,#0,#L2\n"
        "add.i 0,#1,8\n"
        "mov.i 8,0\n"
        "write.i 0\n"
        "jump.i #L1\n"
        "L2:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
