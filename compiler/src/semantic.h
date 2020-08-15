#pragma once

#include "tree.h"
#include "symbolstable.h"
#include <tuple>

std::tuple<bool, SymbolsTable> AnalyzeSemantic(const Tree& tree,
                                               bool debug = false);
