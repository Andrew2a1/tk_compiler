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

    const std::string expected = "exit\n";

    std::ostringstream output;
    Driver driver(output, input);

    ASSERT_EQ(driver.parse(), 0);
    ASSERT_EQ(output.str(), expected);
}
