#include <gtest/gtest.h>

#include "common.h"
#include "../ast.h"
#include "../lexy.h"
#include "../parser.h"
#include "../semantic.h"

TEST(machineCode, mc_variableDeclaration)
{
  std::string filename = astPath + "variableDeclarationTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationTest.F7", AST, symbols);

  std::vector<std::bitset<8>> expectedResult = {0b00010101};
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

TEST(machineCode, mc_DeclarationAssignement)
{
  std::string filename = astPath + "variableDeclarationAssignementTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationAssignementTest.F7", AST, symbols);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "variableDeclarationAssignementTest.F7.ir.bin");

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
  std::string filename = astPath + "variableExpressionsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "variableExpressionsTest.F7.ir.bin");

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
  std::string filename = astPath + "ifNoElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifNoElseTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "ifNoElseTest.F7.ir.bin");

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
  std::string filename = astPath + "ifElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifElseTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "ifElseTest.F7.ir.bin");

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
  std::string filename = astPath + "whileTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("whileTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "whileTest.F7.ir.bin");

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
  std::string filename = astPath + "emptyVoidTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("emptyVoidTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "emptyVoidTest.F7.ir.bin");

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
  std::string filename = astPath + "voidWithArgsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("voidWithArgsTest.F7", AST, symbols);

  auto const machineCode = Parser::ParseMachineCode(ir);

  std::vector<std::bitset<8>> expectedResult = ReadFileAsBitset(binPath + "voidWithArgsTest.F7.ir.bin");
  
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