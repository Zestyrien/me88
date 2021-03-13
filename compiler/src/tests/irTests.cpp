#include <gtest/gtest.h>

#include "../ast.h"
#include "../lexy.h"
#include "../parser.h"
#include "../semantic.h"

std::string const basePath = "../compiler/src/tests/files/ast/";

TEST(IR, ir_variableDeclaration) {
  std::string filename = basePath + "variableDeclarationTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("variableDeclarationTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {"#",
                                             "# variableDeclarationTest.F7",
                                             "#",
                                             "",
                                             "# start scope push variables",
                                             "",
                                             "# end scope clean stack",
                                             "",
                                             "# end program",
                                             "htl"};

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}

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

  std::vector<std::string> expectedResult = {
      "#",
      "# variableDeclarationAssignementTest.F7",
      "#",
      "",
      "# start scope push variables",
      "push_al",
      "",
      "# parse expression from line 1",
      "# load number in al",
      "mov_operand_al",
      "%5",
      "# assignement expression for a",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "^a",
      "mov_ax_di",
      "pop_al",
      "# move value in variable",
      "mov_al_ds$di",
      "mov_operand_al",
      "%1",
      "",
      "# end scope clean stack",
      "pop_al",
      "",
      "# end program",
      "htl"};

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

  auto const ir =
      Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {
      "#",
      "# variableExpressionsTest.F7",
      "#",
      "",
      "# start scope push variables",
      "push_al",
      "push_al",
      "push_al",
      "",
      "# end scope clean stack",
      "pop_al",
      "pop_al",
      "pop_al",
      "",
      "# end program",
      "htl"};

  EXPECT_EQ(ir.size(), expectedResult.size());
  if (ir.size() != expectedResult.size()) {
    return;
  }

  for (size_t i = 0; i < ir.size(); i++) {
    EXPECT_EQ(ir[i], expectedResult[i]);
  }
}