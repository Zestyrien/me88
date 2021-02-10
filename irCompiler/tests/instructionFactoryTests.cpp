#include <gtest/gtest.h>

#include "../instructions/instructionFactory.h"

TEST(InstructionFactory, MakeInstructions) {

  std::vector<std::string> instructionStr;
  instructionStr.push_back("%1 = alloca i32, align 4");
  instructionStr.push_back("store i8 10, i8* %2, align 1");
  instructionStr.push_back("%5 = load i8, i8* %3, align 1");
  instructionStr.push_back("%6 = sext i8 %5 to i32");
  instructionStr.push_back("define dso_local i32 @main() #0 {");
  instructionStr.push_back("}");

  std::vector<Instruction *> expectedResult;
  expectedResult.push_back(new Alloca(instructionStr[0], 0));
  expectedResult.push_back(new Store(instructionStr[1], 1));
  expectedResult.push_back(new Load(instructionStr[2], 2));
  expectedResult.push_back(new Sext(instructionStr[3], 3));
  expectedResult.push_back(new FunctionDefinition(instructionStr[4], 4));
  expectedResult.push_back(new FunctionDefinitionEnd(instructionStr[5], 5));

  std::vector<Instruction *> result =
      InstructionFactory::MakeInstructions(instructionStr);

  auto const size = result.size();
  EXPECT_TRUE(size == expectedResult.size());

  if (size == expectedResult.size()) {
    for (size_t i = 0; i < size; i++) {
      EXPECT_TRUE(result[i]->GetInstruction() ==
                  expectedResult[i]->GetInstruction());
      EXPECT_TRUE(result[i]->GetType() == expectedResult[i]->GetType());
    }
  }

  for (auto const &inst : expectedResult) {
    if (inst) {
      delete inst;
    }
  }

  for (auto const &inst : result) {
    if (inst) {
      delete inst;
    }
  }
}
