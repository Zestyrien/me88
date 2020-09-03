#include "ast.h"
#include <iostream>
#include <tuple>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

bool ParseTree(const std::vector<Token> &tokens, Tree &tree, int &index);

std::tuple<bool, std::shared_ptr<Node>>
ParseStatement(const std::vector<Token> &tokens, const Tree &tree, int &index);

std::tuple<bool, std::shared_ptr<Node>>
ReverseParseExpression(const std::vector<Token> &tokens,
                       const Tree &parentScope, int &index) {
  std::shared_ptr<Node> expression = nullptr;
  std::shared_ptr<Node> lastOperand = nullptr;
  std::shared_ptr<Node> lastOperator = nullptr;

  while (index != tokens.size()) {
    auto itType = tokens[index].GetType();
    if (itType == TokenType::Number || itType == TokenType::Variable) {
      if (lastOperand == nullptr) {
        lastOperand = std::make_shared<Node>(
            tokens[index].GetType() == TokenType::Number ? NodeType::Number
                                                         : NodeType::Variable,
            tokens[index].GetValue(), tokens[index].GetLine());
      } else if (lastOperator != nullptr) {
        auto operand = std::make_shared<Node>(
            tokens[index].GetType() == TokenType::Number ? NodeType::Number
                                                         : NodeType::Variable,
            tokens[index].GetValue(), tokens[index].GetLine());

        lastOperator->SetLeft(operand);

        if (expression == nullptr) {
          lastOperator->SetRight(lastOperand);
        } else {
          lastOperator->SetRight(expression);
        }

        expression = lastOperator;
        lastOperand = nullptr;
        lastOperator = nullptr;
      } else {
        spdlog::error("Invalid expression, unexpected token {} at line {}",
                      tokens[index].GetValue(), tokens[index].GetLine());
        index++;
        return {false, nullptr};
      }
    } else if (itType == TokenType::Operator) {
      lastOperator =
          std::make_shared<Node>(NodeType::Operator, tokens[index].GetValue(),
                                 tokens[index].GetLine());
    } else if (itType == TokenType::Semicol || itType == TokenType::Then ||
               itType == TokenType::Loop || itType == TokenType::Comma ||
               itType == TokenType::CloseBracket) {
      if (lastOperand != nullptr && lastOperator != nullptr) {
        lastOperator->SetLeft(lastOperand);

        if (expression == nullptr) {
          lastOperator->SetRight(lastOperand);
        } else {
          lastOperator->SetRight(expression);
        }

        expression = lastOperator;

        lastOperand = nullptr;
        lastOperator = nullptr;
      }

      return {true, expression};
    } else {
      break;
    }

    index++;
  }

  spdlog::error("Invalid expression, unexpected token {} at line {}",
                tokens[index].GetValue(), tokens[index].GetLine());
  index++;
  return {false, nullptr};
}

std::tuple<bool, std::shared_ptr<Node>>
ParseExpression(const std::vector<Token> &tokens, const Tree &parentScope,
                int &index) {
  if (tokens.size() == index) {
    spdlog::error("Invalid empty expression");
    return {false, nullptr};
  }

  auto currentTok = tokens[index];
  if (currentTok.GetType() != TokenType::Number ||
      currentTok.GetType() != TokenType::Variable) {
    auto firstOp = std::make_shared<Node>(
        currentTok.GetType() == TokenType::Number ? NodeType::Number
                                                  : NodeType::Variable,
        currentTok.GetValue(), currentTok.GetLine());

    index++;
    if (tokens.size() == index) {
      spdlog::error("Invalid expression termination at line {}",
                    currentTok.GetLine());
      return {false, nullptr};
    }
    currentTok = tokens[index];

    if (currentTok.GetType() == TokenType::Semicol ||
        currentTok.GetType() == TokenType::Then ||
        currentTok.GetType() == TokenType::Loop ||
        currentTok.GetType() == TokenType::Comma ||
        currentTok.GetType() == TokenType::CloseBracket) {
      return {true, firstOp};
    }

    if (currentTok.GetType() == TokenType::Assign ||
        currentTok.GetType() == TokenType::Operator) {
      auto op = std::make_shared<Node>(
          NodeType::Operator, currentTok.GetValue(), currentTok.GetLine());
      bool ret = false;
      if (currentTok.GetType() == TokenType::Assign) {

        op->SetLeft(firstOp);
        index++;

        auto [success, expression] =
            ParseExpression(tokens, parentScope, index);
        ret = success;
        if (success) {
          op->SetRight(expression);
        }
      } else {
        index--;
        auto [success, expression] =
            ReverseParseExpression(tokens, parentScope, index);
        ret = success;

        op = expression;
      }

      return {ret, op};
    }
  }

  spdlog::error("Invalid expression, unexpected token {} at line {}",
                tokens[index].GetValue(), tokens[index].GetLine());
  index++;
  return {false, nullptr};
}

std::tuple<bool, std::shared_ptr<Node>>
ParseIf(const std::vector<Token> &tokens, const Tree &parentScope, int &index) {
  auto currentTok = tokens[index];
  auto ifnode = std::make_shared<Node>(NodeType::If, currentTok.GetValue(),
                                       currentTok.GetLine());
  index++;

  auto [validCond, condnode] = ParseExpression(tokens, parentScope, index);
  if (!validCond) {
    spdlog::error("Failed to parse if condition at line {}",
                  currentTok.GetLine());
    return {false, nullptr};
  }

  ifnode->SetLeft(condnode);

  auto bodies = std::make_shared<Node>(NodeType::IfBody, "Bodies", -1);
  ifnode->SetRight(bodies);

  auto ifbody = std::make_shared<Node>(NodeType::IfBody, "If body", -1);
  bodies->SetLeft(ifbody);

  auto ifbodyTree = Tree(parentScope.GetScopeId());
  while (tokens[index].GetType() != TokenType::Else &&
         tokens[index].GetType() != TokenType::EndIf) {

    if (!ParseTree(tokens, ifbodyTree, index)) {
      return {false, nullptr};
    }

    if (tokens.size() == index) {
      spdlog::error("Malformed if statement, missing endif at line {}",
                    tokens[index - 1].GetLine());
      return {false, nullptr};
    }
  }

  auto treeptr = std::make_shared<Tree>(ifbodyTree);
  ifbody->SetTree(treeptr);

  if (tokens[index].GetType() == TokenType::Else) {
    auto elsebody = std::make_shared<Node>(NodeType::ElseBody, "Else body",
                                           tokens[index].GetLine());
    bodies->SetRight(elsebody);

    auto elsebodyTree = Tree(parentScope.GetScopeId());
    while (tokens[index].GetType() != TokenType::EndIf) {
      if (!ParseTree(tokens, elsebodyTree, index)) {
        return {false, nullptr};
      }

      if (index == tokens.size()) {
        spdlog::error("Malformed if statement, missing endif");
        return {false, nullptr};
      }
    }

    treeptr = std::make_shared<Tree>(elsebodyTree);
    elsebody->SetTree(treeptr);
  }

  return {true, ifnode};
}

std::tuple<bool, std::shared_ptr<Node>>
ParseWhile(const std::vector<Token> &tokens, const Tree &parentScope,
           int &index) {

  if (tokens.size() == index) {
    return {false, nullptr};
  }

  auto whilenode = std::make_shared<Node>(
      NodeType::While, tokens[index].GetValue(), tokens[index].GetLine());
  index++;

  auto [validCond, condnode] = ParseExpression(tokens, parentScope, index);
  if (!validCond) {
    spdlog::error("Failed to parse while condition at line {}",
                  tokens[index].GetLine());
    return {false, nullptr};
  }

  whilenode->SetLeft(condnode);

  auto whilebodytree = Tree(parentScope.GetScopeId());

  while (tokens[index].GetType() != TokenType::EndLoop) {
    if (!ParseTree(tokens, whilebodytree, index)) {
      return {false, nullptr};
    }

    if (index == tokens.size()) {
      spdlog::error("Malformed while statement, missing endloop");
      return {false, nullptr};
    }
  }
  
  //remove endloop or in case of nested loop it will exit all loops
  index++;

  auto treeptr = std::make_shared<Tree>(whilebodytree);
  whilenode->SetTree(treeptr);

  return {true, whilenode};
}

std::tuple<bool, std::shared_ptr<Node>>
ParseDeclaration(const std::vector<Token> &tokens, const Tree &parentScope,
                 int &index) {
  auto varType = std::make_shared<Node>(
      NodeType::Type, tokens[index].GetValue(), tokens[index].GetLine());
  index++;

  if (tokens[index].GetType() != TokenType::Variable) {
    spdlog::error(
        "Variable must follow variable type, unexpected token {} at line {}",
        tokens[index].GetValue(), tokens[index].GetLine());
    return {false, nullptr};
  }

  index++;

  if (tokens.size() != index) {
    if (tokens[index].GetType() == TokenType::Assign ||
        tokens[index].GetType() == TokenType::Semicol ||
        tokens[index].GetType() == TokenType::Comma ||
        tokens[index].GetType() == TokenType::OpenCurly ||
        tokens[index].GetType() == TokenType::Column) {
      // basically turning:
      // 	i8 x := 5;
      // into:
      //	i8 x;
      //	x = 5;
      // by re-parsing the var name
      index--;

      auto varName =
          std::make_shared<Node>(NodeType::Variable, tokens[index].GetValue(),
                                 tokens[index].GetLine());
      varName->SetLeft(varType);
      return {true, varName};
    }
  }

  spdlog::error("Declaration unexpected token {} at line {}",
                tokens[index].GetValue(), tokens[index].GetLine());
  return {false, nullptr};
}

std::tuple<bool, std::shared_ptr<Node>>
ParseFunctionDefinition(const std::vector<Token> &tokens,
                        const Tree &parentScope, int &index) {
  auto functionNode = std::make_shared<Node>(NodeType::Function, "Function",
                                             tokens[index].GetLine());
  auto [success, functionDeclaration] =
      ParseDeclaration(tokens, parentScope, index);
  if (success) {
    functionNode->SetLeft(functionDeclaration);
  } else {
    return {false, nullptr};
  }

  index = index + 2; // skip to open curly
  if (index >= tokens.size()) {
    // not possible isFunction check already checked for the curly
    spdlog::error("Error in function definition, this should never happen.");
    return {false, nullptr};
  }

  auto functionArguments = std::make_shared<Tree>(parentScope.GetScopeId());
  while (tokens[index].GetType() != TokenType::OpenCurly) {
    if (tokens[index].GetType() == TokenType::Comma) {
      index++;
    }

    auto [success, declaration] = ParseDeclaration(tokens, parentScope, index);
    if (success && declaration->GetType() != NodeType::Function) {
      functionArguments->AddNode(declaration);
    } else {
      spdlog::error("Function definition unexpected token {} at line {}",
                    tokens[index].GetValue(), tokens[index].GetLine());
      return {false, nullptr};
    }

    index++;
    if (index == tokens.size()) {
      spdlog::error(
          "Invalid function definiition unexpected token {} at line {}",
          tokens[index - 1].GetValue(), tokens[index - 1].GetLine());
      return {false, nullptr};
    }
  }
  functionDeclaration->SetTree(functionArguments);

  if (tokens[index].GetType() == TokenType::OpenCurly) {
    index++;
  }

  auto functionBody = std::make_shared<Tree>(parentScope.GetScopeId());
  while (tokens[index].GetType() != TokenType::CloseCurly) {
    auto [success, statement] = ParseStatement(tokens, parentScope, index);
    if (success) {
      functionBody->AddNode(statement);
    } else {
      return {false, nullptr};
    }
  }

  if (tokens[index].GetType() == TokenType::CloseCurly) {
    index++;
  }

  functionNode->SetTree(functionBody);

  return {true, functionNode};
}

bool IsFunction(const std::vector<Token> &tokens, const int &index) {
  auto expectedColumn = index + 2;
  if (expectedColumn >= tokens.size())
    return false;
  return tokens[expectedColumn].GetType() == TokenType::Column;
}

bool IsFunctionCall(const std::vector<Token> &tokens, const int &index) {
  auto expectedOpenBraket = index + 1;
  if (expectedOpenBraket >= tokens.size())
    return false;
  return tokens[expectedOpenBraket].GetType() == TokenType::OpenBracket;
}

std::tuple<bool, std::shared_ptr<Node>>
ParseStatement(const std::vector<Token> &tokens, const Tree &tree, int &index) {
  // Statements can only begin with
  // 1- variable type: variable declaration
  // 2- variable name: expression
  // 3- semicolumn: is ignored
  // 4- if: beginning of if statement
  // 5- while: beginning of while loop
  // 6- function declaration
  // 7- function call

  switch (tokens[index].GetType()) {
  case TokenType::Type:
    if (IsFunction(tokens, index)) {
      // TO DO
      // return ParseFunctionDefinition(tokens, tree, index);
    } else {
      return ParseDeclaration(tokens, tree, index);
    }
  case TokenType::Variable:
  case TokenType::Number:
    if (IsFunctionCall(tokens, index)) {
      // 	return ParseFunctionCall(tokens, tree, index);
    } else {
      return ParseExpression(tokens, tree, index);
    }
  case TokenType::Semicol:
    // just move forward
    index++;
    return {true, nullptr};
  case TokenType::If:
    return ParseIf(tokens, tree, index);
  case TokenType::While:
    return ParseWhile(tokens, tree, index);
  case TokenType::Else:
  case TokenType::EndIf:
  case TokenType::Loop:
  case TokenType::EndLoop:
  case TokenType::Then:
    index++;
    // else and endif are at the end of the tree
    return {true, nullptr};
  case TokenType::Assign:
  case TokenType::Operator:
  case TokenType::Unknown:
  default:
    spdlog::error("Statement unexpected token {} at line {}",
                  tokens[index].GetValue(), tokens[index].GetLine());
    index++;
    return {false, nullptr};
  }
}

bool ParseTree(const std::vector<Token> &tokens, Tree &tree, int &index) {
  auto [success, node] = ParseStatement(tokens, tree, index);
  if (success) {
    tree.AddNode(node);
  }

  return success;
}

std::tuple<bool, Tree> CreateAST(const std::vector<Token> &tokens) {
  Tree tree(0);
  int index = 0;
  bool success = true;
  while (success && index != tokens.size()) {
    if (!ParseTree(tokens, tree, index)) {
      success = false;
    }
  }

#ifndef NDEBUG
  tree.Print();
#endif

  return {success, tree};
}