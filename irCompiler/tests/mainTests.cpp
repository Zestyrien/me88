#include <gtest/gtest.h>

#include "fileReaderTests.cpp"
#include "symbolTableTests.cpp"
#include "instructionTests.cpp"
#include "instructionFactoryTests.cpp"
#include "allocaTests.cpp"
#include "storeTests.cpp"
#include "loadTests.cpp"
#include "functionTests.cpp"
#include "programGeneratorTests.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}