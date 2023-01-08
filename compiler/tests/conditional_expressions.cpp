#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseConditionalExpressions : public ::testing::Test
{
};

TEST_F(ParseConditionalExpressions, ParsesIfThenElseStatement)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x: integer;\n"
        "begin\n"
        "if x then\n"
        "  write(1)\n"
        "else\n"
        "  write(0)\n"
        "end.\n");

    const std::string expected =
        "je.i 0,#0,#L0\n"
        "write.i #1\n"
        "jump.i #L1\n"
        "L0:\n"
        "write.i #0\n"
        "L1:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}

TEST_F(ParseConditionalExpressions, ParsesNestedConditionalStatements)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "begin\n"
        "if x then\n"
        "  if y=0 then\n"
        "    write(2)\n"
        "  else\n"
        "    write(1)\n"
        "else\n"
        "  write(0)\n"
        "end.\n");

    const std::string expected =
        "je.i 0,#0,#L0\n"
        "je.i 4,#0,#L1\n"
        "mov.i #0,8\n"
        "jump.i #L2\n"
        "L1:\n"
        "mov.i #1,8\n"
        "L2:\n"
        "je.i 8,#0,#L3\n"
        "write.i #2\n"
        "jump.i #L4\n"
        "L3:\n"
        "write.i #1\n"
        "L4:\n"
        "jump.i #L5\n"
        "L0:\n"
        "write.i #0\n"
        "L5:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
