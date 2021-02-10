#include "programGenerator.h"

#include <regex>

#include "../common/opcode.h"

ProgramGenerator::ProgramGenerator(
    std::vector<Instruction *> const &instructions)
    : m_instructions(instructions) {}

ProgramGenerator::~ProgramGenerator() {
  for (auto const &inst : m_instructions) {
    if (inst) {
      delete inst;
    }
  }

  if (m_varTable) {
    delete m_varTable;
  }

  if (m_stackTracker) {
    delete m_stackTracker;
  }
}

std::vector<std::bitset<8>> ProgramGenerator::GetBinary() {
  std::vector<std::bitset<8>> binary;

  auto const assemby = GetAssembly();
  for (auto const &inst : assemby) {
    if (std::regex_match(inst, std::regex("([0-1]{8})"))) {
      binary.push_back(std::bitset<8>(inst));
    }

    auto const [success, opcode] = StringToOpcode(inst);
    if (success) {
      binary.push_back(std::bitset<8>((int)opcode));
    }
  }

  return binary;
}

std::vector<std::string> ProgramGenerator::GetAssembly() {
  if (m_stackTracker) {
    delete m_stackTracker;
  }

  if (m_varTable) {
    delete m_varTable;
  }

  m_stackTracker = new StackTracker();
  m_varTable = new VariableTable();

  std::vector<std::string> assembly;

  for (auto const &inst : m_instructions) {
    switch (inst->GetType()) {
    case InstructionType::FunctionDefinition:
      assembly.push_back(HandleFunctionDefinition((FunctionDefinition *)inst));
      break;
    case InstructionType::Alloca: {
      auto const ret = HandleAlloca((Alloca *)inst);
      assembly.insert(assembly.end(), ret.begin(), ret.end());
      break;
    }
    case InstructionType::Store: {
      auto const ret = HandleStore((Store *)inst);
      assembly.insert(assembly.end(), ret.begin(), ret.end());
      break;
    }
    case InstructionType::Load: {
      auto const ret = HandleLoad((Load *)inst);
      assembly.insert(assembly.end(), ret.begin(), ret.end());
      break;
    }
    case InstructionType::Sext: {
      auto const ret = HandleSext((Sext *)inst);
      assembly.insert(assembly.end(), ret.begin(), ret.end());
      break;
    }
    case InstructionType::FunctionDefinitionEnd: {
      auto const frame = m_stackTracker->GetFrame();
      assembly.push_back("# clean up");
      for (int i = 0; i < frame.varCount; i++) {
        assembly.push_back(OpcodeToString(Opcode::pop_al));
      }
      break;
    }
    default:
      // TO DO log
      break;
    }
  }

  return assembly;
}

std::string
ProgramGenerator::HandleFunctionDefinition(FunctionDefinition *def) const {
  auto const name = def->GetFunctionName();
  m_stackTracker->AddFrame(name);
  return name + ":";
}

std::vector<std::string> ProgramGenerator::HandleAlloca(Alloca *alloc) const {
  std::vector<std::string> alloca;
  alloca.push_back("#" + alloc->GetInstruction());

  auto const size = alloc->GetVariableSize();
  for (int i = 0; i < size; i++) {
    alloca.push_back(OpcodeToString(Opcode::push_al));
  }

  auto const frame = m_stackTracker->GetFrame();
  m_varTable->AddVariable(alloc->GetVariableName(), 0, frame.varCount, size);
  m_stackTracker->Push(size);

  return alloca;
}

std::vector<std::string> ProgramGenerator::HandleStore(Store *store) const {
  std::vector<std::string> storeCode;

  storeCode.push_back("#" + store->GetInstruction());

  // TO DO the following assumes local variables

  // put ss in ds to select the current segment
  storeCode.push_back(OpcodeToString(Opcode::mov_ss_ax));
  storeCode.push_back(OpcodeToString(Opcode::mov_ax_ds));

  storeCode.push_back(OpcodeToString(Opcode::mov_sp_ax));
  // sp will point to the latest push: need the relative displacement of a
  // variable and add it to sp
  storeCode.push_back(OpcodeToString(Opcode::add_operand_al));

  auto const [success, variable] =
      m_varTable->GetVariable(store->GetVariableName());
  if (!success) {
    // TO DO log;
    return storeCode;
  }

  auto const frame = m_stackTracker->GetFrame();
  auto const displacement = frame.varCount - 1 - variable.sp;

  std::bitset<8> const bits = displacement;
  storeCode.push_back(bits.to_string());

  // move the value of ax to di, now ds:(di) points to the first 8 bits of the
  // variable
  storeCode.push_back(OpcodeToString(Opcode::mov_ax_di));

  // TO DO this is assuming number not variable in the store
  auto const value = store->GetVariableValue();
  int operand;
  try {
    operand = atoi(value.c_str());
  } catch (...) {
    operand = 0;
  }

  for (int i = 0; i < variable.size; i++) {
    if (i > 0) {
      storeCode.push_back(OpcodeToString(Opcode::mov_di_ax));
      storeCode.push_back(OpcodeToString(Opcode::sub_operand_al));
      std::bitset<8> const bits = 1;
      storeCode.push_back(bits.to_string());
      storeCode.push_back(OpcodeToString(Opcode::mov_ax_di));
    }

    // load what we need to write in al
    storeCode.push_back(OpcodeToString(Opcode::mov_operand_al));
    // shift if the size of the variable is more than 1
    int const shift = (variable.size - 1 - i) * 8;
    std::bitset<8> offPrint = operand >> shift;
    storeCode.push_back(offPrint.to_string());
    storeCode.push_back(OpcodeToString(Opcode::mov_al_ds$di));
  }

  return storeCode;
}

std::vector<std::string> ProgramGenerator::HandleLoad(Load *load) const {
  std::vector<std::string> loadCode;
  loadCode.push_back("#" + load->GetInstruction());

  // TO DO the following assumes local variables

  // put ss in ds to select the current segment
  loadCode.push_back(OpcodeToString(Opcode::mov_ss_ax));
  loadCode.push_back(OpcodeToString(Opcode::mov_ax_ds));

  loadCode.push_back(OpcodeToString(Opcode::mov_sp_ax));

  auto const [successSource, source] =
      m_varTable->GetVariable(load->GetSourceVariable());
  if (!successSource) {
    // TO DO log;
    return loadCode;
  }

  auto const frame = m_stackTracker->GetFrame();
  auto const sourceOffset = frame.varCount - 1 - source.sp;

  auto const [successDest, destination] =
      m_varTable->GetVariable(load->GetDestinationVariable());
  if (!successDest) {
    // TO DO log;
    return loadCode;
  }

  auto const destinationOffset = frame.varCount - 1 - destination.sp;

  for (int i = 0; i < source.size; i++) {

    // sp will point to the latest push: need the relative displacement of a
    // variable and add it to sp
    // this will put the value in al
    loadCode.push_back(OpcodeToString(Opcode::mov_ds$offset_al));
    // set the offset
    std::bitset<8> const offsetH = (sourceOffset - i) >> 8;
    std::bitset<8> const offsetL = (sourceOffset - i);
    loadCode.push_back(offsetH.to_string());
    loadCode.push_back(offsetL.to_string());

    // save al to point di to the variable to write to
    loadCode.push_back(OpcodeToString(Opcode::push_al));

    // point to the bottom of the stack
    loadCode.push_back(OpcodeToString(Opcode::mov_sp_ax));

    // add displacement
    loadCode.push_back(OpcodeToString(Opcode::add_operand_al));
    // add 1 because of the push
    std::bitset<8> const offsetDestBits = (destinationOffset - i + 1);
    loadCode.push_back(offsetDestBits.to_string());

    loadCode.push_back(OpcodeToString(Opcode::mov_ax_di));

    // retreive value from the stack
    loadCode.push_back(OpcodeToString(Opcode::pop_al));

    // write value
    loadCode.push_back(OpcodeToString(Opcode::mov_al_ds$di));
  }

  return loadCode;
}

std::vector<std::string> ProgramGenerator::HandleSext(Sext *sext) const {
  std::vector<std::string> sextCode;
  return sextCode;
}
