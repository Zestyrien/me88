#pragma once

#include <fstream>
#include <string>
#include <optional>

class FileReader
{
public:
  FileReader() = delete;
  FileReader(std::string const &filename);

  std::optional<std::string> ReadNextLine();
  bool HasError();
private:
  std::string m_filename;
  std::ifstream m_file;
  bool m_error = false;
};