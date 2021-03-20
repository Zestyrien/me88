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
/*
TEST(IR, ir_variableDeclarationAssignement) {
  std::string filename = basePath + "variableDeclarationAssignementTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir =
      Parser::ParseIR("variableDeclarationAssignementTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_variableExpressions) {
  std::string filename = basePath + "variableExpressionsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_ifNoElse) {
  std::string filename = basePath + "ifNoElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifNoElseTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_ifElse) {
  std::string filename = basePath + "ifElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifElseTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_while) {
  std::string filename = basePath + "whileTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("whileTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_emptyVoid) {
  std::string filename = basePath + "emptyVoidTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("emptyVoidTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    Print(ir);
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

TEST(IR, ir_voidWithArgs) {
  std::string filename = basePath + "voidWithArgsTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("voidWithArgsTest.F7", AST, symbols);

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    //Print(ir);
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}*/