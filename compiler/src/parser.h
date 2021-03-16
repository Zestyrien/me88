#pragma once

#include "symbolstable.h"
#include "tree.h"
#include <bitset>
#include <vector>

namespace Parser {

std::vector<std::string> ParseIR(std::string const &filename,
                                 Tree const &program,
                                 SymbolsTable const &symbols);

std::vector<std::bitset<8>>
ParseMachineCode(std::vector<std::string> const &ir);
} // namespace Parser