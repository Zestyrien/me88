#include "variableTable.h"

std::tuple<bool, Variable> VariableTable::GetVariable(std::string const &name) {
  if (m_variables.find(name) != m_variables.end()) {
    return {true, m_variables[name]};
  }

  // TO DO log
  return {false, {}};
}

bool VariableTable::AddVariable(std::string const &varName,
                                unsigned short const &sS,
                                unsigned short const &sP, int const &size) {

  if (m_variables.find(varName) != m_variables.end()) {
    // TO DO log
    return false;
  }

  Variable var;
  var.name = varName;
  var.ss = sS;
  var.sp = sP;
  var.size = size;

  m_variables[varName] = var;

  return true;
}