
#pragma once
#include "bus.h"
#include <unordered_map>
#include <vector>
#include <string>

enum class Star
{
	//fetch phase
	fetch0 = 0,
	fetch1,
	fetch2,
	fetch3,
	fetchF0_0,
	fetchF1_0,
	fetchF1_1,
	fetchF1_2,
	fetchF2_0,
	fetchF3_0,
	fetchF3_1,
	fetchF3_2,
	fetchF4_0,
	fetchF4_1,
	fetchF4_2,
	fetchF4_3,
	fetchF4_4,
	fetchF4_5,
	fetchF4_6,
	fetchF4_7,
	fetchF5_0,
	fetchF5_1,
	fetchF5_2,
	fetchF5_3,
	fetchF5_4,
	fetchF6_0,
	fetchF6_1,
	fetchF6_2,
	fetchF6_3,
	fetchF6_4,
	fetchF7_0,
	fetchF7_1,
	fetchF7_2,
	fetchF7_3,
	fetchF7_4,
	fetchF7_5,
	fetchF7_6,
	fetchF7_7,
	fetchF7_8,
	nvi0, //something went wrong

	//execution phase
	nop0,	 //NOP
	hlt0,	 //HLT
	ldah0, //MOV AL,AH
	ldal0, //MOV AH,AL
	ldds0, //MOV AX,DS
	ldss0, //MOV AX,SS
	ldsp0, //MOV AX,SP
	lddi0, //MOV AX,DI
	ldax0, //MOV DS,AX
	ldax1, //MOV SS,AX
	ldax2, //MOV SP,AX
	ldax3, //MOV DI,AX

	//MOV AL,DS:(DI)
	ld0,
	ld1,
	ld2,

	//OUT AL,offset
	out0,
	out1,
	out2,

	//ADD, SUB, AND, OR, CMP, SHR, SAL...
	arit_log0,

	//MOV, IN
	ldal1,

	//JMP, JA, JAE...
	jmp0,

	//PUSH AL
	push0,
	push1,
	push2,

	//POP AL
	pop0,
	pop1,
	pop2,

	//CALL selector:offset
	call0,
	call1,
	call2,
	call3,
	call4,
	call5,
	call6,
	call7,
	call8,
	call9,
	call10,
	call11,
	call12,

	//RETN, RETF
	ret0,
	ret1,
	ret2,
	ret3,
	ret4,
	ret5,
	ret6,
	ret7,
	ret8,

	//INT
	int0,
	int1,
	int2,
	int3,
	int4,
	int5,
	int6,
	int7,
	int8,
	int9,
	int10,
	int11,
	int12,
	int13,
	int14,
	int15,
	int16,
	int17,
	int18,
	int19,
	int20,
	int21,
	int22,
	int23,
	int24,

	//IRET
	iret0,
	iret1,
	iret2,
	iret3,
	iret4,
	iret5,
	iret6,
	iret7,
	iret8,
	iret9,
	iret10,
	iret11,

	//CLI
	cli0,

	//STI
	sti0,

	//LDPSR
	ldpsr0,

	//STUM
	stum0,

	//new statement
	nvma0,

	//interruption fetch phase
	pre_tipo0,
	pre_tipo1
};

class Processor
{
public:
	struct Status
	{
		int star;
		int mjr;
		int d7d0;
		int cs;
		int ip;
		int opcode;
		int source;
		int ss;
		int sp;
		int ds;
		int di;
		int destSel;
		int destOff;
		int al;
		bool cf;
		bool of;
		bool sf;
		bool zf;

		std::string mar;
		int mbr;
		int mr_;
		int mw_;
		std::vector<std::string> log;
	};

	Processor() = delete;
	Processor(Bus& bus);
	void OnClock();
	void OnReset();
	Status GetStatus() const;

private:
	Bus& m_Bus;
	std::vector<std::string> m_Log;

	std::bitset<8> m_d7_d0;

	//TO DO this is set from an external interface
	bool m_intr; 

	//Registers
	std::bitset<20> m_MAR;
	bool m_MR_;	 //memory read
	bool m_MW_;	 //memory write
	bool m_IOR_; //i/o read
	bool m_IOW_; //i/o write
	bool m_INTA; //interrupt

	bool m_DIR;
	std::bitset<6> m_F; //flags
	std::bitset<8> m_AL, m_AH, m_MBR, m_OPCODE, m_SOURCE;
	std::bitset<16> m_DI, m_DS, m_CS, m_IP, m_SS, m_SP, m_DEST_OFF, m_DEST_SEL, m_PREV_SS, m_PREV_SP;

	Star m_STAR, m_MJR; //status register

	// flags
	void SetCF(bool val); //carry
	void SetZF(bool val); //zero
	void SetSF(bool val); //sign
	void SetOF(bool val); //overflow
	void SetIF(bool val); //interrupt
	void SetUS(bool val); //user/sistem 1=user

	bool GetCF() const;
	bool GetZF() const;
	bool GetSF() const;
	bool GetOF() const;
	bool GetIF() const;
	bool GetUS() const;

	bool IsConditionMatch();
	void ExecuteALU();
};
