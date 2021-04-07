#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include "filereader.h"

std::string const filesPath = "../compiler/src/tests/files/";
std::string const astPath = filesPath + "ast/";
std::string const irPath = filesPath + "ir/";
std::string const binPath = filesPath + "bin/";

auto const PrintBitset = [](std::vector<std::bitset<8>> const &code) {
  for (auto const &entry : code)
  {
    std::cout << entry.to_string() << std::endl;
  }
};

auto const PrintStrVector = [](std::vector<std::string> const &code) {
  for (auto const &entry : code)
  {
    std::cout << entry << std::endl;
  }
};

std::vector<std::string> ReadFile(std::string const &filename)
{
  FileReader reader(filename);

  std::vector<std::string> file;

  while (!reader.HasError())
  {
    auto const line = reader.ReadNextLine();
    if (line.has_value())
    {
      file.push_back(line.value());
    }
  }

  return file;
}

std::vector<std::bitset<8>> ReadFileAsBitset(std::string const &filename)
{
  std::vector<std::string> file = ReadFile(filename);
  std::vector<std::bitset<8>> fileBitset;

  for (auto const &s : file)
  {
    // TO DO 
    std::bitset<8> b(std::stoi(s, nullptr, 2));
    fileBitset.push_back(b);
  }

  return fileBitset;
}