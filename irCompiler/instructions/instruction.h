#pragma once

#include <string>
#include <vector>

enum class InstructionType {
  Unknown = 0,
  Alloca,
  Store,
  Load,
  Sext,
  FunctionDefinition,
  FunctionDefinitionEnd
};

class Instruction {
public:
  Instruction(std::string const &instruction, int line, InstructionType type);
  virtual ~Instruction() = default;

  virtual int GetLine() const;
  virtual std::string GetInstruction() const;
  virtual InstructionType GetType() const;
  virtual std::string PrintType() const;

  static InstructionType GetType(std::string const &instruction);
  virtual std::string Print() const = 0;

  std::string m_instruction;
  int m_line;
  InstructionType m_type;
};
