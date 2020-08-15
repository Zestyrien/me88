#pragma once

#include <string>
#include <tuple>
#include <vector>
class Token;

namespace Lexer {
std::tuple<bool, std::vector<Token>>
GetTokensFromFile(const std::string &filename);
void PrintTokens(const std::vector<Token> &tokens);
} // namespace Lexer