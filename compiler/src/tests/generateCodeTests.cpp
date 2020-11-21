#include <gtest/gtest.h>
#include <unordered_map>

#include "../ast.h"
#include "../codeparse.h"
#include "../lexy.h"
#include "../semantic.h"

TEST(CodeGenerator, Empty) {
  std::string filename = "fakeTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, VariableDeclaration) {
  std::string filename = "../compiler/src/tests/programs/ast/variableDeclarationTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {11, 12, 21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, VariableDeclarationAssignement) {
  std::string filename =
      "../compiler/src/tests/programs/ast/variableDeclarationAssignementTest.F7";

  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {11, 96, 5,  11, 4,  7, 5,  98,
                                              1,  10, 12, 64, 96, 1, 12, 21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, IfNoElse) {
  std::string filename = "../compiler/src/tests/programs/ast/ifNoElse.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {11, 96, 1,  97, 1, 204, 21, 0,
                                              96, 5,  11, 4,  7, 5,   98, 1,
                                              10, 12, 64, 96, 1, 12,  21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, IfElse) {
  std::string filename = "../compiler/src/tests/programs/ast/ifElse.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {
      11, 11, 96, 1,  97, 1,  204, 25, 0,   96, 5,  11, 4, 7,
      5,  98, 1,  10, 12, 64, 96,  1,  192, 38, 0,  96, 7, 11,
      4,  7,  5,  98, 2,  10, 12,  64, 96,  1,  12, 12, 21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, While) {
  std::string filename = "../compiler/src/tests/programs/ast/while.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {
      11, 96, 1, 97, 1,  204, 24, 0, 96,  5, 11, 4,  7,
      5,  98, 1, 10, 12, 64,  96, 1, 192, 1, 0,  12, 21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, EmptyVoid) {
  std::string filename = "../compiler/src/tests/programs/ast/emptyVoid.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {192, 4, 0, 18, 211, 3, 0, 21};

  EXPECT_EQ(machineCode, expectedCode);
}

TEST(CodeGenerator, VoidWithArgs) {
  std::string filename = "../compiler/src/tests/programs/ast/voidWithArgs.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto [validProg, AST] = CreateAST(tokens);
  auto [validSemantic, symbols] = AnalyzeSemantic(AST);

  auto machineCode = GenerateCode(AST, symbols);

  std::vector<std::bitset<8>> expectedCode = {
      11, 11, 192, 6,  0,  18, 96, 5,   11, 4,  7,  5,  98, 1,  10,
      12, 64, 96,  1,  96, 7,  11, 4,   7,  5,  98, 2,  10, 12, 64,
      96, 1,  11,  4,  7,  5,  98, 1,   10, 12, 32, 11, 11, 4,  7,
      5,  98, 3,   10, 12, 32, 11, 211, 5,  0,  12, 12, 12, 12, 21};

  EXPECT_EQ(machineCode, expectedCode);
}
