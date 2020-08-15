#pragma once
#include "symbolstable.h"
#include "tree.h"
#include <bitset>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class CodeGenerator {
public:
  void Push();
  void Pop();
  void LoadAL(int value);
  void LoadAL(const std::string &variable);
  void AddToStack(const std::string &variable);
  void RemoveFromStack(const std::string &variable);
  void AddAssignement(const std::string &variable);
  void AddDSDI(const std::string &variable);
  void SubDSDI(const std::string &variable);
  void CmpDSDI(const std::string &variable);
  void EvalBelowDSDI(const std::string &variable);
  void EvalBelow(int op);
  void JmpNE(int offset);
  void JmpNE();
  void Jmp(int offset);
  void Jmp();
  int CreateJumpPlaceholder();
  void ReplaceJumpPlaceholder(int index);
  void Add(int op);
  void Sub(int op);
  void Cmp(int op);
  void Hlt();
  std::vector<std::bitset<8>> GetCode();
  void Print();
  std::size_t Size();

private:
  void LoadVariableDSDI(const std::string &variable);
  void LoadOperand(std::bitset<8> op);
  void LoadAX(int value);
  void ParseALEval();
  void LoadOffset(int off);
  std::vector<std::bitset<8>> m_code;
  std::unordered_map<std::string, std::bitset<16>> m_varToAddr;
  std::bitset<16> m_sp;
};

std::vector<std::bitset<8>> GenerateCode(const Tree &program,
                                         const SymbolsTable &symbols,
                                         bool debug = false);
