#include <driver.h>

#include <filesystem>

#include "gtest/gtest.h"

class ParserParseDeclarationsTest : public ::testing::Test
{
};

TEST_F(ParserParseDeclarationsTest, EmptyProgramShouldLeaveEmptySymbolTable)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simplest.txt";

    Driver driver;
    ASSERT_EQ(driver.symbol_table.symbols.size(), 0);

    driver.parse(input_filepath);
    ASSERT_EQ(driver.symbol_table.symbols.size(), 0);
}

TEST_F(ParserParseDeclarationsTest, EmptyProgramWithOneVariableDeclaration)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "one_var.txt";

    Driver driver;
    driver.parse(input_filepath);

    ASSERT_EQ(driver.symbol_table.symbols.size(), 1);
}

TEST_F(ParserParseDeclarationsTest, EmptyProgramWithFourVariableDeclaration)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "four_var.txt";

    Driver driver;
    driver.parse(input_filepath);

    ASSERT_EQ(driver.symbol_table.symbols.size(), 4);
}