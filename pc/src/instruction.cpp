#include "instruction.h"
#include "../../common/opcode.h"

Instructions::Format Instructions::GetFormatType(const std::bitset<8>& opcode)
{
	return (Format) (opcode[7]*4 + opcode[6]*2 + opcode[5]);
}

bool Instructions::IsADD(const std::bitset<8>& code)
{
	return code == (int)Opcode::add_ds$di_al || code == (int)Opcode::add_operand_al || code == (int)Opcode::add_ds$offset_al;
}

bool Instructions::IsSUB(const std::bitset<8>& code)
{
	return code == (int)Opcode::sub_ds$di_al || code == (int)Opcode::sub_operand_al || code == (int)Opcode::sub_ds$offset_al;
}

bool Instructions::IsAND(const std::bitset<8>& code)
{
	return code == (int)Opcode::and_ds$di_al || code == (int)Opcode::and_operand_al || code == (int)Opcode::and_ds$offset_al;
}

bool Instructions::IsOR(const std::bitset<8>& code)
{
	return code == (int)Opcode::or_ds$di_al || code == (int)Opcode::or_operand_al || code == (int)Opcode::or_ds$offset_al;
}

bool Instructions::IsCMP(const std::bitset<8>& code)
{
	return code == (int)Opcode::cmp_ds$di_al || code == (int)Opcode::cmp_operand_al || code == (int)Opcode::cmp_ds$offset_al;
}

bool Instructions::IsNOT(const std::bitset<8>& code)
{
	return code == (int)Opcode::not_al;
}

bool Instructions::IsSHL(const std::bitset<8>& code)
{
	return code == (int)Opcode::shl_code;
}

bool Instructions::IsSAL(const std::bitset<8>& code)
{
	return code == (int)Opcode::sal_code;
}

bool Instructions::IsSHR(const std::bitset<8>& code)
{
	return code == (int)Opcode::shr_code;
}

bool Instructions::IsSAR(const std::bitset<8>& code)
{
	return code == (int)Opcode::sar_code;
}
