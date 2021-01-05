#include <gtest/gtest.h>

#include <bitset>

#include "../fileReader.h"
#include "../instructions/alloca.h"
#include "../instructions/functionDefinition.h"
#include "../instructions/store.h"
#include "../programGenerator.h"

TEST(ProgramGeneratorClass, GetAssemblyMainReturnZeroLL) {
  FileReader llReader("../irCompiler/tests/files/mainReturnsZero.me88asm");

  std::vector<std::string> expectedResult;
  while (true) {
    std::string line = llReader.GetNextLine();
    if (line == "") {
      break;
    }

    expectedResult.push_back(line);
  }

  FileReader asmReader("../irCompiler/tests/files/mainReturnsZero.ll");

  std::vector<Instruction *> instructions;
  while (true) {
    std::string line = asmReader.GetNextLine();
    if (line == "") {
      break;
    }

    auto const type = Instruction::GetType(line);
    if (type == InstructionType::Unknown) {
      continue;
    }

    Instruction *inst = nullptr;

    switch (type) {
    case InstructionType::Alloca:
      inst = new Alloca(line, 0);
      break;
    case InstructionType::Store:
      inst = new Store(line, 0);
      break;
    case InstructionType::FunctionDefinition:
      inst = new FunctionDefinition(line, 0);
      break;
    case InstructionType::FunctionDefinitionEnd:
      inst = new FunctionDefinitionEnd(line, 0);
      break;
    default:
      // TO DO log
      break;
    }

    if (inst) {
      instructions.push_back(inst);
    }
  }

  auto generator = ProgramGenerator(instructions);
  auto const result = generator.GetAssembly();

  EXPECT_TRUE(result.size() == expectedResult.size());

  auto const size = result.size();
  if (size == expectedResult.size()) {
    for (size_t i = 0; i < size; i++) {
      if (result[i] != expectedResult[i]) {
        EXPECT_TRUE(false);
      }
    }
  }
}

TEST(ProgramGeneratorClass, GetBinaryMainReturnZeroLL) {
  FileReader llReader("../irCompiler/tests/files/mainReturnsZero.me88bin");

  std::vector<std::bitset<8>> expectedResult;
  while (true) {
    std::string line = llReader.GetNextLine();
    if (line == "") {
      break;
    }
    std::bitset<8> bits(line);
    expectedResult.push_back(bits);
  }

  FileReader asmReader("../irCompiler/tests/files/mainReturnsZero.ll");

  std::vector<Instruction *> instructions;
  while (true) {
    std::string line = asmReader.GetNextLine();
    if (line == "") {
      break;
    }

    auto const type = Instruction::GetType(line);
    if (type == InstructionType::Unknown) {
      continue;
    }

    Instruction *inst = nullptr;

    switch (type) {
    case InstructionType::Alloca:
      inst = new Alloca(line, 0);
      break;
    case InstructionType::Store:
      inst = new Store(line, 0);
      break;
    case InstructionType::FunctionDefinition:
      inst = new FunctionDefinition(line, 0);
      break;
    case InstructionType::FunctionDefinitionEnd:
      inst = new FunctionDefinitionEnd(line, 0);
      break;
    default:
      // TO DO log
      break;
    }

    if (inst) {
      instructions.push_back(inst);
    }
  }

  auto generator = ProgramGenerator(instructions);
  auto const result = generator.GetBinary();

  EXPECT_TRUE(result.size() == expectedResult.size());

  auto const size = result.size();
  if (size == expectedResult.size()) {
    for (size_t i = 0; i < size; i++) {
      if (result[i] != expectedResult[i]) {
        EXPECT_TRUE(false);
      }
    }
  }
}