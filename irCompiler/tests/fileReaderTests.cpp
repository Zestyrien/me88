#include <gtest/gtest.h>

#include "../fileReader.h"

TEST(FileReaderClass, GetNextCharacter_ReturnsEOFOnNonExistentFile) {
  FileReader reader("BikerMice!");

  EXPECT_TRUE(EOF == reader.GetNextCharacter());
}

TEST(FileReaderClass, GetNextCharacter_ReturnsValidCaractersOnExistentFile) {
  FileReader reader("../irCompiler/tests/files/fileReader.txt");

  std::string const expectedResult = "Let's rock and ride!";
  std::string result;

  char c = reader.GetNextCharacter();
  while (c != EOF) {
    result += c;
    c = reader.GetNextCharacter();
  }

  EXPECT_TRUE(result == expectedResult);
}

TEST(FileReaderClass, GetNextLine_ReturnsEmptyStringOnNonExistentFile) {
  FileReader reader("BikerMice!");

  EXPECT_TRUE("" == reader.GetNextLine());
}

TEST(FileReaderClass, GetNextLine_ReturnsValidLineOnExistentFile) {
  FileReader reader("../irCompiler/tests/files/fileReader.txt");
  
  std::string const expectedResult = "Let's rock and ride!";
  std::string result = reader.GetNextLine();

  EXPECT_TRUE(result == expectedResult);
}