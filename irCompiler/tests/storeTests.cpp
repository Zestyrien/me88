#include <gtest/gtest.h>

#include "../instructions/store.h"

TEST(StoreClass, GetLineAndGetInstruction) {
  std::string const instruction = "store i32 0, i32* %1, align 4";
  int const line = 42;
  Store storeInst(instruction, line);

  EXPECT_TRUE(storeInst.GetLine() == line);
  EXPECT_TRUE(storeInst.GetInstruction() == instruction);
}

TEST(StoreClass, GetVariableName) {
  std::string const instruction = "store i8 10, i8* %1, align 1";
  int const line = 42;
  Store storeInst(instruction, line);

  std::string expectedName = "%1";
  auto const name = storeInst.GetVariableName();

  EXPECT_TRUE(expectedName == name);
}

TEST(StoreClass, GetValue) {
  std::string const instruction = "store i32 0, i32* %1, align 4";
  int const line = 42;
  Store storeInst(instruction, line);

  std::string expectedValue = "0";
  auto const value = storeInst.GetVariableValue();

  EXPECT_TRUE(expectedValue == value);
}

TEST(StoreClass, Print) {
  std::string const instruction = "store i32 0, i32* %1, align 4";
  int const line = 42;
  Store storeInst(instruction, line);

  std::string expectedPrint = "42 store i32 0, i32* %1, align "
                              "4\nInstruction:\tStore\nVariableName:\t%"
                              "1\nVariableSize:\t4\nVariableValue:\t0";
  auto const print = storeInst.Print();

  EXPECT_TRUE(expectedPrint == print);
}
