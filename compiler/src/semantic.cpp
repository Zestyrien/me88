#include "semantic.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

bool IsVarType(const std::shared_ptr<Node> &node)
{
  if (node == nullptr)
  {
    return false;
  }

  return node->GetType() == NodeType::Type;
}

bool IsVarName(const std::shared_ptr<Node> &node)
{
  if (node == nullptr)
  {
    return false;
  }

  return node->GetType() == NodeType::Variable;
}

bool IsAssignment(const std::shared_ptr<Node> &node)
{
  if (node == nullptr)
  {
    return false;
  }

  return node->GetType() == NodeType::Assign;
}

bool IsOperator(const std::shared_ptr<Node> &node)
{
  if (node == nullptr)
  {
    return false;
  }

  return node->GetType() == NodeType::Operator;
}

bool IsDeclared(const std::shared_ptr<Node> &node, int scopeId,
                const SymbolsTable &table)
{
  if (node == nullptr)
  {
    return false;
  }

  if (node->GetType() == NodeType::Number)
  {
    return true;
  }

  return table.IsDeclared(scopeId, node->GetValue());
}

bool IsAssigned(const std::shared_ptr<Node> &node, int scopeId,
                const SymbolsTable &table)
{
  if (node == nullptr)
  {
    return false;
  }

  return table.IsAssigned(scopeId, node->GetValue());
}

bool IsUndefined(const std::shared_ptr<Node> &node, int scopeId,
                 const SymbolsTable &table)
{
  if (node == nullptr)
  {
    return false;
  }

  if (IsVarName(node))
  {
    auto val = node->GetValue();
    if (!table.IsDeclared(scopeId, val))
    {
      spdlog::info("decl scopeId {} val {}", scopeId, val);
    }
    if (!table.IsAssigned(scopeId, val))
    {

      spdlog::info("ass scopeId {} val {}", scopeId, val);
    }
    return !table.IsDeclared(scopeId, val) || !table.IsAssigned(scopeId, val);
  }

  if (node->GetType() == NodeType::Number)
  {
    return false;
  }

  auto left = node->GetLeft();
  auto right = node->GetRight();

  return IsUndefined(node->GetLeft(), scopeId, table) &&
         IsUndefined(node->GetRight(), scopeId, table);
}

std::string GetExpressionType(const std::shared_ptr<Node> &node, int scopeId,
                              const SymbolsTable &table)
{
  // TO DO add more types
  auto nodeType = node->GetType();
  if (nodeType == NodeType::Type)
  {
    return node->GetValue();
  }

  if (nodeType == NodeType::Variable)
  {
    return table.GetSymbolStrType(scopeId, node->GetValue());
  }

  if (nodeType == NodeType::Number)
  {
    return "u8";
  }

  auto left = node->GetLeft();
  if (left != nullptr)
  {
    return GetExpressionType(left, scopeId, table);
  }

  return "Unknown";
}

bool ExpressionCheck(const std::shared_ptr<Node> &node, int scopeId,
                     SymbolsTable &table, bool isAssignment = false)
{
  auto left = node->GetLeft();
  auto right = node->GetRight();

  if (left == nullptr)
  {
    if (right != nullptr)
    {
      spdlog::warn(
          "Malformed expression will ignore right operand {} at line {}",
          right->GetValue(), right->GetLine());
    }

    return true;
  }

  if (left == nullptr && right == nullptr)
  {
    return true;
  }

  bool leftUndef = IsUndefined(left, scopeId, table);
  bool rightUndef = IsUndefined(right, scopeId, table);
  if (leftUndef || rightUndef)
  {
    spdlog::error("Malformed expression type of '{}' is undefined at line {}",
                  leftUndef ? left->GetValue() : right->GetValue(),
                  leftUndef ? left->GetLine() : right->GetLine());
    return false;
  }

  auto lType = GetExpressionType(left, scopeId, table);
  auto rType = GetExpressionType(right, scopeId, table);

  if (std::strcmp(lType.c_str(), rType.c_str()) != 0 ||
      std::strcmp(lType.c_str(), "Unknown") == 0)
  {
    spdlog::error("Invalid expression types mismatch {} {} {} {} at line {}",
                  lType, left->GetValue(), rType, right->GetValue(),
                  left->GetLine());
    return false;
  }

  return true;
}

bool GenerateSymbolsForTree(const std::shared_ptr<Tree> tree,
                            SymbolsTable &table);

bool GenerateSymbolsForNode(const std::shared_ptr<Node> &node, int scopeId,
                            SymbolsTable &table);

bool GenerateSymbolsForFunction(const std::shared_ptr<Node> &node, int scopeId,
                                SymbolsTable &table)
{

  // TO DO checks for function errors(?)
  // No args, return type and so on

  auto ndFunName = node->GetLeft();

  auto ndFunType = ndFunName->GetLeft();
  auto funType = ndFunType->GetValue();

  auto treeArgs = ndFunName->GetTree();
  auto ndArgs = treeArgs->GetNodes();

  auto IsArgument = [ndArgs](const std::string &name) {
    for (const auto &ndArg : ndArgs)
    {
      if (name == ndArg->GetValue())
      {
        return true;
      }
    }
    return false;
  };

  auto treeBody = node->GetTree();
  auto functionScopeId = treeBody->GetScopeId();

  auto funName = ndFunName->GetValue();
  auto parentScopeId = treeBody->GetParentScopeId();

  std::vector<std::pair<std::string, std::string>> arguments;
  for (const auto &ndArg : ndArgs)
  {
    arguments.push_back(
        std::make_pair(ndArg->GetLeft()->GetValue(), ndArg->GetValue()));
  }

  table.AddFunctionScope(functionScopeId, parentScopeId);
  if (!table.AddFunctionDefinition(funName, funType, arguments,
                                   parentScopeId))
  {
    // TO DO error case
  }

  int index = 0;
  for (const auto &ndArg : ndArgs)
  {
    auto argName = ndArg->GetValue();
    auto ndType = ndArg->GetLeft();
    auto argType = ndType->GetValue();
    if (!table.AddFunctionArgumentToScope(argName, argType, index,
                                          functionScopeId))
    {
      // TO DO error case
    }
    index++;
  }

  auto ndsBody = treeBody->GetNodes();
  for (const auto &ndBody : ndsBody)
  {
    if (!IsArgument(ndBody->GetValue()))
    {
      auto success = GenerateSymbolsForNode(ndBody, functionScopeId, table);
      if (!success)
      {
        // TO DO error case
        return false;
      }
    }
  }

  return true;
}

bool GenerateSymbolsForFunctionCall(const std::shared_ptr<Node> &node,
                                    int scopeId, SymbolsTable &table)
{

  auto funName = node->GetValue();
  auto funArgsTree = node->GetTree();
  auto funArgsNodes = funArgsTree->GetNodes();

  std::vector<std::string> argTypes;
  for (const auto &ndArg : funArgsNodes)
  {
    if (!IsDeclared(ndArg, scopeId, table))
    {
      spdlog::error("Undefined argument {} in function call {} at line {}",
                    ndArg->GetValue(), funName, ndArg->GetLine());
      return false;
    }

    argTypes.push_back(GetExpressionType(ndArg, scopeId, table));
  }

  if (!table.AddSymbolOccurrence(funName, scopeId, argTypes))
  {
    spdlog::error("Undeclared function {}", funName);
    return false;
  }

  return true;
}

bool GenerateSymbolsForNode(const std::shared_ptr<Node> &node, int scopeId,
                            SymbolsTable &table)
{
  if (node == nullptr)
  {
    return true;
  }

  if (node->GetType() == NodeType::Function)
  {
    if (!GenerateSymbolsForFunction(node, scopeId, table))
    {
      return false;
    }
    return true;
  }

  if (node->GetType() == NodeType::FunctionCall)
  {
    if (!GenerateSymbolsForFunctionCall(node, scopeId, table))
    {
      return false;
    }

    return true;
  }

  auto left = node->GetLeft();
  if (left != nullptr)
  {
    if (!GenerateSymbolsForNode(left, scopeId, table))
    {
      return false;
    }
  }

  auto right = node->GetRight();
  if (right != nullptr)
  {
    if (!GenerateSymbolsForNode(right, scopeId, table))
    {
      return false;
    }
  }

  auto tree = node->GetTree();
  if (tree != nullptr)
  {
    auto success = GenerateSymbolsForTree(tree, table);
    if (!success)
    {
      return false;
    }
  }

  if (IsVarType(node))
  {
    // var declaration parent node will take care of it
    return true;
  }

  if (IsVarType(left) && IsVarName(node))
  {
    if (!table.AddVariableSymbolToScope(left->GetValue(), node->GetValue(),
                                        scopeId))
    {
      spdlog::error("Line {}", node->GetLine());
      return false;
    }

    return true;
  }

  if (IsVarName(node))
  {
    if (!table.AddSymbolOccurrence(node->GetValue(), scopeId))
    {
      spdlog::error("Line {}", node->GetLine());
      return false;
    }
  }

  if (IsAssignment(node) || IsOperator(node))
  {
    if (IsVarName(left))
    {
      if (!table.SetVariableAssigend(left->GetValue(), scopeId))
      {
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
                            SymbolsTable &table)
{
  auto nodes = tree->GetNodes();
  auto scopeId = tree->GetScopeId();
  auto parentScopeId = tree->GetParentScopeId();
  table.AddScope(scopeId, parentScopeId);
  for (const auto &node : nodes)
  {
    auto success = GenerateSymbolsForNode(node, scopeId, table);
    if (!success)
    {
      return false;
    }
  }

  return true;
}

std::tuple<bool, SymbolsTable> AnalyzeSemantic(const Tree &tree)
{
  auto table = SymbolsTable();

  auto success = GenerateSymbolsForTree(std::make_shared<Tree>(tree), table);

#ifndef NDEBUG
  table.Print();
#endif

  return {success, table};
}