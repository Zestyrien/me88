#include "codegenerator.h"
#include "../../common/opcode.h"
#include <bitset>
#include <iostream>
#include <unordered_map>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

void CodeGenerator::Push() { m_code.push_back((int)Opcode::push_al); }

void CodeGenerator::PushArgument() {
  m_alreadyPushed++;
  m_code.push_back((int)Opcode::push_al);
}

void CodeGenerator::Pop() { m_code.push_back((int)Opcode::pop_al); }

std::size_t CodeGenerator::Size() { return m_code.size(); }

void CodeGenerator::AddToStack(const std::string &variable, SymbolType type) {
  auto frame = &m_frames[m_frames.size() - 1];

  auto var = &frame->varToAddr[variable];
  var->type = type;

  if (type == SymbolType::FunctionArg) {
    var->position.push_back(frame->argCount);
    frame->argCount++;
  } else if (type == SymbolType::Variable) {
    var->position.push_back(frame->varCount);
    frame->varCount++;
  }
}

void CodeGenerator::SetOutOfScope(const std::string &variable) {
  /*if (m_frames[m_frames.size() - 1].varToAddr[variable].size() > 1) {
    m_frames[m_frames.size() - 1].varToAddr[variable].pop_back();
  } else {
    m_frames[m_frames.size() - 1].varToAddr.erase(variable);
  }

  m_frames[m_frames.size() - 1].varCount--;*/
}

void CodeGenerator::RemoveFromStack(const std::string &variable) {
  SetOutOfScope(variable);
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
  m_code.push_back((int)Opcode::mov_sp_ax);

  // this can crash but for now I want it to, so I can debug it
  auto frame = &m_frames[m_frames.size() - 1];
  auto size = frame->varToAddr[variable].position.size();

  if (size < 1) {
    spdlog::error("Error loading variable address for variable {}", variable);
  }

  // the first frame does not have a padding for the return address of the
  // functions if there are multiple frames there is a padding of 2 for each
  // frame
  int offset = m_frames.size() > 1 ? 2 : 0;

  if (frame->varToAddr[variable].type == SymbolType::FunctionArg) {
    offset = offset + frame->varToAddr[variable].position[size - 1];
  } else {
    if (m_pushingArgs) {
      offset += m_alreadyPushed;
    }

    offset = frame->varCount - frame->varToAddr[variable].position[size - 1];
  }

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

void CodeGenerator::NewFunFrame() { m_frames.push_back({}); }

void CodeGenerator::RemoveFrame() { m_frames.pop_back(); }

void CodeGenerator::Hlt() { m_code.push_back((int)Opcode::htl); }

void CodeGenerator::SetPushingFunctionArgs(bool pushing) {
  m_pushingArgs = pushing;
  m_alreadyPushed = 0;
}

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
