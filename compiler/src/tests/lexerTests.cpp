#include <gtest/gtest.h>
#include <unordered_map>

#include "../lexy.h"
#include "../token.h"

TEST(Lexer, GetTokensFromFile_InvalidFile) {
  std::string filename = "fakeFile.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  EXPECT_FALSE(validToks);
}

TEST(Lexer, GetTokensFromFile_ValidFile_AllTokens) {
  std::string filename = "../src/tests/programs/lexerAllTokenTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  EXPECT_TRUE(validToks);

  std::unordered_map<int, std::string> tokenMap = {
      // start at line 2 because of the comment
      {2, "u8"},    {3, "void"},     {4, ":="},        {5, "+"},
      {6, "-"},     {7, "<<"},       {8, ">>"},        {9, ">"},
      {10, "<"},    {11, ">="},      {12, "<="},       {13, "=="},
      {14, "!="},   {15, ";"},       {16, ":"},        {17, ","},
      {18, "("},    {19, ")"},       {20, "{"},        {21, "}"},
      {22, "if"},   {23, "then"},    {24, "else"},     {25, "endif"},
      {26, "loop"}, {27, "endloop"}, {28, "variable"}, {29, "variable2"},
      {30, "1"}};

  int line = 1;
  for (const auto &tok : tokens) {
    line++;
    EXPECT_EQ(tok.GetLine(), line);

    EXPECT_FALSE(tok.GetType() == TokenType::Unknown);

    EXPECT_EQ(tok.GetValue(), tokenMap[line]);
  }
}