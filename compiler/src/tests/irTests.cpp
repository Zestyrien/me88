#include <gtest/gtest.h>

#include "common.h"
#include "../ast.h"
#include "../lexy.h"
#include "../parser.h"
#include "../semantic.h"

TEST(IR, ir_variableDeclaration)
{
  std::string filename = astPath + "variableDeclarationTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);
  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);

  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "variableDeclarationTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_variableDeclarationAssignement)
{
  std::string filename = astPath + "variableDeclarationAssignementTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationAssignementTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "variableDeclarationAssignementTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_variableExpressions)
{
  std::string filename = astPath + "variableExpressionsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);
  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "variableExpressionsTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_ifNoElse)
{
  std::string filename = astPath + "ifNoElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifNoElseTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "ifNoElseTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_ifElse)
{
  std::string filename = astPath + "ifElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifElseTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "ifElseTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_while)
{
  std::string filename = astPath + "whileTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("whileTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "whileTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_emptyVoid)
{
  std::string filename = astPath + "emptyVoidTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("emptyVoidTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "emptyVoidTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_voidWithArgs)
{
  std::string filename = astPath + "voidWithArgsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  auto const [validAST, AST] = CreateAST(tokens);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("voidWithArgsTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = ReadFile(irPath + "voidWithArgsTest.F7.ir");

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size())
  {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++)
  {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}