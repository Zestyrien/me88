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
  int index;
  std::string name;
  std::string type;
  std::vector<std::pair<std::string, std::string>> funArgs;
  void Print() const;
};

struct Scope {
  Scope() = delete;
  Scope(int parentId);
  Scope(int parentId, bool isFunctionScope);
  int m_parentId;
  bool m_isFunction;
  std::unordered_map<std::string, std::shared_ptr<Symbol>> m_symbols;
  void Print(int id) const;
};

class SymbolsTable {
public:
  bool IsFunctionArgument(std::string const &variable, int const &scopeId) const;
  bool AddFunctionDefinition(
      const std::string &funName, const std::string &funType,
      const std::vector<std::pair<std::string, std::string>> &funArgs,
      int scopeId);
  bool AddFunctionArgumentToScope(const std::string &argName,
                                  const std::string &argType, int index,
                                  int scopeId);
  std::tuple<bool, std::vector<std::pair<std::string, std::string>>>
  GetArgumentsForFunction(const std::string &funName) const;
  void AddScope(int scopeId, int parentScopeId);
  void AddFunctionScope(int scopeId, int parentScopeId);
  std::tuple<bool, std::shared_ptr<Scope>> GetScope(int scopeId) const;
  bool AddVariableSymbolToScope(const std::string &varType,
                                const std::string &varName, int scopeId);
  bool AddSymbolToScope(std::shared_ptr<Symbol> sy, int scopeId);
  bool AddSymbolOccurrence(
      const std::string &name, int scopeId,
      const std::vector<std::string> &funArgs = std::vector<std::string>());
  bool SetVariableAssigend(const std::string &name, int scopeId);
  std::tuple<bool, std::shared_ptr<Symbol>>
  GetSymbol(const std::string &name, int scopeId,
            const std::vector<std::string> &funArgsTypes =
                std::vector<std::string>()) const;
  std::tuple<bool, std::vector<std::shared_ptr<Symbol>>>
  GetSymbolsInScope(int scopeId) const;
  std::tuple<bool, std::vector<std::shared_ptr<Symbol>>>
  GetVariablesInFrame(int scopeId) const;
  bool IsDeclared(int scopeId, const std::string &symbolName) const;
  bool IsAssigned(int scopeId, const std::string &symbolName) const;
  std::string GetSymbolStrType(int scopeId,
                               const std::string &symbolName) const;
  void Print() const;

private:
  std::unordered_map<int, std::shared_ptr<Scope>> m_table;
};
