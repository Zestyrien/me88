#pragma once
#include "symbolstable.h"
#include "tree.h"
#include <bitset>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class CodeGenerator
{
public:
  void Push();
  void PushArgument();
  void Pop();
  void LoadAL(int value);
  void LoadAL(const std::string &variable);
  void AddToStack(const std::string &variable, SymbolType type);
  void SetOutOfScope(const std::string &variable);
  void RemoveFromStack(const std::string &variable);
  void AddAssignement(const std::string &variable);
  void AddDSDI(const std::string &variable);
  void SubDSDI(const std::string &variable);
  void CmpDSDI(const std::string &variable);
  void EvalBelowDSDI(const std::string &variable);
  void EvalBelowEqDSDI(const std::string &variable);
  void EvalAboveDSDI(const std::string &variable);
  void EvalEqualDSDI(const std::string &variable);
  void EvalNotEqualDSDI(const std::string &variable);
  void EvalBelow(int op);
  void EvalAbove(int op);
  void EvalAboveEq(int op);
  void EvalEqual(int op);
  void EvalNotEqual(int op);
  void JmpNE(int offset);
  void JmpNE();
  void Jmp(int offset);
  void Jmp();
  void CreateCallNear(const std::string &function);
  void ReturnNear();
  void CallNear(const std::string &function);
  int CreateJumpPlaceholder();
  void ReplaceJumpPlaceholder(int index);
  void NewFunFrame();
  void RemoveFrame();
  void Add(int op);
  void Sub(int op);
  void Cmp(int op);
  void Hlt();
  void SetPushingFunctionArgs(bool pushing);
  std::vector<std::bitset<8>> GetCode();
  void Print();
  std::size_t Size();

  std::unordered_map<std::string, std::shared_ptr<Tree>> m_funBodies;

private:
  struct StackVariable
  {
    std::vector<int> position;
    SymbolType type;
  };

  struct StackFrame
  {
    std::unordered_map<std::string, StackVariable> varToAddr;
    int varCount = 0;
    int argCount = 0;
  };

  void LoadVariableDSDI(const std::string &variable);
  void LoadOperand(std::bitset<8> op);
  void LoadAX(int value);
  void ParseALEval();
  void LoadOffset(int off);
  void AddFunctionBody(const std::string &funName);
  std::vector<std::bitset<8>> m_code;
  std::vector<StackFrame> m_frames;
  std::unordered_map<std::string, std::bitset<16>> m_funBodyAddr;
  bool m_pushingArgs = false;
  int m_alreadyPushed = 0;
};
