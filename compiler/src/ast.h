#pragma once

#include "token.h"
#include "tree.h"
#include <tuple>
#include <vector>

std::tuple<bool, Tree> CreateAST(const std::vector<Token> &tokens);
                                 bool debug = false);
