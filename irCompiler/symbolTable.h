#pragma once

#include <string>
#include <unordered_map>

struct Symbol {
  unsigned short ss = 0;
  unsigned short sp = 0;
  std::string name;
};

class SymbolTable {
public:
  Symbol GetSymbol(std::string const &symbolName);
  void AddSymbol(std::string const &symbolName);
  void RemoveSymbol(std::string const &symbolName);

private:
  std::unordered_map<std::string, Symbol> m_symbols;
  unsigned short m_index = 0;
};