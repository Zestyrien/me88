#pragma once
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>
#include "tree.h"
#include "symboltable.h"

namespace CodeGen
{
	class Code
	{
	private:
		std::vector<std::bitset<8>> m_code;
		std::shared_ptr<std::bitset<16>> m_sp;
		std::shared_ptr<std::unordered_map<std::string, std::bitset<16>>> m_varToAddr;
		void SetALEval();

	public:
		Code() = delete;
		Code(std::shared_ptr<std::unordered_map<std::string, std::bitset<16>>>& vatToAddr,
				 std::shared_ptr<std::bitset<16>>& sp);
		std::size_t Size() const;
		std::vector<std::bitset<8>> GetCode() const;
		void Concat(const Code &othercode);
		void Print() const;
		//code functions
		void Push();
		void Pop();
		void Hlt();
		void AddToStack(const std::string &variable);
		void RemoveFromStack(const std::string &variable);
		void ChangeSign();
		void Assignement(const std::string &variable);
		void LoadVariableDSDI(const std::string &variable);
		void LoadAX(std::bitset<16> imm);
		void LoadAL(std::bitset<8> imm);
		void LoadAL(const std::string &variable);
		void LoadOperand(std::bitset<8> imm);
		void LoadOffset(std::bitset<16> offset);
		void AddDSDI(const std::string &variable);
		void SubDSDI(const std::string &variable);
		void CmpDSDI(const std::string &variable);
		void EvalAboveDSDI(const std::string &variable);
		void Add(int op);
		void Sub(int op);
		void Cmp(int op);
		void JmpNE(int offset);
		void Jmp(int offset);
		void EvalAbove(int offset);
		void CondBodies(const Code &condTrue, const Code &condFalse);
	};

	std::vector<std::bitset<8>> GenerateCode(const Tree& program, const SymbolTable& symbols);
	void Print(const std::vector<std::bitset<8>>& code);

}; // namespace CodeGen