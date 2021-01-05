#pragma once

#include "instruction.h"

class FunctionDefinition : public Instruction {
public:
  FunctionDefinition(std::string const &instruction, int line);

  std::string Print() const final;
  std::string GetFunctionName() const;
};

class FunctionDefinitionEnd : public Instruction {
public:
  FunctionDefinitionEnd(std::string const &instruction, int line);

  std::string Print() const final;
};