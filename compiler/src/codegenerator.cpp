#include "codegenerator.h"
#include "../../common/opcode.h"
#include <bitset>
#include <iostream>
#include <unordered_map>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

void CodeGenerator::Push() { m_code.push_back((int)Opcode::push_al); }

void CodeGenerator::Pop() { m_code.push_back((int)Opcode::pop_al); }

std::size_t CodeGenerator::Size() { return m_code.size(); }

void CodeGenerator::AddToStack(const std::string &variable) {
  // Push();
  m_frames[m_frames.size() - 1];
  m_frames[m_frames.size() - 1].varToAddr[variable].push_back(
      m_frames[m_frames.size() - 1].varCount);
  m_frames[m_frames.size() - 1].varCount++;
}

void CodeGenerator::SetOutOfScope(const std::string &variable) {
  if (m_frames[m_frames.size() - 1].varToAddr[variable].size() > 1) {
    m_frames[m_frames.size() - 1].varToAddr[variable].pop_back();
  } else {
    m_frames[m_frames.size() - 1].varToAddr.erase(variable);
  }

  m_frames[m_frames.size() - 1].varCount--;
}

void CodeGenerator::RemoveFromStack(const std::string &variable) {
  // SetOutOfScope(variable);
  Pop();
}

void CodeGenerator::LoadOperand(std::bitset<8> op) { m_code.push_back(op); }

void CodeGenerator::LoadAX(int value) {
  auto h = value >> 8;
  LoadAL(h);
  m_code.push_back((int)Opcode::mov_al_ah);
  auto l = value;
  LoadAL(l);
};

void CodeGenerator::AddAssignement(const std::string &variable) {
  LoadVariableDSDI(variable);
  m_code.push_back((int)Opcode::mov_al_ds$di);
  LoadAL(1);
}

void CodeGenerator::LoadVariableDSDI(const std::string &variable) {
  Push(); // save al content

  int offset = 3; // we just pushed

  // set ds
  m_code.push_back((int)Opcode::mov_ss_ax);
  m_code.push_back((int)Opcode::mov_ax_ds);

  // set di
  m_code.push_back((int)Opcode::mov_sp_ax);

  // this can crash but for now I want it to, so I can debug it
  auto frame = &m_frames[m_frames.size() - 1];
  auto size = frame->varToAddr[variable].size();

  if (size < 1) {
    spdlog::error("Error loading variable address for variable {}", variable);
  }

  offset = offset + frame->varCount + m_offsetFix - 1 - frame->varToAddr[variable][size - 1];

  Add(offset);

  m_code.push_back((int)Opcode::mov_ax_di);

  Pop(); // restore al content
}

void CodeGenerator::AddDSDI(const std::string &variable) {
  LoadVariableDSDI(variable);
  m_code.push_back((int)Opcode::add_ds$di_al);
}

void CodeGenerator::SubDSDI(const std::string &variable) {
  LoadVariableDSDI(variable);
  m_code.push_back((int)Opcode::sub_ds$di_al);
}

void CodeGenerator::CmpDSDI(const std::string &variable) {
  LoadVariableDSDI(variable);
  m_code.push_back((int)Opcode::cmp_ds$di_al);
}

void CodeGenerator::EvalBelowDSDI(const std::string &variable) {
  CmpDSDI(variable);
  m_code.push_back((int)Opcode::jb_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalAboveDSDI(const std::string &variable) {
  CmpDSDI(variable);
  m_code.push_back((int)Opcode::ja_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalEqualDSDI(const std::string &variable) {
  CmpDSDI(variable);
  m_code.push_back((int)Opcode::je_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalNotEqualDSDI(const std::string &variable) {
  CmpDSDI(variable);
  m_code.push_back((int)Opcode::jne_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalBelow(int op) {
  Cmp(op);
  m_code.push_back((int)Opcode::jb_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalAbove(int op) {
  Cmp(op);
  m_code.push_back((int)Opcode::ja_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalEqual(int op) {
  Cmp(op);
  m_code.push_back((int)Opcode::je_cs_offset);
  ParseALEval();
}

void CodeGenerator::EvalNotEqual(int op) {
  Cmp(op);
  m_code.push_back((int)Opcode::jne_cs_offset);
  ParseALEval();
}

void CodeGenerator::LoadOffset(int offset) {
  int off = offset;
  m_code.push_back(off);
  m_code.push_back(off >> 8);
}

void CodeGenerator::ParseALEval() {
  // Eval code looks like
  //...
  // ja/jb/je true_
  // false_:
  // mov 0,al
  // jmp end_
  // true_:
  // mov 1,al
  // end_:
  //...

  // offset for the jump to true when the condition is meet
  auto true_ = CreateJumpPlaceholder();
  LoadAL(0);
  // skip mov 1,al
  Jmp();
  // offset to skip the true condition when we did not jump
  auto end_ = CreateJumpPlaceholder();
  // if the condition is true we jump here
  ReplaceJumpPlaceholder(true_);
  LoadAL(1);
  ReplaceJumpPlaceholder(end_);
}

void CodeGenerator::Add(int op) {
  m_code.push_back((int)Opcode::add_operand_al);
  LoadOperand(op);
}

void CodeGenerator::Sub(int op) {
  m_code.push_back((int)Opcode::sub_operand_al);
  LoadOperand(op);
}

void CodeGenerator::Cmp(int op) {
  m_code.push_back((int)Opcode::cmp_operand_al);
  LoadOperand(op);
}

void CodeGenerator::LoadAL(int value) {
  m_code.push_back((int)Opcode::mov_operand_al);
  LoadOperand(value);
}

void CodeGenerator::LoadAL(const std::string &variable) {
  LoadVariableDSDI(variable);
  m_code.push_back((int)Opcode::mov_ds$di_al);
}

void CodeGenerator::JmpNE(int offset) {
  m_code.push_back((int)Opcode::jne_cs_offset);
  LoadOffset(offset);
}

void CodeGenerator::JmpNE() { m_code.push_back((int)Opcode::jne_cs_offset); }

void CodeGenerator::Jmp(int offset) {
  m_code.push_back((int)Opcode::jmp_cs_offset);
  LoadOffset(offset);
}

void CodeGenerator::Jmp() { m_code.push_back((int)Opcode::jmp_cs_offset); }

void CodeGenerator::CreateCallNear(const std::string &function) {
  m_funBodyAddr[function] = Size();
}

void CodeGenerator::ReturnNear() { m_code.push_back((int)Opcode::retn); }

void CodeGenerator::CallNear(const std::string &function) {
  m_code.push_back((int)Opcode::call_cs_offset);
  LoadOffset(m_funBodyAddr[function].to_ulong());
}

int CodeGenerator::CreateJumpPlaceholder() {
  auto sz = Size();
  LoadOffset(0);
  return sz;
}

void CodeGenerator::ReplaceJumpPlaceholder(int index) {
  int offset = Size();
  m_code[index] = offset;
  m_code[index + 1] = (offset >> 8);
}

void CodeGenerator::NewFrame() { m_frames.push_back({}); }

void CodeGenerator::IncrementOffsetCount() { m_offsetFix++; }

void CodeGenerator::RemoveFrame() { m_frames.pop_back(); }

void CodeGenerator::Hlt() { m_code.push_back((int)Opcode::htl); }
std::vector<std::bitset<8>> CodeGenerator::GetCode() { return m_code; }
void CodeGenerator::Print() {
  std::cout << "££££££££££££££££ Code Debug ££££££££££££££££" << std::endl;
  int count = 0;
  for (const auto &st : m_code) {
    std::cout << count << "\t" << st << "\t" << st.to_ulong() << std::endl;
    count++;
  }

  std::cout << "££££££££££££££££££££££££££££££££££££££££££££" << std::endl;
}

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

void AllocateFrame(const std::shared_ptr<Tree> &tree,
                   const SymbolsTable &symbols, CodeGenerator &code,
                   std::vector<std::shared_ptr<Node>> argnodes = {});

void RemoveFrame(CodeGenerator &code);

void PopVariables(const std::shared_ptr<Tree> &tree,
                  const SymbolsTable &symbols, CodeGenerator &code);

void CreateFramShape(const std::shared_ptr<Tree> &tree,
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

  CreateFramShape(code.m_funBodies[funName], symbols, code);
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

  AllocateFrame(code.m_funBodies[funName], symbols, code, argNods);

  // auto size = args.size();
  // // push in reverse it helps when getting the arguments back
  // for (int i = size - 1; i <= 0; i--) {
  //   ParseExpression8Bits(argNods[i], code);
  //   code.Push();
  // }

  code.CallNear(funName);

  PopVariables(code.m_funBodies[funName], symbols, code);
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

  // if (!success) {
  //   spdlog::error("Congratulazioni! This message should never be seen. There
  //   "
  //                 "is a bug somewhere, please open a issue on github this is
  //                 a " "bug for sure! Please attach your F7 code.");
  //   return;
  // }

  // for (auto entry : entries) {
  //   if (entry->symbolTp == SymbolType::Variable) {
  //     // add variables in the scope
  //     code.AddToStack(entry->name);
  //   }
  // }

  // for (const auto arg : args) {
  // }

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

void AllocateFrame(const std::shared_ptr<Tree> &tree,
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

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::Variable) {
      // add variables in the scope
      code.Push();
    }
  }

  for (auto &node : argnodes) {
    ParseExpression8Bits(node, code);
    code.IncrementOffsetCount();
    code.Push();
  }
}

void CreateFramShape(const std::shared_ptr<Tree> &tree,
                     const SymbolsTable &symbols, CodeGenerator &code) {

  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetVariablesInFrame(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen, please "
                  "open a issue on github this is a bug for sure! Please "
                  "attach your F7 code.");
    return;
  }

  code.NewFrame();

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::Variable) {
      // add variables in the scope
      code.AddToStack(entry->name);
    }
  }

  for (auto entry : entries) {
    if (entry->symbolTp == SymbolType::FunctionArg) {
      // add arguments in the scope
      code.AddToStack(entry->name);
    }
  }
}

void RemoveFrame(CodeGenerator &code) { code.RemoveFrame(); }

void PopVariables(const std::shared_ptr<Tree> &tree,
                  const SymbolsTable &symbols, CodeGenerator &code) {

  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetVariablesInFrame(scopeId);

  if (!success) {
    spdlog::error("Congratulazioni! This message should never be seen. There "
                  "is a bug somewhere, please open a issue on github this is a "
                  "bug for sure! Please attach your F7 code.");
    return;
  }

  // for (auto entry : entries) {
  //   if (entry->symbolTp == SymbolType::Variable ||
  //       entry->symbolTp == SymbolType::FunctionArg) {
  //     // remove variables and arguments from the scope
  //     code.RemoveFromStack(entry->name);
  //   }
  // }
}

std::vector<std::bitset<8>> GenerateCode(const Tree &ast,
                                         const SymbolsTable &symbols) {

  CodeGenerator code;
  auto tree = std::make_shared<Tree>(ast);
  CreateFramShape(tree, symbols, code);
  AllocateFrame(tree, symbols, code);
  // TO DO: push space for ret remove it when something is working better
  code.Push();
  code.Push();
  ParseScope(tree, symbols, code);
  RemoveFrame(code);
  PopVariables(tree, symbols, code);
  code.Hlt();

#ifndef NDEBUG
  code.Print();
#endif

  return code.GetCode();
}
