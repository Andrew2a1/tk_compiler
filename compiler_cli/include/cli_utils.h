#pragma once

#include <driver.h>

#include <filesystem>
#include <string>

std::string var_type_to_str(VariableType var_type);
std::string symbol_type_to_str(SymbolType symbol_type);
void print_symbol_table(const SymbolTable &table);

bool use_console(const std::string &filename);
