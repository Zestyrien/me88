#pragma once

#include <fstream>
#include <string>

class FileReader {
public:
  FileReader(std::string const &fileName);
  ~FileReader();

  char GetNextCharacter();
  std::string GetNextLine();

private:
  std::string const m_fileName;
  std::ifstream m_fileStream;
  bool m_error = false;
  bool m_opened = false;

  bool OpenFile();
};