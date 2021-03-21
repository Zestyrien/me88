#include <gtest/gtest.h>

#include "../ast.h"
#include "../lexy.h"
#include "../parser.h"
#include "../semantic.h"

#include <iostream>

// std::string const basePath = "../compiler/src/tests/files/ast/";

auto const PrintMC = [](std::vector<std::bitset<8>> const &code) {
  for (auto const &entry : code)
  {
    std::cout << entry.to_string() << std::endl;
  }
};
TEST(machineCode, mc_variableDeclaration)
{
  std::string filename = basePath + "variableDeclarationTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationTest.F7", AST, symbols);

  std::vector<std::bitset<8>> expectedResult = {0b00010101};
  auto const machineCode = Parser::ParseMachineCode(ir);

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    PrintMC(machineCode);
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_DeclarationAssignement)
{
  std::string filename = basePath + "variableDeclarationAssignementTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir =
      Parser::ParseIR("variableDeclarationAssignementTest.F7", AST, symbols);

  std::vector<std::bitset<8>> expectedResult = {
      0b00001011, 0b01100000, 0b00000101, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000001, 0b00001010, 0b00001100, 0b01000000,
      0b01100000, 0b00000001, 0b00001100, 0b00010101};

  auto const machineCode = Parser::ParseMachineCode(ir);

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_variableExpressions)
{
  std::string filename = basePath + "variableExpressionsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b00001011, 0b00001011, 0b00001011, 0b01100000, 0b00000101,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000010, 0b00001010, 0b00001100, 0b01000000, 0b01100000,
      0b00000001, 0b01100000, 0b01100100, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000001, 0b00001010,
      0b00001100, 0b01000000, 0b01100000, 0b00000001, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000001,
      0b00001010, 0b00001100, 0b00100000, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000010, 0b00001010,
      0b00001100, 0b00100010, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000011, 0b00001010, 0b00001100,
      0b01000000, 0b01100000, 0b00000001, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000011, 0b00001010,
      0b00001100, 0b00100000, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000010, 0b00001010, 0b00001100,
      0b00100011, 0b01100010, 0b00000011, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000011, 0b00001010,
      0b00001100, 0b01000000, 0b01100000, 0b00000001, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000011,
      0b00001010, 0b00001100, 0b00100000, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000010, 0b00001010,
      0b00001100, 0b00100001, 0b11000011, 0b00000000, 0b01110000,
      0b01100000, 0b00000000, 0b11000000, 0b00000000, 0b01110010,
      0b01100000, 0b00000001, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000011, 0b00001010, 0b00001100,
      0b00100000, 0b00001011, 0b00000100, 0b00000111, 0b11100010,
      0b01100011, 0b00000010, 0b00001010, 0b00001100, 0b00100001,
      0b11000001, 0b00000000, 0b10001100, 0b01100000, 0b00000000,
      0b11000000, 0b00000000, 0b10001110, 0b01100000, 0b00000001,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000011, 0b00001010, 0b00001100, 0b00100000, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000010,
      0b00001010, 0b00001100, 0b00100001, 0b11000110, 0b00000000,
      0b10101000, 0b01100000, 0b00000000, 0b11000000, 0b00000000,
      0b10101010, 0b01100000, 0b00000001, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000011, 0b00001010,
      0b00001100, 0b00100000, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000010, 0b00001010, 0b00001100,
      0b00100001, 0b11001100, 0b00000000, 0b11000100, 0b01100000,
      0b00000000, 0b11000000, 0b00000000, 0b11000110, 0b01100000,
      0b00000001, 0b00001011, 0b00000100, 0b00000111, 0b11100010,
      0b01100011, 0b00000011, 0b00001010, 0b00001100, 0b00100000,
      0b01100001, 0b00000101, 0b11000011, 0b00000000, 0b11011001,
      0b01100000, 0b00000000, 0b11000000, 0b00000000, 0b11011011,
      0b01100000, 0b00000001, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000011, 0b00001010, 0b00001100,
      0b00100000, 0b01100001, 0b00000110, 0b11000001, 0b00000000,
      0b11101110, 0b01100000, 0b00000000, 0b11000000, 0b00000000,
      0b11110000, 0b01100000, 0b00000001, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000011, 0b00001010,
      0b00001100, 0b00100000, 0b01100001, 0b00000111, 0b11000110,
      0b00000001, 0b00000011, 0b01100000, 0b00000000, 0b11000000,
      0b00000001, 0b00000101, 0b01100000, 0b00000001, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000011,
      0b00001010, 0b00001100, 0b00100000, 0b01100001, 0b00001000,
      0b11001100, 0b00000001, 0b00011000, 0b01100000, 0b00000000,
      0b11000000, 0b00000001, 0b00011010, 0b01100000, 0b00000001,
      0b00001100, 0b00001100, 0b00001100, 0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_ifNoElse)
{
  std::string filename = basePath + "ifNoElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifNoElseTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b01100000, 0b00000001, 0b01100001, 0b00000001, 0b11001100,
      0b00000000, 0b00010011, 0b00001011, 0b01100000, 0b00000101,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000001, 0b00001010, 0b00001100, 0b01000000, 0b01100000,
      0b00000001, 0b00001100, 0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_ifElse)
{
  std::string filename = basePath + "ifElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifElseTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b01100000, 0b00000001, 0b01100001, 0b00000001, 0b11001100,
      0b00000000, 0b00010110, 0b00001011, 0b01100000, 0b00000101,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000001, 0b00001010, 0b00001100, 0b01000000, 0b01100000,
      0b00000001, 0b00001100, 0b11000000, 0b00000000, 0b00100101,
      0b00001011, 0b01100000, 0b00000111, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000001, 0b00001010,
      0b00001100, 0b01000000, 0b01100000, 0b00000001, 0b00001100,
      0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_while)
{
  std::string filename = basePath + "whileTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("whileTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b01100000, 0b00000001, 0b01100001, 0b00000001, 0b11001100,
      0b00000000, 0b00010110, 0b00001011, 0b01100000, 0b00000101,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100011,
      0b00000001, 0b00001010, 0b00001100, 0b01000000, 0b01100000,
      0b00000001, 0b00001100, 0b11000000, 0b00000000, 0b00000000,
      0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_emptyVoid)
{
  std::string filename = basePath + "emptyVoidTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("emptyVoidTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b11000000, 0b00000000, 0b00000001, 0b00010010, 0b11010011,
      0b00000000, 0b00000010, 0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}

TEST(machineCode, mc_voidWithArgs)
{
  std::string filename = basePath + "voidWithArgsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("voidWithArgsTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = {
      0b00001011, 0b00001011, 0b11000000, 0b00000000, 0b00100010,
      0b00001011, 0b00001011, 0b00000100, 0b00000111, 0b11100010,
      0b01100010, 0b00000011, 0b00001010, 0b00001100, 0b00100000,
      0b00001011, 0b00000100, 0b00000111, 0b11100010, 0b01100010,
      0b00000100, 0b00001010, 0b00001100, 0b00100010, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000001,
      0b00001010, 0b00001100, 0b01000000, 0b01100000, 0b00000001,
      0b00001100, 0b00010010, 0b01100000, 0b00000101, 0b00001011,
      0b00000100, 0b00000111, 0b11100010, 0b01100011, 0b00000010,
      0b00001010, 0b00001100, 0b01000000, 0b01100000, 0b00000001,
      0b01100000, 0b00000111, 0b00001011, 0b00000100, 0b00000111,
      0b11100010, 0b01100011, 0b00000001, 0b00001010, 0b00001100,
      0b01000000, 0b01100000, 0b00000001, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000001, 0b00001010,
      0b00001100, 0b00100000, 0b00001011, 0b00001011, 0b00000100,
      0b00000111, 0b11100010, 0b01100011, 0b00000010, 0b00001010,
      0b00001100, 0b00100000, 0b00001011, 0b11010011, 0b00000000,
      0b00000100, 0b00001100, 0b00001100, 0b00001100, 0b00001100,
      0b00010101};

  EXPECT_EQ(machineCode.size(), expectedResult.size());
  if (machineCode.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < machineCode.size(); i++)
  {
    EXPECT_EQ(machineCode[i], expectedResult[i]);
  }
}