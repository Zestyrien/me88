#include <gtest/gtest.h>

#include "../ast.h"
#include "../lexy.h"
#include "../parser.h"
#include "../semantic.h"

#include <iostream>

std::string const basePath = "../compiler/src/tests/files/ast/";

auto const Print = [](std::vector<std::string> const &code) {
  for (auto const &entry : code) {
    std::cout << entry << std::endl;
  }
};

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
      "# displacement for a",
      "^1",
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

  auto const ir = Parser::ParseIR("variableExpressionsTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {"#",
                                             "# variableExpressionsTest.F7",
                                             "#",
                                             "",
                                             "# start scope push variables",
                                             "push_al",
                                             "push_al",
                                             "push_al",
                                             "",
                                             "# parse expression from line 2",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%5",
                                             "# assignement expression for b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "# move value in variable",
                                             "mov_al_ds$di",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 3",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%100",
                                             "# assignement expression for c",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for c",
                                             "^1",
                                             "mov_ax_di",
                                             "pop_al",
                                             "# move value in variable",
                                             "mov_al_ds$di",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 5",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for c",
                                             "^1",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded c in AL",
                                             "# expression + b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "add_ds$di_al",
                                             "# assignement expression for a",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "# move value in variable",
                                             "mov_al_ds$di",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 7",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression - b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "sub_ds$di_al",
                                             "# expression + 3",
                                             "add_operand_al",
                                             "%3",
                                             "# assignement expression for a",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "# move value in variable",
                                             "mov_al_ds$di",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 9",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression > b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "cmp_ds$di_al",
                                             "jb_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 10",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression < b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "cmp_ds$di_al",
                                             "ja_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 11",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression == b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "cmp_ds$di_al",
                                             "je_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 12",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression != b",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for b",
                                             "^2",
                                             "mov_ax_di",
                                             "pop_al",
                                             "cmp_ds$di_al",
                                             "jne_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 14",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression > 5",
                                             "cmp_operand_al",
                                             "%5",
                                             "jb_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 15",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression < 6",
                                             "cmp_operand_al",
                                             "%6",
                                             "ja_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 16",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression == 7",
                                             "cmp_operand_al",
                                             "%7",
                                             "je_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
                                             "",
                                             "# parse expression from line 17",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for a",
                                             "^3",
                                             "mov_ax_di",
                                             "pop_al",
                                             "mov_ds$di_al",
                                             "# loaded a in AL",
                                             "# expression != 8",
                                             "cmp_operand_al",
                                             "%8",
                                             "jne_cs_offset",
                                             "off+5",
                                             "off+5",
                                             "# condition false",
                                             "mov_operand_al",
                                             "%0",
                                             "jmp_cs_offset",
                                             "off+2",
                                             "off+2",
                                             "# condition true",
                                             "mov_operand_al",
                                             "%1",
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

TEST(IR, ir_ifNoElse) {
  std::string filename = basePath + "ifNoElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifNoElseTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {"#",
                                             "# ifNoElseTest.F7",
                                             "#",
                                             "",
                                             "# start scope push variables",
                                             "",
                                             "# parse if statement from line 1",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%1",
                                             "# check expression result",
                                             "cmp_operand_al",
                                             "%1",
                                             "jne_cs_offset",
                                             "off+15",
                                             "off+15",
                                             "# start scope push variables",
                                             "push_al",
                                             "",
                                             "# parse expression from line 2",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%5",
                                             "# assignement expression for x",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for x",
                                             "^1",
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

TEST(IR, ir_ifElse) {
  std::string filename = basePath + "ifElseTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("ifElseTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {"#",
                                             "# ifElseTest.F7",
                                             "#",
                                             "",
                                             "# start scope push variables",
                                             "",
                                             "# parse if statement from line 1",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%1",
                                             "# check expression result",
                                             "cmp_operand_al",
                                             "%1",
                                             "jne_cs_offset",
                                             "off+18",
                                             "off+18",
                                             "# start scope push variables",
                                             "push_al",
                                             "",
                                             "# parse expression from line 2",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%5",
                                             "# assignement expression for x",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for x",
                                             "^1",
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
                                             "jmp_cs_offset",
                                             "off+15",
                                             "off+15",
                                             "# start scope push variables",
                                             "push_al",
                                             "",
                                             "# parse expression from line 4",
                                             "# load number in al",
                                             "mov_operand_al",
                                             "%7",
                                             "# assignement expression for y",
                                             "push_al",
                                             "mov_ss_ax",
                                             "mov_ax_ds",
                                             "mov_bp_ax",
                                             "sub_operand_al",
                                             "# displacement for y",
                                             "^1",
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

TEST(IR, ir_while) {
  std::string filename = basePath + "whileTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("whileTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {
      "#",
      "# whileTest.F7",
      "#",
      "",
      "# start scope push variables",
      "",
      "# parse while statement from line 1",
      "# while loop",
      "# load number in al",
      "mov_operand_al",
      "%1",
      "cmp_operand_al",
      "%1",
      "jne_cs_offset",
      "off+18",
      "off+18",
      "# start scope push variables",
      "push_al",
      "",
      "# parse expression from line 2",
      "# load number in al",
      "mov_operand_al",
      "%5",
      "# assignement expression for x",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for x",
      "^1",
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
      "jmp_cs_offset",
      "off-25",
      "off-25",
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

TEST(IR, ir_emptyVoid) {
  std::string filename = basePath + "emptyVoidTest.F7";
  auto const [validToks, tokens] = Lexer::GetTokensFromFile(filename);
  EXPECT_TRUE(validToks);

  auto const [validAST, AST] = CreateAST(tokens);
  EXPECT_TRUE(validAST);

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  EXPECT_TRUE(validSemantic);

  auto const ir = Parser::ParseIR("emptyVoidTest.F7", AST, symbols);

  std::vector<std::string> expectedResult = {
      "#",
      "# emptyVoidTest.F7",
      "#",
      "",
      "# start scope push variables",
      "",
      "# parse function definition from line 1",
      "# function definition for foo",
      "jmp_cs_offset",
      "off+1",
      "off+1",
      "@foo",
      "# start scope push variables",
      "",
      "# end scope clean stack",
      "",
      "retn",
      "# parse function call from line 3",
      "# function call for foo",
      "call_cs_offset",
      "!foo",
      "!foo",
      "# end scope clean stack",
      "",
      "# end program",
      "htl"};

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

  std::vector<std::string> expectedResult = {
      "#",
      "# voidWithArgsTest.F7",
      "#",
      "",
      "# start scope push variables",
      "push_al",
      "push_al",
      "",
      "# parse function definition from line 1",
      "# function definition for foo",
      "jmp_cs_offset",
      "off+32",
      "off+32",
      "@foo",
      "# start scope push variables",
      "push_al",
      "",
      "# parse expression from line 2",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "add_operand_al",
      "# displacement for a",
      "^3",
      "mov_ax_di",
      "pop_al",
      "mov_ds$di_al",
      "# loaded a in AL",
      "# expression + b",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "add_operand_al",
      "# displacement for b",
      "^4",
      "mov_ax_di",
      "pop_al",
      "add_ds$di_al",
      "# assignement expression for k",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for k",
      "^1",
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
      "retn",
      "# parse expression from line 5",
      "# load number in al",
      "mov_operand_al",
      "%5",
      "# assignement expression for x",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for x",
      "^2",
      "mov_ax_di",
      "pop_al",
      "# move value in variable",
      "mov_al_ds$di",
      "mov_operand_al",
      "%1",
      "",
      "# parse expression from line 6",
      "# load number in al",
      "mov_operand_al",
      "%7",
      "# assignement expression for y",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for y",
      "^1",
      "mov_ax_di",
      "pop_al",
      "# move value in variable",
      "mov_al_ds$di",
      "mov_operand_al",
      "%1",
      "",
      "# parse function call from line 8",
      "# function call for foo",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for y",
      "^1",
      "mov_ax_di",
      "pop_al",
      "mov_ds$di_al",
      "# loaded y in AL",
      "push_al",
      "push_al",
      "mov_ss_ax",
      "mov_ax_ds",
      "mov_bp_ax",
      "sub_operand_al",
      "# displacement for x",
      "^2",
      "mov_ax_di",
      "pop_al",
      "mov_ds$di_al",
      "# loaded x in AL",
      "push_al",
      "call_cs_offset",
      "!foo",
      "!foo",
      "pop_al",
      "pop_al",
      "# end scope clean stack",
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