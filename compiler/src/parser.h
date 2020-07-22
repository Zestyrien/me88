#pragma once
#include <vector>
#include <map>
#include "token.h"
#include "tree.h"
#include "symboltable.h"
#include <tuple>

namespace Parser
{
	std::tuple<bool, Tree> ParseProgram(std::vector<Token>& tokens);
	std::tuple<bool, SymbolTable> GenerateSymbolTable(Tree& tree);
	void PrintTree(const Tree& tree);
}