#pragma once

#include <string>
#include <unordered_map>

struct Variable {
  unsigned short ss;
  unsigned short sp;
  int size;
  std::string name;
};

class VariableTable {
public:
  std::tuple<bool, Variable> GetVariable(std::string const &name);
  bool AddVariable(std::string const &varName, unsigned short const &sS,
                   unsigned short const &sP, int const& size);

private:
  std::unordered_map<std::string, Variable> m_variables;
};