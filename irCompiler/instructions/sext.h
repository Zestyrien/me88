#pragma once

#include "instruction.h"

class Sext : public Instruction {
public:
  Sext(std::string const &instruction, int line);

  std::string Print() const final;

  std::string GetDestinationVariable() const;
  std::string GetSourceVariable() const;
private:
  int GetVariableSize() const;
};