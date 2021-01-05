#pragma once

#include "instruction.h"

class Load : public Instruction {
public:
  Load(std::string const &instruction, int line);

  std::string Print() const final;

  std::string GetDestinationVariable() const;
  std::string GetSourceVariable() const;
private:
  int GetVariableSize() const;
};