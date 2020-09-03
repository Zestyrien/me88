#pragma once

#include "symbolstable.h"
#include "tree.h"
#include <tuple>

std::tuple<bool, SymbolsTable> AnalyzeSemantic(const Tree &tree);
