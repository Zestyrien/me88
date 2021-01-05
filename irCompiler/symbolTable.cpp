#include "symbolTable.h"

Symbol SymbolTable::GetSymbol(std::string const &symbolName) {
  if (m_symbols.find(symbolName) != m_symbols.end()) {
    return m_symbols[symbolName];
  }

  return Symbol();
}

void SymbolTable::AddSymbol(std::string const &symbolName) {
  Symbol symbol;
  symbol.name = symbolName;
  symbol.ss = 0;
  symbol.sp = m_index;

  m_index++;

  m_symbols[symbolName] = symbol;
}

void SymbolTable::RemoveSymbol(std::string const &symbolName) {
  m_symbols.erase(symbolName);
}