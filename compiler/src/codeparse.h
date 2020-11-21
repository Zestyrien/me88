#pragma once

#include "symbolstable.h"
#include "tree.h"
#include <bitset>
#include <vector>

std::vector<std::bitset<8>> GenerateCode(const Tree &program,
                                         const SymbolsTable &symbols);
