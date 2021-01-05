#pragma once

#include "instruction.h"

class Store : public Instruction {
public:
  Store(std::string const &instruction, int line);

  std::string Print() const final;

  std::string GetVariableName() const;
  std::string GetVariableValue() const;
private:
  int GetVariableSize() const;
};