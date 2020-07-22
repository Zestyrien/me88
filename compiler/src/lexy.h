#pragma once

#include <vector>
#include <string>
#include <tuple>
class Token;

namespace Lexer 
{
	std::tuple<bool, std::vector<Token>> GetTokensFromFile(const std::string &filename);
	void PrintTokens(const std::vector<Token>& tokens);
}