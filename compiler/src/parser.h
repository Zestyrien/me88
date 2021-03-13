#pragma once

#include "symbolstable.h"
#include "tree.h"
#include <bitset>
#include <vector>

namespace Parser {
struct ScopeInfo {

};


std::vector<std::string> ParseIR(std::string const &filename,
                                 Tree const &program,
                                 SymbolsTable const &symbols);
}