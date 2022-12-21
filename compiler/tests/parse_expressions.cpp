#include <driver.h>

#include <filesystem>

#include "gtest/gtest.h"

class ParserParseExpressionsTest : public ::testing::Test
{
};

TEST_F(ParserParseExpressionsTest, CreatesTemporaryVariablesInSymbolTable)
{
    const auto input_filepath = std::filesystem::path{TEST_DATA_DIRECOTRY} / "simple_from_web.txt";

    Driver driver;

    driver.parse(input_filepath);
    ASSERT_EQ(driver.symbol_table.symbols.size(), 7);  // 4 var, 1 const, 2 tmp
}
