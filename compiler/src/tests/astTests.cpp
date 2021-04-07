#include <gtest/gtest.h>
#include <unordered_map>

#include "../ast.h"
#include "../lexy.h"

TEST(AST, CreateAST_Empty)
{
  std::string filename = "fakeTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);
}

TEST(AST, CreateAST_VariableDeclaration)
{
  std::string filename = "../compiler/src/tests/files/ast/variableDeclarationTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  // 2 nodes because there is a reparse of te expression
  // doesn't make much sense for declarations but leave it for now
  EXPECT_EQ(nodes.size(), 2);

  // declaration ast
  auto first = nodes[0];
  EXPECT_EQ(first->GetValue(), "a");
  EXPECT_EQ(first->GetType(), NodeType::Variable);
  EXPECT_EQ(first->GetLine(), 1);
  EXPECT_EQ(first->GetRight(), nullptr);
  EXPECT_EQ(first->GetTree(), nullptr);

  auto left = first->GetLeft();
  EXPECT_EQ(left->GetRight(), nullptr);
  EXPECT_EQ(left->GetTree(), nullptr);
  EXPECT_EQ(left->GetValue(), "u8");
  EXPECT_EQ(left->GetLine(), 1);
}

TEST(AST, CreateAST_VariableDeclarationAssignement)
{
  std::string filename =
      "../compiler/src/tests/files/ast/variableDeclarationAssignementTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();

  EXPECT_EQ(nodes.size(), 2);

  // assignement ast
  auto assignement = nodes[1];
  EXPECT_EQ(assignement->GetValue(), ":=");
  EXPECT_EQ(assignement->GetType(), NodeType::Operator);
  EXPECT_EQ(assignement->GetLine(), 1);

  auto left = assignement->GetLeft();
  EXPECT_EQ(left->GetValue(), "a");
  EXPECT_EQ(left->GetType(), NodeType::Variable);
  EXPECT_EQ(left->GetLine(), 1);
  EXPECT_EQ(left->GetLeft(), nullptr);
  EXPECT_EQ(left->GetRight(), nullptr);
  EXPECT_EQ(left->GetTree(), nullptr);

  auto right = assignement->GetRight();
  EXPECT_EQ(right->GetValue(), "5");
  EXPECT_EQ(right->GetType(), NodeType::Number);
  EXPECT_EQ(right->GetLine(), 1);
  EXPECT_EQ(right->GetLeft(), nullptr);
  EXPECT_EQ(right->GetRight(), nullptr);
  EXPECT_EQ(right->GetTree(), nullptr);
}

TEST(AST, CreateAST_IfNoElse)
{
  std::string filename = "../compiler/src/tests/files/ast/ifNoElseTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  auto ifnode = nodes[0];

  EXPECT_EQ(ifnode->GetValue(), "if");
  EXPECT_EQ(ifnode->GetType(), NodeType::If);
  EXPECT_EQ(ifnode->GetLine(), 1);
  EXPECT_EQ(ifnode->GetTree(), nullptr);

  auto ifCondition = ifnode->GetLeft();
  EXPECT_EQ(ifCondition->GetValue(), "1");
  EXPECT_EQ(ifCondition->GetType(), NodeType::Number);
  EXPECT_EQ(ifCondition->GetLine(), 1);
  EXPECT_EQ(ifCondition->GetTree(), nullptr);
  EXPECT_EQ(ifCondition->GetRight(), nullptr);
  EXPECT_EQ(ifCondition->GetLeft(), nullptr);

  auto firstIfBody = ifnode->GetRight();
  EXPECT_EQ(firstIfBody->GetValue(), "Bodies");
  EXPECT_EQ(firstIfBody->GetType(), NodeType::IfBody);
  //-1 is to point out that is only in the tree
  EXPECT_EQ(firstIfBody->GetLine(), -1);
  EXPECT_EQ(firstIfBody->GetTree(), nullptr);
  EXPECT_EQ(firstIfBody->GetRight(), nullptr);

  auto realIfBody = firstIfBody->GetLeft();
  EXPECT_EQ(realIfBody->GetValue(), "If body");
  EXPECT_EQ(realIfBody->GetType(), NodeType::IfBody);
  //-1 is to point out that is only in the tree
  EXPECT_EQ(realIfBody->GetLine(), -1);
  EXPECT_EQ(realIfBody->GetLeft(), nullptr);
  EXPECT_EQ(realIfBody->GetRight(), nullptr);

  auto ifTree = realIfBody->GetTree();
  EXPECT_EQ(ifTree->GetParentScopeId(), 4);

  auto ifBodyNodes = ifTree->GetNodes();
  EXPECT_EQ(ifBodyNodes.size(), 2);
}

TEST(AST, CreateAST_IfElse)
{
  std::string filename = "../compiler/src/tests/files/ast/ifElseTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  auto ifnode = nodes[0];

  EXPECT_EQ(ifnode->GetValue(), "if");
  EXPECT_EQ(ifnode->GetType(), NodeType::If);
  EXPECT_EQ(ifnode->GetLine(), 1);
  EXPECT_EQ(ifnode->GetTree(), nullptr);

  auto ifCondition = ifnode->GetLeft();
  EXPECT_EQ(ifCondition->GetValue(), "1");
  EXPECT_EQ(ifCondition->GetType(), NodeType::Number);
  EXPECT_EQ(ifCondition->GetLine(), 1);
  EXPECT_EQ(ifCondition->GetTree(), nullptr);
  EXPECT_EQ(ifCondition->GetRight(), nullptr);
  EXPECT_EQ(ifCondition->GetLeft(), nullptr);

  auto firstIfBody = ifnode->GetRight();
  EXPECT_EQ(firstIfBody->GetValue(), "Bodies");
  EXPECT_EQ(firstIfBody->GetType(), NodeType::IfBody);
  //-1 is to point out that is only in the tree
  EXPECT_EQ(firstIfBody->GetLine(), -1);
  EXPECT_EQ(firstIfBody->GetTree(), nullptr);

  auto realIfBody = firstIfBody->GetLeft();
  EXPECT_EQ(realIfBody->GetValue(), "If body");
  EXPECT_EQ(realIfBody->GetType(), NodeType::IfBody);
  //-1 is to point out that is only in the tree
  EXPECT_EQ(realIfBody->GetLine(), -1);
  EXPECT_EQ(realIfBody->GetLeft(), nullptr);
  EXPECT_EQ(realIfBody->GetRight(), nullptr);

  auto ifTree = realIfBody->GetTree();
  EXPECT_EQ(ifTree->GetParentScopeId(), 6);

  auto ifBodyNodes = ifTree->GetNodes();
  EXPECT_EQ(ifBodyNodes.size(), 2);

  auto elseCondition = firstIfBody->GetRight();
  EXPECT_EQ(elseCondition->GetValue(), "Else body");
  EXPECT_EQ(elseCondition->GetType(), NodeType::ElseBody);
  EXPECT_EQ(elseCondition->GetLine(), 3);
  EXPECT_EQ(elseCondition->GetLeft(), nullptr);
  EXPECT_EQ(elseCondition->GetRight(), nullptr);

  auto elseTree = elseCondition->GetTree();
  EXPECT_EQ(elseTree->GetParentScopeId(), 6);

  auto elseBodyNodes = elseTree->GetNodes();
  EXPECT_EQ(ifBodyNodes.size(), 2);
}

TEST(AST, CreateAST_While)
{
  std::string filename = "../compiler/src/tests/files/ast/whileTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  EXPECT_EQ(nodes.size(), 1);

  auto whileNode = nodes[0];
  EXPECT_EQ(whileNode->GetValue(), "while");
  EXPECT_EQ(whileNode->GetType(), NodeType::While);
  EXPECT_EQ(whileNode->GetLine(), 1);
  EXPECT_EQ(whileNode->GetRight(), nullptr);

  auto whileCondition = whileNode->GetLeft();
  EXPECT_EQ(whileCondition->GetValue(), "1");
  EXPECT_EQ(whileCondition->GetType(), NodeType::Number);
  EXPECT_EQ(whileCondition->GetLine(), 1);
  EXPECT_EQ(whileCondition->GetTree(), nullptr);
  EXPECT_EQ(whileCondition->GetRight(), nullptr);
  EXPECT_EQ(whileCondition->GetTree(), nullptr);

  auto whileBody = whileNode->GetTree();
  EXPECT_EQ(whileBody->GetParentScopeId(), 9);

  auto whileBodyNodes = whileBody->GetNodes();
  EXPECT_EQ(whileBodyNodes.size(), 2);
}

TEST(AST, Create_EmptyVoid)
{
  std::string filename = "../compiler/src/tests/files/ast/emptyVoidTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  EXPECT_EQ(nodes.size(), 2);

  auto funNd = nodes[0];
  EXPECT_EQ(funNd->GetValue(), "Function");
  EXPECT_EQ(funNd->GetType(), NodeType::Function);
  EXPECT_EQ(funNd->GetLine(), 1);
  EXPECT_EQ(funNd->GetRight(), nullptr);
  EXPECT_EQ(funNd->GetTree()->GetNodes().size(), 0);

  auto funLeft = funNd->GetLeft();
  EXPECT_EQ(funLeft->GetValue(), "foo");
  EXPECT_EQ(funLeft->GetType(), NodeType::Variable);
  EXPECT_EQ(funLeft->GetLine(), 1);
  EXPECT_EQ(funLeft->GetRight(), nullptr);
  EXPECT_EQ(funLeft->GetTree()->GetNodes().size(), 0);

  auto funLeftType = funLeft->GetLeft();
  EXPECT_EQ(funLeftType->GetValue(), "void");
  EXPECT_EQ(funLeftType->GetType(), NodeType::Type);
  EXPECT_EQ(funLeftType->GetLine(), 1);
  EXPECT_EQ(funLeftType->GetLeft(), nullptr);
  EXPECT_EQ(funLeftType->GetRight(), nullptr);
  EXPECT_EQ(funLeftType->GetTree(), nullptr);

  auto funCall = nodes[1];
  EXPECT_EQ(funCall->GetValue(), "foo");
  EXPECT_EQ(funCall->GetType(), NodeType::FunctionCall);
  EXPECT_EQ(funCall->GetLine(), 3);
  EXPECT_EQ(funCall->GetTree()->GetNodes().size(), 0);
  EXPECT_EQ(funCall->GetRight(), nullptr);
  EXPECT_EQ(funCall->GetLeft(), nullptr);
}

TEST(AST, Create_VoidWithArgs)
{
  std::string filename = "../compiler/src/tests/files/ast/voidWithArgsTest.F7";
  auto [validToks, tokens] = Lexer::GetTokensFromFile(filename);

  auto [validProg, AST] = CreateAST(tokens);

  EXPECT_TRUE(validProg);

  EXPECT_EQ(AST.GetParentScopeId(), 0);

  auto nodes = AST.GetNodes();
  EXPECT_EQ(nodes.size(), 6);

  auto funNd = nodes[0];
  EXPECT_EQ(funNd->GetValue(), "Function");
  EXPECT_EQ(funNd->GetType(), NodeType::Function);
  EXPECT_EQ(funNd->GetLine(), 1);
  EXPECT_EQ(funNd->GetRight(), nullptr);
  EXPECT_EQ(funNd->GetTree()->GetNodes().size(), 2);

  auto funLeft = funNd->GetLeft();
  EXPECT_EQ(funLeft->GetValue(), "foo");
  EXPECT_EQ(funLeft->GetType(), NodeType::Variable);
  EXPECT_EQ(funLeft->GetLine(), 1);
  EXPECT_EQ(funLeft->GetRight(), nullptr);

  auto argNds = funLeft->GetTree()->GetNodes();
  EXPECT_EQ(argNds.size(), 2);

  auto firstArg = argNds[0];
  EXPECT_EQ(firstArg->GetValue(), "a");
  EXPECT_EQ(firstArg->GetType(), NodeType::Variable);
  EXPECT_EQ(firstArg->GetLine(), 1);
  EXPECT_EQ(firstArg->GetTree(), nullptr);
  EXPECT_EQ(firstArg->GetRight(), nullptr);

  auto firstArgType = firstArg->GetLeft();
  EXPECT_EQ(firstArgType->GetValue(), "u8");
  EXPECT_EQ(firstArgType->GetType(), NodeType::Type);
  EXPECT_EQ(firstArgType->GetLine(), 1);
  EXPECT_EQ(firstArgType->GetLeft(), nullptr);
  EXPECT_EQ(firstArgType->GetRight(), nullptr);
  EXPECT_EQ(firstArgType->GetTree(), nullptr);

  auto secodnArg = argNds[1];
  EXPECT_EQ(secodnArg->GetValue(), "b");
  EXPECT_EQ(secodnArg->GetType(), NodeType::Variable);
  EXPECT_EQ(secodnArg->GetLine(), 1);
  EXPECT_EQ(secodnArg->GetTree(), nullptr);
  EXPECT_EQ(secodnArg->GetRight(), nullptr);

  auto secondArgType = firstArg->GetLeft();
  EXPECT_EQ(secondArgType->GetValue(), "u8");
  EXPECT_EQ(secondArgType->GetType(), NodeType::Type);
  EXPECT_EQ(secondArgType->GetLine(), 1);
  EXPECT_EQ(secondArgType->GetLeft(), nullptr);
  EXPECT_EQ(secondArgType->GetRight(), nullptr);
  EXPECT_EQ(secondArgType->GetTree(), nullptr);

  auto funLeftType = funLeft->GetLeft();
  EXPECT_EQ(funLeftType->GetValue(), "void");
  EXPECT_EQ(funLeftType->GetType(), NodeType::Type);
  EXPECT_EQ(funLeftType->GetLine(), 1);
  EXPECT_EQ(funLeftType->GetLeft(), nullptr);
  EXPECT_EQ(funLeftType->GetRight(), nullptr);
  EXPECT_EQ(funLeftType->GetTree(), nullptr);

  auto funCall = nodes[5];
  EXPECT_EQ(funCall->GetValue(), "foo");
  EXPECT_EQ(funCall->GetType(), NodeType::FunctionCall);
  EXPECT_EQ(funCall->GetLine(), 8);
  EXPECT_EQ(funCall->GetRight(), nullptr);
  EXPECT_EQ(funCall->GetLeft(), nullptr);

  auto callArgs = funCall->GetTree()->GetNodes();
  EXPECT_EQ(callArgs.size(), 2);

  auto firstCallArg = callArgs[0];
  EXPECT_EQ(firstCallArg->GetValue(), "x");
  EXPECT_EQ(firstCallArg->GetType(), NodeType::Variable);
  EXPECT_EQ(firstCallArg->GetLine(), 8);
  EXPECT_EQ(firstCallArg->GetLeft(), nullptr);
  EXPECT_EQ(firstCallArg->GetRight(), nullptr);
  EXPECT_EQ(firstCallArg->GetTree(), nullptr);

  auto secondCallArg = callArgs[1];
  EXPECT_EQ(secondCallArg->GetValue(), "y");
  EXPECT_EQ(secondCallArg->GetType(), NodeType::Variable);
  EXPECT_EQ(secondCallArg->GetLine(), 8);
  EXPECT_EQ(secondCallArg->GetLeft(), nullptr);
  EXPECT_EQ(secondCallArg->GetRight(), nullptr);
  EXPECT_EQ(secondCallArg->GetTree(), nullptr);
}