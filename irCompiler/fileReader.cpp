#include "fileReader.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

FileReader::FileReader(std::string const &fileName) : m_fileName(fileName) {}
FileReader::~FileReader() {
  if (m_fileStream.is_open()) {
    m_fileStream.close();
  }
}

char FileReader::GetNextCharacter() {
  if (m_error) {
    return EOF;
  }

  if (!OpenFile()) {
    return EOF;
  }

  return m_fileStream.get();
}

std::string FileReader::GetNextLine() {
  if (m_error) {
    return "";
  }

  if (!OpenFile()) {
    return "";
  }

  std::string line;
  char c;
  while (m_fileStream.get(c)) {
    if (c == EOF || (c == '\n' && line.length() != 0)) {
      break;
    }

    line += c;

    if(c == '\n') {
      line = "";
    }
  }

  return line;
}

bool FileReader::OpenFile() {
  if (!m_opened) {
    m_fileStream.open(m_fileName);
    m_opened = true;

    if (!m_fileStream.is_open()) {
      m_error = true;
      spdlog::error("Can't open file.");
      return false;
    };
  }

  return true;
}