#include "codeparse.h"
#include "codegenerator.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

void ParseExpression8Bits(const std::shared_ptr<Node> &node,
                          CodeGenerator &code) {
  // expressions leave the result in al
  auto ndvalue = node->GetValue();
  auto ndtype = node->GetType();
  if (ndtype == NodeType::Number) {
    code.LoadAL(std::stoi(ndvalue));
    return;
  }

  if (ndtype == NodeType::Variable) {
    code.LoadAL(ndvalue);
    return;
  }

  auto right = node->GetRight();
  if (right != nullptr) {
    // evaluate the right expression
    ParseExpression8Bits(right, code);
  }

  auto left = node->GetLeft();
  // Assignement
  if (ndvalue == ":=") {
    // assignement must have a variable on the left
    // and an expression on the right
    code.AddAssignement(left->GetValue());
    return;
  }

  // this is dependent of what is on the right
  // whatever was in the left was evaluated and
  // it is sitting in al
  auto lftype = left->GetType();
  auto lfvalue = left->GetValue();
  if (lftype == NodeType::Variable) {
    if (ndvalue == "+") {
      code.AddDSDI(lfvalue);
    } else if (ndvalue == "-") {
      code.SubDSDI(lfvalue);
    } else if (ndvalue == ">") {
      code.EvalBelowDSDI(lfvalue);
    } else if (ndvalue == "<") {
      code.EvalAboveDSDI(lfvalue);
    } else if (ndvalue == "==") {
      code.EvalEqualDSDI(lfvalue);
    } else if (ndvalue == "!=") {
      code.EvalNotEqualDSDI(lfvalue);
    }
  } else {
    if (ndvalue == "+") {
      code.Add(std::stoi(lfvalue));
    } else if (ndvalue == "-") {
      code.Sub(std::stoi(lfvalue));
    } else if (ndvalue == ">") {
      code.EvalBelow(std::stoi(lfvalue));
    } else if (ndvalue == "<") {
      code.EvalAbove(std::stoi(lfvalue));
    } else if (ndvalue == "==") {
      code.EvalEqual(std::stoi(lfvalue));
    } else if (ndvalue == "!=") {
      code.EvalNotEqual(std::stoi(lfvalue));
    }
  }
}

void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolsTable &symbols,
                CodeGenerator &code,
                const std::vector<std::shared_ptr<Node>> &args =
                    std::vector<std::shared_ptr<Node>>());

void PushArguments(const std::shared_ptr<Tree> &tree,
                   const SymbolsTable &symbols, CodeGenerator &code,
                   std::vector<std::shared_ptr<Node>> argnodes = {});

void RemoveFrame(CodeGenerator &code);

void PopArguments(const std::shared_ptr<Tree> &tree,
                  const SymbolsTable &symbols, CodeGenerator &code);

void CreateFunFrame(const std::shared_ptr<Tree> &tree,
                    const SymbolsTable &symbols, CodeGenerator &code);

void ParseWhile(const std::shared_ptr<Node> &node, const SymbolsTable &symbols,
                CodeGenerator &code) {
  auto whilebody = node->GetTree();

  // this is where we jump after the loop is executed
  int preconditionAddr = code.Size();

  // condition will be evaluated and put in al
  ParseExpression8Bits(node->GetLeft(), code);

  // if the condition is true there will be 1 in AL
  code.Cmp(1);
  // if the condition is meet we don't jump and execute the  while body
  code.JmpNE();
  auto whileblockPlaceholder = code.CreateJumpPlaceholder();

  if (whilebody != nullptr) {
    ParseScope(whilebody, symbols, code);
  }

  code.Jmp(preconditionAddr);
  code.ReplaceJumpPlaceholder(whileblockPlaceholder);
}

void ParseIf(const std::shared_ptr<Node> &node, const SymbolsTable &symbols,
             CodeGenerator &code) {

  // condition will be evaluated and put in al
  ParseExpression8Bits(node->GetLeft(), code);

  auto ndbodies = node->GetRight();
  auto ifbodyTree = ndbodies->GetLeft()->GetTree();
  auto ndbodieselse = ndbodies->GetRight();
  std::shared_ptr<Tree> elsebodyTree = nullptr;
  if (ndbodieselse != nullptr) {
    elsebodyTree = ndbodieselse->GetTree();
  }

  // if the condition is true there will be 1 in AL
  code.Cmp(1);
  // if the condition is meet we don't jump and execute the if block
  code.JmpNE();
  // TO DO optimize jumps in case of empty if block
  auto ifblockPlaceholder = code.CreateJumpPlaceholder();

  if (ifbodyTree != nullptr) {
    ParseScope(ifbodyTree, symbols, code);
  }

  if (elsebodyTree != nullptr) {
    code.Jmp();
    auto elseblockPlaceholder = code.CreateJumpPlaceholder();
    code.ReplaceJumpPlaceholder(ifblockPlaceholder);
    ParseScope(elsebodyTree, symbols, code);
    code.ReplaceJumpPlaceholder(elseblockPlaceholder);
  } else {
    code.ReplaceJumpPlaceholder(ifblockPlaceholder);
  }
}

void ParseFunctionDefinition(const std::shared_ptr<Node> &node,
                             const SymbolsTable &symbols, CodeGenerator &code) {

  auto ndFunName = node->GetLeft();
  auto funName = ndFunName->GetValue();

  auto ndFunType = ndFunName->GetLeft();
  auto funType = ndFunType->GetValue();

  auto treeArgs = ndFunName->GetTree();
  auto ndArgs = treeArgs->GetNodes();

  auto treeBody = node->GetTree();

  // skip the function during the normal flow of execution
  code.Jmp();
  auto functionPlaceholder = code.CreateJumpPlaceholder();

  code.m_funBodies[funName] = treeBody;

  CreateFunFrame(code.m_funBodies[funName], symbols, code);
  code.CreateCallNear(funName);

  ParseScope(treeBody, symbols, code, ndArgs);
  RemoveFrame(code);
  code.ReturnNear();

  code.ReplaceJumpPlaceholder(functionPlaceholder);
}

void ParseFunctionCall(const std::shared_ptr<Node> &node,
                       const SymbolsTable &symbols, CodeGenerator &code) {
  auto funName = node->GetValue();
  auto argTree = node->GetTree();
  auto argNods = argTree->GetNodes();

  auto [success, args] = symbols.GetArgumentsForFunction(funName);

  PushArguments(code.m_funBodies[funName], symbols, code, argNods);

  code.CallNear(funName);

  PopArguments(code.m_funBodies[funName], symbols, code);
}

// void ParseNode(const std::shared_ptr<Node> &node, const SymbolsTable
// &symbols,
//                CodeGenerator &code) {
//   switch (node->GetType()) {
//   case NodeType::Variable:
//     // var declaration ignore
//     break;
//   case NodeType::Operator:
//     ParseExpression8Bits(node, code);
//     break;
//   case NodeType::If:
//     ParseIf(node, symbols, code);
//     break;
//   case NodeType::While:
//     ParseWhile(node, symbols, code);
//     break;
//   case NodeType::Function:
//     ParseFunctionDefinition(node, symbols, code);
//     break;
//   case NodeType::FunctionCall:
//     ParseFunctionCall(node, symbols, code);
//     break;
//   default:
//     spdlog::error("Invalid node type in tree: type {} value {}",
//                   node->GetType(), node->GetValue());
//     break;
//   }
// }

void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolsTable &symbols,
                CodeGenerator &code,
                const std::vector<std::shared_ptr<Node>> &args) {
  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetSymbolsInScope(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return;
  }

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::Variable) {
      // push in the stack variables in the scope
      code.Push();
    }
  }

  for (auto node : tree->GetNodes()) {
    // ParseNode(node, symbols, code);
    switch (node->GetType()) {
    case NodeType::Variable:
      // var declaration ignore
      break;
    case NodeType::Operator:
      ParseExpression8Bits(node, code);
      break;
    case NodeType::If:
      ParseIf(node, symbols, code);
      break;
    case NodeType::While:
      ParseWhile(node, symbols, code);
      break;
    case NodeType::Function:
      ParseFunctionDefinition(node, symbols, code);
      break;
    case NodeType::FunctionCall:
      ParseFunctionCall(node, symbols, code);
      break;
    default:
      spdlog::error("Invalid node type in tree: type {} value {}",
                    node->GetType(), node->GetValue());
      break;
    }
  }
}

void PushArguments(const std::shared_ptr<Tree> &tree,
                   const SymbolsTable &symbols, CodeGenerator &code,
                   std::vector<std::shared_ptr<Node>> argnodes) {

  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetVariablesInFrame(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return;
  }

  code.SetPushingFunctionArgs(true);

  for (auto &node : argnodes) {
    ParseExpression8Bits(node, code);
    code.PushArgument();
  }
  
  code.SetPushingFunctionArgs(false);

}

void CreateFunFrame(const std::shared_ptr<Tree> &tree,
                    const SymbolsTable &symbols, CodeGenerator &code) {

  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetVariablesInFrame(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return;
  }

  code.NewFunFrame();

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::FunctionArg) {
      // add arguments in the scope
      code.AddToStack(entry->name, SymbolType::FunctionArg);
    }
  }

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::Variable) {
      // add variables in the scope
      code.AddToStack(entry->name, SymbolType::Variable);
    }
  }
}

void RemoveFrame(CodeGenerator &code) { code.RemoveFrame(); }

void PopArguments(const std::shared_ptr<Tree> &tree,
                  const SymbolsTable &symbols, CodeGenerator &code) {

  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetVariablesInFrame(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen. There "
                  "is a bug somewhere, please open a issue on github this is a "
                  "bug for sure! Please attach your F7 code.");
    return;
  }

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::FunctionArg) {
      // remove arguments from the scope
      code.Pop();
    }
  }
}

std::vector<std::bitset<8>> GenerateCode(const Tree &ast,
                                         const SymbolsTable &symbols) {

  CodeGenerator code;
  auto tree = std::make_shared<Tree>(ast);

  CreateFunFrame(tree, symbols, code);
  PushArguments(tree, symbols, code);

  ParseScope(tree, symbols, code);

  PopArguments(tree, symbols, code);
  RemoveFrame(code);

  code.Hlt();

#ifndef NDEBUG
  code.Print();
#endif

  return code.GetCode();
}
