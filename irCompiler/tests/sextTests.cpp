#include <gtest/gtest.h>

#include "../instructions/sext.h"

TEST(SextClass, GetLineAndGetInstruction) {
  std::string const instruction = "%6 = sext i8 %5 to i32";
  int const line = 42;
  Sext sextInst(instruction, line);

  EXPECT_TRUE(sextInst.GetLine() == line);
  EXPECT_TRUE(sextInst.GetInstruction() == instruction);
}

TEST(SextClass, GetSourceVariable) {
  std::string const instruction = "%6 = sext i8 %5 to i32";
  int const line = 42;
  Sext sextInst(instruction, line);

  std::string expectedValue = "%5";
  auto const value = sextInst.GetSourceVariable();

  EXPECT_TRUE(expectedValue == value);
}

TEST(SextClass, GetDestinationVariable) {
  std::string const instruction = "%6 = sext i8 %5 to i32";
  int const line = 42;
  Sext sextInst(instruction, line);

  std::string expectedValue = "%6";
  auto const value = sextInst.GetDestinationVariable();

  EXPECT_TRUE(expectedValue == value);
}

TEST(SextClass, Print) {
  std::string const instruction = "%6 = sext i8 %5 to i32";
  int const line = 42;
  Sext sextInst(instruction, line);

  std::string expectedPrint =
      "42 %6 = sext i8 %5 to i32"
      "\nInstruction:\tSext\nSource:\t%5\nDestination:\t%6\nVariableSize:\t4";
  auto const print = sextInst.Print();

  EXPECT_TRUE(expectedPrint == print);
}
