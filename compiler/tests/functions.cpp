#include <driver.h>

#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

class ParseFunctions : public ::testing::Test
{
};

TEST_F(ParseFunctions, ReturnValueShouldGenerateMovInstruction)
{
    std::istringstream input(
        "program example(input, output);\n"
        "var x, y: integer;\n"
        "var g,h:real;\n"
        "\n"
        "function a(b: integer): integer;\n"
        "begin\n"
        "a := b\n"
        "end; \n"
        "\n"
        "begin\n"
        "end.\n");

    const std::string expected =
        "jump.i #L0\n"
        "a:\n"
        "enter.i #0\n"
        "mov.i *BP+12,*BP+8\n"
        "leave\n"
        "return\n"
        "L0:\n"
        "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
