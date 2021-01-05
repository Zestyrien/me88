#include <gtest/gtest.h>

#include "../instructions/load.h"

TEST(LoadClass, GetLineAndGetInstruction) {
  std::string const instruction = "%5 = load i8, i8* %3, align 1";
  int const line = 42;
  Load loadInst(instruction, line);

  EXPECT_TRUE(loadInst.GetLine() == line);
  EXPECT_TRUE(loadInst.GetInstruction() == instruction);
}

TEST(LoadClass, GetSourceVariable) {
  std::string const instruction = "%5 = load i8, i8* %3, align 1";
  int const line = 42;
  Load loadInst(instruction, line);

  std::string expectedValue = "%3";
  auto const value = loadInst.GetSourceVariable();

  EXPECT_TRUE(expectedValue == value);
}

TEST(LoadClass, GetDestinationVariable) {
  std::string const instruction = "%5 = load i8, i8* %3, align 1";
  int const line = 42;
  Load loadInst(instruction, line);

  std::string expectedValue = "%5";
  auto const value = loadInst.GetDestinationVariable();

  EXPECT_TRUE(expectedValue == value);
}

TEST(LoadClass, Print) {
  std::string const instruction = "%5 = load i8, i8* %3, align 1";
  int const line = 42;
  Load loadInst(instruction, line);

  std::string expectedPrint =
      "42 %5 = load i8, i8* %3, align "
      "1\nInstruction:\tLoad\nSource:\t%3\nDestination:\t%5\nVariableSize:\t1";
  auto const print = loadInst.Print();

  EXPECT_TRUE(expectedPrint == print);
}
