#include <gtest/gtest.h>

#include "../instructions/functionDefinition.h"

TEST(FunctionDefinitionClass, GetFunctionName) {
  std::string const instruction = "define dso_local i32 @main() #0 {";
  int const line = 42;
  FunctionDefinition functionDef(instruction, line);

  std::string const expectedFunctionName = "main";
  auto const functionName = functionDef.GetFunctionName();

  EXPECT_TRUE(expectedFunctionName == functionName);
}

TEST(FunctionDefinitionClass, Print) {
  std::string const instruction = "define dso_local i32 @main() #0 {";
  int const line = 42;
  FunctionDefinition functionDef(instruction, line);

  std::string const expectedPrint =
      "42 define dso_local i32 @main() #0 {\nInstruction:\nFunctionDefinition "
      "FunctionName:\tmain";
  auto const print = functionDef.Print();

  EXPECT_TRUE(expectedPrint == print);
}

TEST(FunctionDefinitionEndClass, Print) {
  std::string const instruction = "}";
  int const line = 42;
  FunctionDefinitionEnd functionDefEnd(instruction, line);

  std::string const expectedPrint = "42 }\nInstruction:\nFunctionDefinitionEnd";
  auto const print = functionDefEnd.Print();
  
  EXPECT_TRUE(expectedPrint == print);
}