#pragma once

#include <bitset>
namespace Instructions
{
	const int IN_OPCODE = 0b10000110;
	const int OUT_OPCODE = 0b10100001;
	const int CALLF_OPCODE = 0b11100001;
	const int RETF_OPCODE = 0b00010100;

	//alu opcodes
	bool IsADD(const std::bitset<8> &code);
	bool IsSUB(const std::bitset<8> &code);
	bool IsAND(const std::bitset<8> &code);
	bool IsOR(const std::bitset<8> &code);
	bool IsCMP(const std::bitset<8> &code);
	bool IsNOT(const std::bitset<8> &code);
	bool IsSHL(const std::bitset<8> &code);
	bool IsSAL(const std::bitset<8> &code);
	bool IsSHR(const std::bitset<8> &code);
	bool IsSAR(const std::bitset<8> &code);

	enum class Format
	{
		F0 = 0b000,
		F1 = 0b001,
		F2 = 0b010,
		F3 = 0b011,
		F4 = 0b100,
		F5 = 0b101,
		F6 = 0b110,
		F7 = 0b111
	};

	Format GetFormatType(const std::bitset<8> &opcode);
} // namespace Instructions
