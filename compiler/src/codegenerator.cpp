#include "codegenerator.h"
#include "../../common/opcode.h"
#include <bitset>
#include <iostream>
#include <unordered_map>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

void CodeGenerator::Push() {
  m_sp = m_sp.to_ulong() - 1;
  m_code.push_back((int)Opcode::push_al);
}

void CodeGenerator::Pop() {
  m_code.push_back((int)Opcode::pop_al);
  m_sp = m_sp.to_ulong() + 1;
}

std::size_t CodeGenerator::Size() { return m_code.size(); }

void CodeGenerator::AddToStack(const std::string &variable) {
  Push();
  m_varToAddr[variable] = m_sp;
}

void CodeGenerator::RemoveFromStack(const std::string &variable) {
  m_varToAddr.erase(variable);
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

  // set ds
  m_code.push_back((int)Opcode::mov_ss_ax);
  m_code.push_back((int)Opcode::mov_ax_ds);

  // set di
  LoadAX(m_varToAddr[variable].to_ulong());
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

void CodeGenerator::EvalBelow(int op) {
  Cmp(op);
  m_code.push_back((int)Opcode::jb_cs_offset);
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
    }
  } else {
    if (ndvalue == "+") {
      code.Add(std::stoi(lfvalue));
    } else if (ndvalue == "-") {
      code.Sub(std::stoi(lfvalue));
    } else if (ndvalue == ">") {
      code.EvalBelow(std::stoi(lfvalue));
    }
  }
}

void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolsTable &symbols,
                CodeGenerator &code);

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

void ParseNode(const std::shared_ptr<Node> &node, const SymbolsTable &symbols,
               CodeGenerator &code) {
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
  default:
    spdlog::error("Invalid node type in tree: type {} value {}",
                  node->GetType(), node->GetValue());
    break;
  }
}

void ParseScope(const std::shared_ptr<Tree> &tree, const SymbolsTable &symbols,
                CodeGenerator &code) {
  auto scopeId = tree->GetScopeId();
  auto [success, entries] = symbols.GetSymbolsInScope(scopeId);

  if (!success) {
    // TO DO error
  }

  for (auto entry : entries) {
    // add variables in the scope
    code.AddToStack(entry->name);
  }

  for (auto node : tree->GetNodes()) {
    ParseNode(node, symbols, code);
  }

  for (auto entry : entries) {
    // remove variables from the scope
    code.RemoveFromStack(entry->name);
  }
}

std::vector<std::bitset<8>>
GenerateCode(const Tree &ast, const SymbolsTable &symbols, bool debug) {

  CodeGenerator code;
  ParseScope(std::make_shared<Tree>(ast), symbols, code);
  code.Hlt();

  if (debug) {
    code.Print();
  }

  return code.GetCode();
}
