#pragma once

#include "instruction.h"

class Alloca : public Instruction {
public:
  Alloca(std::string const &instruction, int line);

  std::string Print() const final;
  
  int GetVariableSize() const;
  std::string GetVariableName() const;
};