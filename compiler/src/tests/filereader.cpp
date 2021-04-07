#include "filereader.h"

FileReader::FileReader(std::string const &filename) : m_filename(filename), m_file(filename) {}

std::optional<std::string> FileReader::ReadNextLine()
{
  if (m_error)
  {
    return std::nullopt;
  }

  if (!m_file.is_open())
  {
    m_file.open(m_filename);
  }
  
  std::string ret;
  if (!std::getline(m_file, ret))
  {
    m_error = true;
    return std::nullopt;
  }

  return ret;
}

bool FileReader::HasError()
{
  return m_error;
}