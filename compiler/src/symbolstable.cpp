#include "symbolstable.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <iostream>

std::string PrintSymbolType(SymbolType tp) {
  switch (tp) {
  case SymbolType::Variable:
    return "Variable";
  case SymbolType::FunctionDefinition:
    return "FunctionDefinition";
  case SymbolType::FunctionCall:
    return "FunctionCall";
  case SymbolType::FunctionArg:
    return "FunctionArg";
  default:
    return "Unknown";
  }
}

void Symbol::Print() const {
  std::cout << "** " << PrintSymbolType(symbolTp) << " " << type << " " << name
            << " Occurrences: " << occurrences
            << " Assigned: " << (isAssigned ? "true" : "false") << std::endl;
}

Scope::Scope(int parentId) : m_parentId(parentId) {}

void Scope::Print(int id) const {
  std::cout << "------ Scope :" << id << " Parent: " << m_parentId << "------"
            << std::endl;

  for (const auto &sy : m_symbols) {
    sy.second->Print();
  }
}

void SymbolsTable::AddScope(int scopeId, int parentScopeId) {
  m_table[scopeId] = std::make_shared<Scope>(parentScopeId);
}

std::tuple<bool, std::shared_ptr<Scope>> SymbolsTable::GetScope(int scopeId) {
  if (m_table.find(scopeId) != m_table.end()) {
    return {true, m_table[scopeId]};
  }

  return {false, nullptr};
}

bool SymbolsTable::AddVariableSymbolToScope(const std::string &varType,
                                            const std::string &varName,
                                            int scopeId) {

  Symbol newsymbol;
  newsymbol.symbolTp = SymbolType::Variable;
  newsymbol.isAssigned = false;
  newsymbol.occurrences = 0;
  newsymbol.name = varName;
  newsymbol.type = varType;

  return AddSymbolToScope(std::make_shared<Symbol>(newsymbol), scopeId);
}

bool SymbolsTable::AddSymbolToScope(std::shared_ptr<Symbol> newsymbol,
                                    int scopeId) {

  auto [success, scope] = GetScope(scopeId);
  if (!success) {
    spdlog::error("{} {} scope not found.", newsymbol->type, newsymbol->name);
  }

  if (scope->m_symbols.find(newsymbol->name) != scope->m_symbols.end()) {
    spdlog::error("{} {} is already defined.", newsymbol->type,
                  newsymbol->name);
    return false;
  }

  scope->m_symbols[newsymbol->name] = newsymbol;
  return true;
}

bool SymbolsTable::AddSymbolOccurrence(const std::string &name, int scopeId) {
  auto [success, symbol] = GetSymbol(name, scopeId);
  if (success) {
    symbol->occurrences++;
  } else {
    spdlog::error("{} is not defined.", name);
  }
  return success;
}

bool SymbolsTable::SetVariableAssigend(const std::string &name, int scopeId) {
  auto [success, symbol] = GetSymbol(name, scopeId);
  if (success) {
    symbol->isAssigned = true;
  } else {
    spdlog::error("Assignement of {}, {} is not defined.", name, name);
  }
  return success;
}

std::tuple<bool, std::shared_ptr<Symbol>>
SymbolsTable::GetSymbol(const std::string &name, int scopeId) const {
  if (m_table.find(scopeId) == m_table.end()) {
    return {false, nullptr};
  }

  auto scope = m_table.find(scopeId)->second;

  if (scope->m_symbols.find(name) == scope->m_symbols.end()) {

    if (scope->m_parentId == 0 && scopeId == 0) {
      return {false, nullptr};
    }

    return GetSymbol(name, scope->m_parentId);
  }

  return {true, scope->m_symbols[name]};
}

std::tuple<bool, std::vector<std::shared_ptr<Symbol>>>
SymbolsTable::GetSymbolsInScope(int scopeId) const {
  std::vector<std::shared_ptr<Symbol>> ret;

  if (m_table.find(scopeId) == m_table.end()) {
    return {false, ret};
  }
  auto scope = m_table.find(scopeId)->second;
  for (auto symbol : scope->m_symbols) {
    ret.push_back(symbol.second);
  }

  return {true, ret};
}

bool SymbolsTable::IsDeclared(int scopeId,
                              const std::string &symbolName) const {
  auto [success, sy] = GetSymbol(symbolName, scopeId);
  return success;
}

bool SymbolsTable::IsAssigned(int scopeId,
                              const std::string &symbolName) const {
  auto [success, sy] = GetSymbol(symbolName, scopeId);
  if (!success) {
    return false;
  }

  sy->isAssigned = true;
  return true;
}

std::string
SymbolsTable::GetSymbolStrType(int scopeId,
                               const std::string &symbolName) const {
  auto [success, sy] = GetSymbol(symbolName, scopeId);
  // TO DO Error ?
  if (!success) {
    return "";
  }

  return sy->type;
}

void SymbolsTable::Print() const {
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++"
            << std::endl;
  std::cout << "-------------------SYMBOLS  TABLE-------------------"
            << std::endl;
  for (const auto &sc : m_table) {
    sc.second->Print(sc.first);
  }
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++"
            << std::endl;
}