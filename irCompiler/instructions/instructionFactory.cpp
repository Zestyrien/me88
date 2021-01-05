#include "instructionFactory.h"

std::vector<Instruction *> InstructionFactory::MakeInstructions(
    std::vector<std::string> const &instructionsString) {
  std::vector<Instruction *> instructions;

  int count = 0;
  for (auto const &instStr : instructionsString) {
    count++;
    auto const type = Instruction::GetType(instStr);
    if (type == InstructionType::Unknown) {
      continue;
    }

    Instruction *inst = nullptr;

    switch (type) {
    case InstructionType::Alloca:
      inst = new Alloca(instStr, count);
      break;
    case InstructionType::Store:
      inst = new Store(instStr, count);
      break;
    case InstructionType::Load:
      inst = new Load(instStr, count);
      break;
    case InstructionType::FunctionDefinition:
      inst = new FunctionDefinition(instStr, count);
      break;
    case InstructionType::FunctionDefinitionEnd:
      inst = new FunctionDefinitionEnd(instStr, count);
      break;
    default:
      // TO DO log
      break;
    }

    if(inst) {
      instructions.push_back(inst);
    }
  }

  return instructions;
}