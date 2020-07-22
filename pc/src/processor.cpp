#include "processor.h"
#include "instruction.h"
#include "../../common/opcode.h"

#define SIZE_ALU 8
Processor::Processor(Bus& bus) : m_Bus(bus)
{
}

std::bitset<20> ComputePhysicalAddress(const std::bitset<16> &rOne, const std::bitset<16> &rTwo)
{
	//according to the docs, physical_add = | selector * 16 + offset | mod 2^20
	std::bitset<20> physical_add;
	physical_add = (rOne.to_ulong() * 16) + rTwo.to_ulong();

	return physical_add;
}

std::bitset<2> IsInstructionValid(const std::bitset<8> &opcode, bool flag)
{
	//TO DO
	return 0b11;
}

bool IsAccessValid(const std::bitset<20> &address, bool usermode)
{
	//TO DO
	return true;
}

std::bitset<16> Concat(const std::bitset<8> &head, const std::bitset<8> &tail)
{
	std::bitset<16> concatenated = head.to_ulong();
	concatenated = concatenated << 8;
	concatenated = concatenated.to_ulong() + tail.to_ulong();
	return concatenated;
}

std::bitset<8> GetPart(const std::bitset<16> &one, bool high)
{
	std::bitset<8> splitted;

	for (int i = 0; i < 8; i++)
	{
		int oneIndex = high ? i + 8 : i;
		splitted[i] = one[oneIndex];
	}

	return splitted;
}

Star GetFirstExecutionState(const std::bitset<8> &code)
{
	int opcode = (int)code.to_ulong();
	switch (opcode)
	{
	case 0b00010100:
		return Star::nop0;
	case 0b00010101:
		return Star::hlt0;
	case 0b00000001: //mov al,ah
		return Star::ldah0;
	case 0b00000010: //mov ah,al
		return Star::ldal0;
	case 0b00000111: //mov ax,ds
		return Star::ldds0;
	case 0b00001000: //mov ax,ss
		return Star::ldss0;
	case 0b00001001: //mov ax,sp
		return Star::ldsp0;
	case 0b00001010: //mov ax,di
		return Star::lddi0;
	case 0b00000011: //mov ds,ax
		return Star::ldax0;
	case 0b00000100: //mov ss,ax
		return Star::ldax1;
	case 0b00000101: //mov sp,ax
		return Star::ldax2;
	case 0b00000110: //mov di,ax
		return Star::ldax3;
	case 0b01000000: //mov al,ds:(di)
	case 0b10100000: //mov al,ds:offset
		return Star::ld0;
	case 0b10100001: //out al,offset
		return Star::out0;
	case 0b00100001: //cmp ds:(di),al
	case 0b00100010: //add
	case 0b00100011: //sub
	case 0b00100100: //and
	case 0b00100101: //or
	case 0b01100001: //cmp $operand,al
	case 0b01100010: //add
	case 0b01100011: //sub
	case 0b01100100: //and
	case 0b01100101: //or
	case 0b10000001: //cmp
	case 0b10000010: //add
	case 0b10000011: //sub
	case 0b10000100: //and
	case 0b10000101: //or
	case 0b00010001: //not al
	case 0b00001101: //shl al
	case 0b00001110: //sal al
	case 0b00001111: //shr al
	case 0b00010000: //sar al
		return Star::arit_log0;
	case 0b00100000: //mov ds:(di),al
	case 0b01100000: //mov $operand,al
	case 0b10000000: //mov ds:offset,al
	case 0b10000110: //in offset,al
		return Star::ldal1;
	case 0b11000000: //jmp cs:offset
	case 0b11000001: //ja
	case 0b11000010: //jae
	case 0b11000011: //jb
	case 0b11000100: //jbe
	case 0b11000101: //jc
	case 0b11000110: //je
	case 0b11000111: //jg
	case 0b11001000: //jge
	case 0b11001001: //jl
	case 0b11001010: //jle
	case 0b11001011: //jnc
	case 0b11001100: //jne
	case 0b11001101: //jno
	case 0b11001110: //jns
	case 0b11001111: //jnz
	case 0b11010000: //jo
	case 0b11010001: //js
	case 0b11010010: //jz
	case 0b11100000: //jmp selector:offset
		return Star::jmp0;
	case 0b00001011: //push al
		return Star::push0;
	case 0b00001100: //pop al
		return Star::pop0;
	case 0b11010011: //call cs:offset
	case 0b11100001: //call selector:offset
		return Star::call0;
	case 0b00010010: //retn
	case 0b00010011: //retf
		return Star::ret0;
	case 0b01100110: //int operand
		return Star::int0;
	case 0b00010110: //iret
		return Star::iret0;
	case 0b00010111: //cli
		return Star::cli0;
	case 0b00011000: //sli
		return Star::sti0;
	case 0b00011001: //ldpsr
		return Star::ldpsr0;
	case 0b00011010: //stum
		return Star::stum0;
	}
	return Star::nop0;
}

void Processor::OnClock()
{
	switch (m_STAR)
	{
	//////////////////////////////// fetch phase
	case Star::fetch0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetch1;
		break;
	case Star::fetch1:
		m_STAR = Star::fetch2;
		break;
	case Star::fetch2:
		m_MR_ = true;
		m_OPCODE = m_d7_d0;
		m_STAR = IsInstructionValid(m_OPCODE, GetUS()) == 0b11 ? Star::fetch3 : Star::nvi0;
		switch (Instructions::GetFormatType(m_OPCODE))
		{
		case Instructions::Format::F0:
			m_MJR = Star::fetchF0_0;
			break;
		case Instructions::Format::F1:
			m_MJR = Star::fetchF1_0;
			break;
		case Instructions::Format::F2:
			m_MJR = Star::fetchF2_0;
			break;
		case Instructions::Format::F3:
			m_MJR = Star::fetchF3_0;
			break;
		case Instructions::Format::F4:
			m_MJR = Star::fetchF4_0;
			break;
		case Instructions::Format::F5:
			m_MJR = Star::fetchF5_0;
			break;
		case Instructions::Format::F6:
			m_MJR = Star::fetchF6_0;
			break;
		case Instructions::Format::F7:
			m_MJR = Star::fetchF7_0;
			break;
		}
		break;
	case Star::fetch3:
		m_STAR = m_MJR;
		m_MJR = GetFirstExecutionState(m_OPCODE);
		break;
	case Star::fetchF0_0:
		m_STAR = m_MJR;
		break;
	case Star::fetchF1_0:
		m_MAR = ComputePhysicalAddress(m_DS, m_DI);
		m_MR_ = false;
		m_STAR = Star::fetchF1_1;
		break;
	case Star::fetchF1_1:
		m_STAR = Star::fetchF1_2;
		break;
	case Star::fetchF1_2:
		m_SOURCE = m_d7_d0;
		m_MR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::fetchF2_0:
		m_DEST_SEL = m_DS;
		m_DEST_OFF = m_DI;
		m_STAR = m_MJR;
		break;
	case Star::fetchF3_0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetchF3_1;
		break;
	case Star::fetchF3_1:
		m_STAR = Star::fetchF3_2;
		break;
	case Star::fetchF3_2:
		m_SOURCE = m_d7_d0;
		m_MR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::fetchF4_0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetchF4_1;
		break;
	case Star::fetchF4_1:
		m_STAR = Star::fetchF4_2;
		break;
	case Star::fetchF4_2:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF4_3;
		break;
	case Star::fetchF4_3:
		m_STAR = Star::fetchF4_4;
		break;
	case Star::fetchF4_4:
	{
		bool isIN = (Instructions::IN_OPCODE == m_OPCODE.to_ulong());
		m_MR_ = isIN;
		m_MAR = isIN ? ComputePhysicalAddress(0x0000, Concat(m_d7_d0, m_MBR)) : ComputePhysicalAddress(m_DS, Concat(m_d7_d0, m_MBR));
		m_STAR = isIN ? Star::fetchF4_5 : Star::fetchF4_6;
	}
	break;
	case Star::fetchF4_5:
		m_IOR_ = false;
		m_STAR = Star::fetchF4_6;
		break;
	case Star::fetchF4_6:
		m_STAR = Star::fetchF4_7;
		break;
	case Star::fetchF4_7:
		m_SOURCE = m_d7_d0;
		m_MR_ = true;
		m_IOR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::fetchF5_0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetchF5_1;
		break;
	case Star::fetchF5_1:
		m_STAR = Star::fetchF5_2;
		break;
	case Star::fetchF5_2:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF5_3;
		break;
	case Star::fetchF5_3:
		m_STAR = Star::fetchF5_4;
		break;
	case Star::fetchF5_4:
		m_DEST_SEL = Instructions::OUT_OPCODE == m_OPCODE.to_ulong() ? 0x0000 : m_DS;
		m_DEST_OFF = Concat(m_d7_d0, m_MBR);
		m_MR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::fetchF6_0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetchF6_1;
		break;
	case Star::fetchF6_1:
		m_STAR = Star::fetchF6_2;
		break;
	case Star::fetchF6_2:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF6_3;
		break;
	case Star::fetchF6_3:
		m_STAR = Star::fetchF6_4;
		break;
	case Star::fetchF6_4:
		m_DEST_SEL = m_CS;
		m_DEST_OFF = Concat(m_d7_d0, m_MBR);
		m_MR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::fetchF7_0:
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::fetchF7_1;
		break;
	case Star::fetchF7_1:
		m_STAR = Star::fetchF7_2;
		break;
	case Star::fetchF7_2:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF7_3;
		break;
	case Star::fetchF7_3:
		m_STAR = Star::fetchF7_4;
		break;
	case Star::fetchF7_4:
		m_DEST_OFF = Concat(m_d7_d0, m_MBR);
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF7_5;
		break;
	case Star::fetchF7_5:
		m_STAR = Star::fetchF7_6;
		break;
	case Star::fetchF7_6:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_CS, m_IP);
		m_IP = (m_IP.to_ulong() + 1);
		m_STAR = Star::fetchF7_7;
		break;
	case Star::fetchF7_7:
		m_STAR = Star::fetchF7_8;
		break;
	case Star::fetchF7_8:
		m_DEST_OFF = Concat(m_d7_d0, m_MBR);
		m_MR_ = true;
		m_STAR = m_MJR;
		break;
	case Star::nvi0:
		m_SOURCE = IsInstructionValid(m_OPCODE, GetUS()) == 0b00 ? 0x06 : 0x05;
		m_STAR = Star::nvi0;
		break;
	//////////////////////////////// end fetch phase
	//////////////////////////////// execution phase
	case Star::nop0:
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::hlt0:
		m_STAR = Star::hlt0;
		break;
	case Star::ldah0:
		m_AH = m_AL;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldal0:
		m_AL = m_AH;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldds0:
		m_DS = Concat(m_AH, m_AL);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldss0:
		m_SS = Concat(m_AH, m_AL);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldsp0:
		m_SP = Concat(m_AH, m_AL);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::lddi0:
		m_DI = Concat(m_AH, m_AL);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldax0:
		m_AH = GetPart(m_DS, true);
		m_AL = GetPart(m_DS, false);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldax1:
		m_AH = GetPart(m_SS, true);
		m_AL = GetPart(m_SS, false);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldax2:
		m_AH = GetPart(m_SP, true);
		m_AL = GetPart(m_SP, false);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldax3:
		m_AH = GetPart(m_DI, true);
		m_AL = GetPart(m_DI, false);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ld0:
		m_MAR = ComputePhysicalAddress(m_DEST_SEL, m_DEST_OFF);
		m_MBR = m_AL;
		m_DIR = true;
		m_STAR = Star::ld1;
		break;
	case Star::ld1:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::ld2 : Star::nvma0;
		break;
	case Star::ld2:
		m_MW_ = true;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::out0:
		m_MAR = ComputePhysicalAddress(m_DEST_SEL, m_DEST_OFF);
		m_MBR = m_AL;
		m_DIR = true;
		m_STAR = Star::out1;
		break;
	case Star::out1:
		m_IOW_ = false;
		m_STAR = Star::out2;
		break;
	case Star::out2:
		m_IOW_ = true;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::arit_log0:
		ExecuteALU();
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldal1:
		m_AL = m_SOURCE;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::jmp0:
		m_CS = m_DEST_SEL;
		m_IP = IsConditionMatch() ? m_DEST_OFF : m_IP;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::push0:
		m_MAR = ComputePhysicalAddress(m_SS, (m_SP.to_ulong() - 1));
		m_MBR = m_AL;
		m_DIR = true;
		m_STAR = Star::push1;
		break;
	case Star::push1:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::push2 : Star::nvma0;
		break;
	case Star::push2:
		m_MW_ = true;
		m_SP = (m_SP.to_ulong() - 1);
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::pop0:
		m_MAR = ComputePhysicalAddress(m_SS, m_SP);
		m_SP = (m_SP.to_ulong() + 1);
		m_MR_ = false;
		m_STAR = Star::pop1;
		break;
	case Star::pop1:
		m_STAR = Star::pop2;
		break;
	case Star::pop2:
		m_AL = m_d7_d0;
		m_MR_ = true;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::call0:
		m_MAR = ComputePhysicalAddress(m_SS, (m_SP.to_ulong() - 1));
		m_MBR = GetPart(m_IP, true);
		m_DIR = true;
		m_STAR = Star::call1;
		break;
	case Star::call1:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::call2 : Star::nvma0;
		break;
	case Star::call2:
		m_MW_ = true;
		m_SP = m_SP.to_ulong() - 1;
		m_STAR = Star::call3;
		break;
	case Star::call3:
		m_MAR = ComputePhysicalAddress(m_SS, (m_SP.to_ulong() - 1));
		m_MBR = GetPart(m_IP, false);
		m_STAR = Star::call4;
		break;
	case Star::call4:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::call5 : Star::nvma0;
		break;
	case Star::call5:
		m_MW_ = true;
		m_SP = m_SP.to_ulong() - 1;
		m_IP = m_DEST_OFF;
		m_STAR = m_OPCODE == Instructions::CALLF_OPCODE ? Star::call6 : Star::call12;
		break;
	case Star::call6:
		m_MAR = ComputePhysicalAddress(m_SS, (m_SP.to_ulong() - 1));
		m_MBR = GetPart(m_CS, true);
		m_STAR = Star::call7;
		break;
	case Star::call7:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::call8 : Star::nvma0;
		break;
	case Star::call8:
		m_MW_ = true;
		m_SP = m_SP.to_ulong() - 1;
		m_STAR = Star::call9;
		break;
	case Star::call9:
		m_MAR = ComputePhysicalAddress(m_SS, (m_SP.to_ulong() - 1));
		m_MBR = GetPart(m_CS, false);
		m_STAR = Star::call10;
		break;
	case Star::call10:
		m_MW_ = IsAccessValid(m_MAR, GetUS()) ? false : true;
		m_STAR = IsAccessValid(m_MAR, GetUS()) ? Star::call11 : Star::nvma0;
		break;
	case Star::call11:
		m_MW_ = true;
		m_SP = m_SP.to_ulong() - 1;
		m_CS = m_DEST_SEL;
		m_STAR = Star::call12;
		break;
	case Star::call12:
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ret0:
		m_MAR = ComputePhysicalAddress(m_SS, m_SP);
		m_SP = m_SP.to_ulong() + 1;
		m_MR_ = false;
		m_STAR = Star::ret1;
		break;
	case Star::ret1:
		m_STAR = Star::ret2;
		break;
	case Star::ret2:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_SS, m_SP);
		m_SP = m_SP.to_ulong() + 1;
		m_STAR = Star::ret3;
		break;
	case Star::ret3:
		m_STAR = m_OPCODE == Instructions::RETF_OPCODE ? Star::ret4 : Star::ret8;
		break;
	case Star::ret4:
		m_CS = Concat(m_d7_d0, m_MBR);
		m_MAR = ComputePhysicalAddress(m_SS, m_SP);
		m_SP = m_SP.to_ulong() + 1;
		m_STAR = Star::ret5;
		break;
	case Star::ret5:
		m_STAR = Star::ret6;
		break;
	case Star::ret6:
		m_MBR = m_d7_d0;
		m_MAR = ComputePhysicalAddress(m_SS, m_SP);
		m_SP = m_SP.to_ulong() + 1;
		m_STAR = Star::ret7;
		break;
	case Star::ret7:
		m_STAR = Star::ret8;
		break;
	case Star::ret8:
		m_IP = Concat(m_d7_d0, m_MBR);
		m_MR_ = true;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
		//TO DO int0
		//TO DO iret0
	case Star::cli0:
		SetIF(false);
		m_STAR = Star::fetch0;
		break;
	case Star::sti0:
		SetIF(true);
		m_STAR = m_intr ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::ldpsr0:
		m_PREV_SS = m_SS;
		m_PREV_SP = m_SP;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
		break;
	case Star::stum0:
	{
		SetUS(true);
		auto tmp = m_SS;
		m_SS = m_PREV_SS;
		m_PREV_SS = tmp;
		tmp = m_SP;
		m_SP = m_PREV_SP;
		m_PREV_SP = tmp;
		m_STAR = GetIF() ? Star::pre_tipo0 : Star::fetch0;
	}
	break;
	case Star::nvma0:
		m_SOURCE = 0x04;
		m_STAR = Star::int0;
		break;
	case Star::pre_tipo0:
		m_DIR = 0;
		m_INTA = 1;
		m_STAR = m_intr ? Star::pre_tipo0 : Star::pre_tipo1;
		break;
	case Star::pre_tipo1:
		m_SOURCE = m_d7_d0;
		m_INTA = 0;
		m_STAR = Star::int0;
		break;
	//////////////////////////////// end execution phase
	default:
		//TO DO error log
		m_STAR = Star::hlt0;
		break;
	}

	if (!m_MR_ || !m_IOR_)
	{
		m_Log.push_back("Reading: ");
		m_Log.push_back(m_MAR.to_string());
		m_Log.push_back(" ");
		m_Log.push_back(m_Bus.Read(m_MAR.to_ulong()).to_string());
		m_Log.push_back("\n\n");
		m_d7_d0 = m_Bus.Read(m_MAR.to_ulong());
	}

	if (!m_MW_ || !m_IOW_)
	{
		m_Bus.Write(m_MAR.to_ulong(), m_MBR.to_ulong());
	}

	//TO DO
	// if (!m_IOR_)
	// 	m_d7_d0 = m_Bus->IORead(m_MAR.to_ulong());
	// if (!m_IOW_)
	// 	m_Bus->IOWrite(m_MAR.to_ulong(), m_MBR.to_ulong());
}

void Processor::OnReset()
{
	m_DIR = false;
	m_MR_ = true;
	m_MW_ = true;
	m_IOR_ = true;
	m_IOW_ = true;
	m_INTA = false;
	m_F = 0b000000;
	m_CS = 0xF000;
	m_IP = 0x0000;
	m_STAR = Star::fetch0;
}

Processor::Status Processor::GetStatus() const
{
	Status status;
	status.star = (int)m_STAR;
	status.mjr = (int)m_MJR;
	status.d7d0 = m_d7_d0.to_ullong();
	status.cs = m_CS.to_ulong();
	status.ip = m_IP.to_ulong();
	status.opcode = m_OPCODE.to_ulong();
	status.source = m_SOURCE.to_ulong();
	status.ss = m_SS.to_ulong();
	status.sp = m_SP.to_ulong();
	status.ds = m_DS.to_ulong();
	status.di = m_DI.to_ulong();
	status.destSel = m_DEST_SEL.to_ulong();
	status.destOff = m_DEST_OFF.to_ulong();
	status.al = m_AL.to_ulong();
	status.cf = GetCF();
	status.of = GetOF();
	status.sf = GetSF();
	status.zf = GetZF();
	status.mar = m_MAR.to_string();
	status.mbr = m_MBR.to_ulong();
	status.mr_ = m_MR_;
	status.mw_ = m_MW_;
	status.log = m_Log;
	return status;
}

void Processor::SetCF(bool val)
{
	m_F[0] = val;
}

void Processor::SetZF(bool val)
{
	m_F[1] = val;
}

void Processor::SetSF(bool val)
{
	m_F[2] = val;
}

void Processor::SetOF(bool val)
{
	m_F[3] = val;
}

void Processor::SetIF(bool val)
{
	m_F[4] = val;
}

void Processor::SetUS(bool val)
{
	m_F[5] = val;
}

bool Processor::GetCF() const
{
	return m_F[0];
}

bool Processor::GetZF() const
{
	return m_F[1];
}

bool Processor::GetSF() const
{
	return m_F[2];
}

bool Processor::GetOF() const
{
	return m_F[3];
}

bool Processor::GetIF() const
{
	return m_F[4];
}

bool Processor::GetUS() const
{
	return m_F[5];
}

bool Processor::IsConditionMatch()
{
	//TO DO make proper function to convert bitsets
	switch ((Opcode)m_OPCODE.to_ulong())
	{
	case Opcode::jmp_cs_offset:
	case Opcode::jmp_selector$offset:
		return true;
	case Opcode::je_cs_offset:
		return GetZF();
	case Opcode::jne_cs_offset:
		return !GetZF();
	case Opcode::ja_cs_offset:
		return !GetCF() && !GetZF();
	case Opcode::jae_cs_offset:
		return !GetCF();
	case Opcode::jb_cs_offset:
		return GetCF();
	case Opcode::jbe_cs_offset:
		return GetCF() && GetZF();
	case Opcode::jg_cs_offset:
		return !GetZF() && (GetSF() == GetOF());
	case Opcode::jge_cs_offset:
		return GetSF() == GetOF();
	case Opcode::jl_cs_offset:
		return GetSF() != GetOF();
	case Opcode::jle_cs_offset:
		return GetZF() == 1 || (GetSF() != GetOF());
	case Opcode::jz_cs_offset:
		return GetZF();
	case Opcode::jnz_cs_offset:
		return !GetZF();
	case Opcode::jc_cs_offset:
		return GetCF();
	case Opcode::jnc_cs_offset:
		return !GetCF();
	case Opcode::jo_cs_offset:
		return GetOF();
	case Opcode::jno_cs_offset:
		return !GetOF();
	case Opcode::js_cs_offset:
		return GetSF();
	case Opcode::jns_cs_offset:
		return !GetSF();
	}

	//TO DO add some kind of errorlog
	return false;
}

void Processor::ExecuteALU()
{
	//  CF  is set if there is an overflow when the operands are
	//      considered natural number
	//  OF  is set if there is an overflow when the operands are
	//      considered integers
	//  ZF  is set when the result of the last operation is 0
	//
	//  SF  is set when the most significant bit is set to 1
	//      for integers means the result is negative
	bool isSubtraction = Instructions::IsSUB(m_OPCODE);
	if (Instructions::IsADD(m_OPCODE) || isSubtraction)
	{
		auto localSource = m_SOURCE;

		if (isSubtraction)
		{
			//TO DO better
			localSource = ~localSource;
			localSource = localSource.to_ulong() + 1;
		}

		bool sameSign = m_AL[SIZE_ALU - 1] & m_SOURCE[SIZE_ALU - 1];

		bool carry = false;
		for (int i = 0; i < SIZE_ALU; i++)
		{
			auto res = (m_AL[i] ^ localSource[i]) ^ carry;
			carry = (m_AL[i] & localSource[i]) | (carry & localSource[i]) | (m_AL[i] & carry);
			m_AL[i] = res;
		}

		SetCF(carry);
		SetOF(carry || (sameSign && (localSource[SIZE_ALU - 1] != m_AL[SIZE_ALU - 1])));
	}
	else if (Instructions::IsAND(m_OPCODE))
	{
		m_AL = m_AL & m_SOURCE;
	}
	else if (Instructions::IsOR(m_OPCODE))
	{
		m_AL = m_AL | m_SOURCE;
	}
	else if (Instructions::IsNOT(m_OPCODE))
	{
		m_AL = ~m_AL;
	}
	else if (Instructions::IsSHL(m_OPCODE) || Instructions::IsSAL(m_OPCODE))
	{
		SetCF(m_AL[SIZE_ALU - 1]);
		bool of = Instructions::IsSHL(m_OPCODE) ? false : m_AL[SIZE_ALU - 1] != m_AL[SIZE_ALU - 2] ? true : false;
		SetOF(of);

		m_AL = m_AL << 1;
	}
	else if (Instructions::IsSHR(m_OPCODE) || Instructions::IsSAR(m_OPCODE))
	{

		SetOF(false);
		SetCF(m_AL[0]);
		m_AL = m_AL >> 1;

		if (Instructions::IsSAR(m_OPCODE))
		{
			m_AL[SIZE_ALU - 1] = m_AL[SIZE_ALU - 2];
		}
	}

	SetSF(m_AL[SIZE_ALU - 1]); //TO DO should this be set in case of CMP?
	SetZF(m_AL.to_ulong() == 0);

	if (Instructions::IsCMP(m_OPCODE))
	{
		SetZF(m_AL == m_SOURCE);
		SetCF(m_AL.to_ulong() > m_SOURCE.to_ulong());
		if (m_AL[SIZE_ALU - 1] > m_SOURCE[SIZE_ALU - 1])
		{
			SetOF(false);
		}
		else if (m_AL[SIZE_ALU - 1] < m_SOURCE[SIZE_ALU - 1])
		{
			SetOF(true);
		}
		else if (m_AL[SIZE_ALU - 1] == 0)
		{
			SetOF(GetCF() && !GetZF());
		}
		else
		{
			SetOF(!GetCF() && !GetZF());
		}
	}
}
