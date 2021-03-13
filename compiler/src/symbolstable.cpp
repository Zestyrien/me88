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

  if (funArgs.size() > 0) {
    std::cout << "Arguments: ";

    for (const auto &arg : funArgs) {
      std::cout << arg.first << " " << arg.second << " ";
    }
    std::cout << std::endl;
  }
}

Scope::Scope(int parentId) : m_parentId(parentId), m_isFunction(false) {}

Scope::Scope(int parentId, bool isFunctionScope)
    : m_parentId(parentId), m_isFunction(isFunctionScope){};

void Scope::Print(int id) const {
  std::cout << "------ Scope :" << id << " Parent: " << m_parentId << "------"
            << std::endl;

  for (const auto &sy : m_symbols) {
    sy.second->Print();
  }
}

bool SymbolsTable::AddFunctionDefinition(
    const std::string &funName, const std::string &funType,
    const std::vector<std::pair<std::string, std::string>> &funArgs,
    int scopeId) {
  Symbol newsymbol;
  newsymbol.symbolTp = SymbolType::FunctionDefinition;
  newsymbol.funArgs = funArgs;
  newsymbol.isAssigned = true;
  newsymbol.occurrences = 0;
  newsymbol.name = funName;
  newsymbol.type = funType;

  return AddSymbolToScope(std::make_shared<Symbol>(newsymbol), scopeId);
}

bool SymbolsTable::AddFunctionArgumentToScope(const std::string &argName,
                                              const std::string &argType,
                                              int index, int scopeId) {
  Symbol newsymbol;
  newsymbol.symbolTp = SymbolType::FunctionArg;
  newsymbol.isAssigned = true;
  newsymbol.occurrences = 0;
  newsymbol.name = argName;
  newsymbol.type = argType;
  newsymbol.index = index;

  return AddSymbolToScope(std::make_shared<Symbol>(newsymbol), scopeId);
}

std::tuple<bool, std::vector<std::pair<std::string, std::string>>>
SymbolsTable::GetArgumentsForFunction(const std::string &funName) const {
  for (const auto &scope : m_table) {

    auto symbol = scope.second->m_symbols.find(funName);
    if (symbol != scope.second->m_symbols.end()) {
      return {true, symbol->second->funArgs};
    }
  }

  return {false, std::vector<std::pair<std::string, std::string>>()};
}

void SymbolsTable::AddScope(int scopeId, int parentScopeId) {
  m_table[scopeId] = std::make_shared<Scope>(parentScopeId);
}

void SymbolsTable::AddFunctionScope(int scopeId, int parentScopeId) {
  m_table[scopeId] = std::make_shared<Scope>(parentScopeId, true);
}

std::tuple<bool, std::shared_ptr<Scope>> SymbolsTable::GetScope(int scopeId) const {
  
  auto const it = m_table.find(scopeId);
  if ( it != m_table.end()) {
    return {true, it->second};
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

bool SymbolsTable::AddSymbolOccurrence(
    const std::string &name, int scopeId,
    const std::vector<std::string> &funArgs) {
  auto [success, symbol] = GetSymbol(name, scopeId, funArgs);
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
SymbolsTable::GetSymbol(const std::string &name, int scopeId,
                        const std::vector<std::string> &funArgsTypes) const {
  if (m_table.find(scopeId) == m_table.end()) {
    return {false, nullptr};
  }

  auto scope = m_table.find(scopeId)->second;

  auto symbol = scope->m_symbols.find(name);
  if (symbol == scope->m_symbols.end()) {
    if (scope->m_parentId == 0 && scopeId == 0) {
      return {false, nullptr};
    }

    return GetSymbol(name, scope->m_parentId, funArgsTypes);
  }

  if (symbol->second->symbolTp == SymbolType::FunctionDefinition) {
    // check arguments type
    bool valid = true;
    auto size = funArgsTypes.size();

    if (size == symbol->second->funArgs.size()) {
      for (int i = 0; i < funArgsTypes.size(); i++) {
        if (funArgsTypes[i] != symbol->second->funArgs[i].first) {
          valid = false;
          break;
        }
      }
    } else {
      valid = false;
    }

    if (!valid) {
      spdlog::error("Error in {} function call, argument mismatch.", name);
      return {false, nullptr};
    }
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

std::tuple<bool, std::vector<std::shared_ptr<Symbol>>>
SymbolsTable::GetVariablesInFrame(int scopeId) const {
  std::vector<std::shared_ptr<Symbol>> ret;

  for (const auto scope : m_table) {
    if (scope.first == scopeId) {
      for (auto symbol : scope.second->m_symbols) {
        ret.push_back(symbol.second);
      }
    } else if (!scope.second->m_isFunction &&
               scope.second->m_parentId == scopeId) {
      auto [success, ret2] = GetVariablesInFrame(scope.first);
      if (success) {
        ret.insert(ret.end(), ret2.begin(), ret2.end());
      }
    }
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