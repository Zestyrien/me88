#include <gtest/gtest.h>

#include "../instructions/alloca.h"

TEST(AllocaClass, GetLineAndGetInstruction) {
  std::string const instruction = "%1 = alloca i32, align 4";
  int const line = 42;
  Alloca allocaInst(instruction, line);

  EXPECT_TRUE(allocaInst.GetLine() == line);
  EXPECT_TRUE(allocaInst.GetInstruction() == instruction);
}

TEST(AllocaClass, GetVariableName) {
  std::string const instruction = "%1 = alloca i8, align 1";
  int const line = 42;
  Alloca allocaInst(instruction, line);

  std::string expectedName = "%1";
  auto const name = allocaInst.GetVariableName();

  EXPECT_TRUE(expectedName == name);
}

TEST(AllocaClass, Print) {
  std::string const instruction = "%1 = alloca i8, align 1";
  int const line = 42;
  Alloca allocaInst(instruction, line);

  std::string expectedPrint = "42 %1 = alloca i8, align 1\nInstruction:\nAlloca VariableName:\t%1\tVariableSize:\t1";
  auto const print = allocaInst.Print();

  EXPECT_TRUE(expectedPrint == print);
}
