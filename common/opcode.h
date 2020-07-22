enum class Opcode
{
	//F0
	mov_al_ah = 0b00000001,
	mov_ah_al = 0b00000010,
	mov_ds_ax = 0b00000011,
	mov_ss_ax = 0b00000100,
	mov_sp_ax = 0b00000101,
	mov_di_ax = 0b00000110,
	mov_ax_ds = 0b00000111,
	mov_ax_ss = 0b00001000,
	mov_ax_sp = 0b00001001,
	mov_ax_di = 0b00001010,
	push_al = 0b00001011,
	pop_al = 0b00001100,
	shl_code = 0b00001101,
	sal_code = 0b00001110,
	shr_code = 0b00001111,
	sar_code = 0b00010000,
	not_al = 0b00010001,
	retn_code = 0b00010010,
	retf_code = 0b00010011,
	nop_code = 0b00010100,
	htl_code = 0b00010101,
	iret_code = 0b00010110,
	cli_code = 0b00010111,
	sti_code = 0b00011000,
	ldpsr_code = 0b00011001,
	stum_code = 0b00011010,

	//F1
	mov_ds$di_al = 0b00100000,
	cmp_ds$di_al = 0b00100001,
	add_ds$di_al = 0b00100010,
	sub_ds$di_al = 0b00100011,
	and_ds$di_al = 0b00100100,
	or_ds$di_al = 0b00100101,

	//F2
	mov_al_ds$di = 0b01000000,

	//F3
	mov_operand_al = 0b01100000,
	cmp_operand_al = 0b01100001,
	add_operand_al = 0b01100010,
	sub_operand_al = 0b01100011,
	and_operand_al = 0b01100100,
	or_operand_al = 0b01100101,

	//F4
	mov_ds$offset_al = 0b10000000,
	cmp_ds$offset_al = 0b10000001,
	add_ds$offset_al = 0b10000010,
	sub_ds$offset_al = 0b10000011,
	and_ds$offset_al = 0b10000100,
	or_ds$offset_al = 0b10000101,
	in_offset_al = 0b10000110,

	//F5
	mov_al_ds$offset = 0b10100000,
	out_al_offset = 0b10100001,

	//F6
	jmp_cs_offset = 0b11000000,
	ja_cs_offset = 0b11000001,
	jae_cs_offset = 0b11000010,
	jb_cs_offset = 0b11000011,
	jbe_cs_offset = 0b11000100,
	jc_cs_offset = 0b11000101,
	je_cs_offset = 0b11000110,
	jg_cs_offset = 0b11000111,
	jge_cs_offset = 0b11001000,
	jl_cs_offset = 0b11001001,
	jle_cs_offset = 0b11001010,
	jnc_cs_offset = 0b11001011,
	jne_cs_offset = 0b11001100,
	jno_cs_offset = 0b11001101,
	jns_cs_offset = 0b11001110,
	jnz_cs_offset = 0b11001111,
	jo_cs_offset = 0b11010000,
	js_cs_offset = 0b11010001,
	jz_cs_offset = 0b11010010,
	call_cs_offset = 0b11010011,

	//F7
	jmp_selector$offset = 0b11100000,
	call_selector$offsett = 0b11100001
};