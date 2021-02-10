#pragma once

#include <bitset>

#include "./instructions/alloca.h"
#include "./instructions/functionDefinition.h"
#include "./instructions/store.h"
#include "./instructions/load.h"
#include "./instructions/sext.h"

#include "stackTracker.h"
#include "variableTable.h"

class ProgramGenerator {
public:
  ProgramGenerator(std::vector<Instruction *> const &instructions);
  ~ProgramGenerator();

  std::vector<std::bitset<8>> GetBinary();
  std::vector<std::string> GetAssembly();

private:
  std::vector<Instruction *> const m_instructions;
  VariableTable *m_varTable = nullptr;
  StackTracker *m_stackTracker = nullptr;

  std::string HandleFunctionDefinition(FunctionDefinition *def) const;
  std::vector<std::string> HandleAlloca(Alloca *alloc) const;
  std::vector<std::string> HandleStore(Store *store) const;
  std::vector<std::string> HandleLoad(Load *load) const;
  std::vector<std::string> HandleSext(Sext *sext) const;
};
