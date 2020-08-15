#include "semantic.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

bool IsVarType(const std::shared_ptr<Node> &node) {
  if (node == nullptr) {
    return false;
  }

  return node->GetType() == NodeType::Type;
}

bool IsVarName(const std::shared_ptr<Node> &node) {
  if (node == nullptr) {
    return false;
  }

  return node->GetType() == NodeType::Variable;
}

bool IsAssignment(const std::shared_ptr<Node> &node) {
  if (node == nullptr) {
    return false;
  }

  return node->GetType() == NodeType::Assign;
}

bool IsOperator(const std::shared_ptr<Node> &node) {
  if (node == nullptr) {
    return false;
  }

  return node->GetType() == NodeType::Operator;
}

bool IsDeclared(const std::shared_ptr<Node> &node, int scopeId,
                const SymbolsTable &table) {
  if (node == nullptr) {
    return false;
  }

  return table.IsDeclared(scopeId, node->GetValue());
}

bool IsAssigned(const std::shared_ptr<Node> &node, int scopeId,
                const SymbolsTable &table) {
  if (node == nullptr) {
    return false;
  }

  return table.IsAssigned(scopeId, node->GetValue());
}

bool IsUndefined(const std::shared_ptr<Node> &node, int scopeId,
                 const SymbolsTable &table) {
  if (node == nullptr) {
    return false;
  }

  if (IsVarName(node)) {
    auto val = node->GetValue();
    if (!table.IsDeclared(scopeId, val)) {
      spdlog::info("decl scopeId {} val {}", scopeId, val);
    }
    if (!table.IsAssigned(scopeId, val)) {

      spdlog::info("ass scopeId {} val {}", scopeId, val);
    }
    return !table.IsDeclared(scopeId, val) || !table.IsAssigned(scopeId, val);
  }

  if (node->GetType() == NodeType::Number) {
    return false;
  }

  auto left = node->GetLeft();
  auto right = node->GetRight();

  return IsUndefined(node->GetLeft(), scopeId, table) &&
         IsUndefined(node->GetRight(), scopeId, table);
}

std::string GetExpressionType(const std::shared_ptr<Node> &node, int scopeId,
                              const SymbolsTable &table) {
  // TO DO add more types
  auto nodeType = node->GetType();
  if (nodeType == NodeType::Type) {
    return node->GetValue();
  }

  if (nodeType == NodeType::Variable) {
    return table.GetSymbolStrType(scopeId, node->GetValue());
  }

  if (nodeType == NodeType::Number) {
    return "u8";
  }

  auto left = node->GetLeft();
  if (left != nullptr) {
    return GetExpressionType(left, scopeId, table);
  }

  return "Unknown";
}

bool ExpressionCheck(const std::shared_ptr<Node> &node, int scopeId,
                     SymbolsTable &table, bool isAssignment = false) {
  auto left = node->GetLeft();
  auto right = node->GetRight();

  if (left == nullptr) {
    if (right != nullptr) {
      spdlog::warn(
          "Malformed expression will ignore right operand {} at line {}",
          right->GetValue(), right->GetLine());
    }

    return true;
  }

  if (left == nullptr && right == nullptr) {
    return true;
  }

  bool leftUndef = IsUndefined(left, scopeId, table);
  bool rightUndef = IsUndefined(right, scopeId, table);
  if (leftUndef || rightUndef) {
    spdlog::error("Malformed expression type of '{}' is undefined at line {}",
                  leftUndef ? left->GetValue() : right->GetValue(),
                  leftUndef ? left->GetLine() : right->GetLine());
    return false;
  }

  auto lType = GetExpressionType(left, scopeId, table);
  auto rType = GetExpressionType(right, scopeId, table);

  if (std::strcmp(lType.c_str(), rType.c_str()) != 0 ||
      std::strcmp(lType.c_str(), "Unknown") == 0) {
    spdlog::error("Invalid expression types mismatch {} {} {} {} at line {}",
                  lType, left->GetValue(), rType, right->GetValue(),
                  left->GetLine());
    return false;
  }

  return true;
}

bool GenerateSymbolsForTree(const std::shared_ptr<Tree> tree,
                            SymbolsTable &table);

bool GenerateSymbolsForFunction(const std::shared_ptr<Node> &node, int scopeId,
                                SymbolsTable &table) {
  // TO DO error checks
  // auto funBodyTree = node->GetTree();

  // // TO DO generating simbols like this allows the following
  // //
  // //			void write : u8 pos, u8 val {
  // //			 	u8 s := 5;
  // //			 	s := s + x;
  // //			}
  // //			u8 x := 6;
  // //
  // //			x is defined after the function declaration and I don't
  // // like it

  // auto funNameNode = node->GetLeft();
  // auto name = funNameNode->GetValue();

  // auto typeNode = funNameNode->GetLeft();
  // auto type = typeNode->GetValue();

  // auto argsTree = funNameNode->GetTree();
  // auto argsNodes = argsTree->GetNodes();
  // std::vector<std::pair<std::string, std::string>> args;
  // for (const auto &argnode : argsNodes) {
  //   auto argname = argnode->GetValue();
  //   auto argtype = argnode->GetLeft()->GetValue();
  //   args.push_back(std::make_pair(argname, argtype));
  // }

  // auto success = GenerateSymbolsForTree(funBodyTree, table, args);
  // if (!success) {
  //   return false;
  // }
  // funBodyTree->SetScope(treescope);
  // table.AddFunction(type, name, args, scopeId);

  return true;
}

bool IsValidFunctionCall(const std::shared_ptr<Node> &node, int scopeId,
                         SymbolsTable &table) {
  // auto funName = node->GetValue();
  // if (table.IsDeclared(scopeId, funName)) {
  //   table.AddFunctionOccurrence(funName, scopeId);
  // } else {
  //   spdlog::error("Undeclared function {}", funName);
  //   return false;
  // }

  // auto argsTree = node->GetTree();
  // auto argsNodes = argsTree->GetNodes();
  // std::vector<std::string> args;
  // for (const auto &argnode : argsNodes) {
  //   auto argtype = GetExpressionType(argnode, scopeId, table);
  //   args.push_back(argtype);
  // }

  // if (table.AreValidFunctionArgs(scopeId, funName, args)) {
  //   return true;
  // } else {
  //   spdlog::error("Arguments type mismatch for '{}' function call", funName);
  //   return false;
  // }
  return true;
}

bool GenerateSymbolsForNode(const std::shared_ptr<Node> &node, int scopeId,
                            SymbolsTable &table) {
  if (node == nullptr) {
    return true;
  }

  if (node->GetType() == NodeType::Function) {
    if (!GenerateSymbolsForFunction(node, scopeId, table)) {
      return false;
    }
    return true;
  }

  if (node->GetType() == NodeType::FunctionCall) {
    // if (!GenerateSymbolsForFunctionCall(node, scopeId, table)) {
    //   return false;
    // }

    return true;
  }

  auto left = node->GetLeft();
  if (left != nullptr) {
    if (!GenerateSymbolsForNode(left, scopeId, table)) {
      return false;
    }
  }

  auto right = node->GetRight();
  if (right != nullptr) {
    if (!GenerateSymbolsForNode(right, scopeId, table)) {
      return false;
    }
  }

  auto tree = node->GetTree();
  if (tree != nullptr) {
    auto success = GenerateSymbolsForTree(tree, table);
    if (!success) {
      return false;
    }
  }

  if (IsVarType(node)) {
    // var declaration parent node will take care of it
    return true;
  }

  if (IsVarType(left) && IsVarName(node)) {
    if (!table.AddVariableSymbolToScope(left->GetValue(), node->GetValue(),
                                        scopeId)) {
      spdlog::error("Line {}", node->GetLine());
      return false;
    }

    return true;
  }

  if (IsVarName(node)) {
    if (!table.AddSymbolOccurrence(node->GetValue(), scopeId)) {
      spdlog::error("Line {}", node->GetLine());
      return false;
    }
  }

  if (IsAssignment(node) || IsOperator(node)) {
    if (IsVarName(left)) {
      if (!table.SetVariableAssigend(left->GetValue(), scopeId)) {
        spdlog::error("Line {}", left->GetLine());
        return false;
      }
    }

    return ExpressionCheck(left, scopeId, table, IsAssignment(node)) &&
           ExpressionCheck(right, scopeId, table);
  }

  return true;
}

bool GenerateSymbolsForTree(const std::shared_ptr<Tree> tree,
                            SymbolsTable &table) {
  auto nodes = tree->GetNodes();
  auto scopeId = tree->GetScopeId();
  auto parentScopeId = tree->GetParentScopeId();
  table.AddScope(scopeId, parentScopeId);
  for (const auto &node : nodes) {
    auto success = GenerateSymbolsForNode(node, scopeId, table);
    if (!success) {
      return false;
    }
  }

  return true;
}

std::tuple<bool, SymbolsTable> AnalyzeSemantic(const Tree &tree, bool debug) {
  auto table = SymbolsTable();

  auto success = GenerateSymbolsForTree(std::make_shared<Tree>(tree), table);

  if (debug) {
    table.Print();
  }

  return {success, table};
}