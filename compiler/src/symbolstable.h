#pragma once

#include "tree.h"
#include <string>
#include <tuple>
#include <unordered_map>

enum class SymbolType {
  Variable = 0,
  FunctionDefinition,
  FunctionCall,
  FunctionArg
};

struct Symbol {
  SymbolType symbolTp;
  bool isAssigned;
  int occurrences;
  std::string name;
  std::string type;
  void Print() const;
};

struct Scope {
  Scope() = delete;
  Scope(int parentId);
  int m_parentId;
  std::unordered_map<std::string, std::shared_ptr<Symbol>> m_symbols;
  void Print(int id) const;
};

class SymbolsTable {
public:
  void AddScope(int scopeId, int parentScopeId);
  std::tuple<bool, std::shared_ptr<Scope>> GetScope(int scopeId);
  bool AddVariableSymbolToScope(const std::string &varType,
                                const std::string &varName, int scopeId);
  bool AddSymbolToScope(std::shared_ptr<Symbol> sy, int scopeId);
  bool AddSymbolOccurrence(const std::string &name, int scopeId);
  bool SetVariableAssigend(const std::string &name, int scopeId);
  std::tuple<bool, std::shared_ptr<Symbol>> GetSymbol(const std::string &name,
                                                      int scopeId) const;
  std::tuple<bool, std::vector<std::shared_ptr<Symbol>>>
  GetSymbolsInScope(int scopeId) const;
  bool IsDeclared(int scopeId, const std::string &symbolName) const;
  bool IsAssigned(int scopeId, const std::string &symbolName) const;
  std::string GetSymbolStrType(int scopeId,
                               const std::string &symbolName) const;
  void Print() const;

private:
  std::unordered_map<int, std::shared_ptr<Scope>> m_table;
};
