#include <gtest/gtest.h>

#include "../instructions/instruction.h"

TEST(InstructionClass, GetTypeFromString) {
  std::string instruction = "%1 = alloca i32, align 4";
  
  EXPECT_TRUE(Instruction::GetType(instruction) == InstructionType::Alloca);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Store);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Load);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinition);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinitionEnd);  
  
  instruction = "store i8 10, i8* %2, align 1";
  
  EXPECT_TRUE(Instruction::GetType(instruction) == InstructionType::Store);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Alloca);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Load);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinition);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinitionEnd);  

  instruction = "%5 = load i8, i8* %3, align 1";
  EXPECT_TRUE(Instruction::GetType(instruction) == InstructionType::Load);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Store);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Alloca);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinition);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinitionEnd);  

  instruction = "define dso_local i32 @main() #0 {";
  EXPECT_TRUE(Instruction::GetType(instruction) == InstructionType::FunctionDefinition);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Store);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Load);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Alloca);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinitionEnd);

  instruction = "}";
  EXPECT_TRUE(Instruction::GetType(instruction) == InstructionType::FunctionDefinitionEnd);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Store);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Load);
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::Alloca);  
  EXPECT_TRUE(Instruction::GetType(instruction) != InstructionType::FunctionDefinition);
}
