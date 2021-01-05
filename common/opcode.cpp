#include "opcode.h"
#include <tuple>
#include <unordered_map>

std::string OpcodeToString(Opcode const opcode) {
  switch (opcode) {
    // F0
  case Opcode::mov_al_ah:
    return "mov al, ah";
  case Opcode::mov_ah_al:
    return "mov ah, al";
  case Opcode::mov_ds_ax:
    return "mov ds, ax";
  case Opcode::mov_ss_ax:
    return "mov ss, ax";
  case Opcode::mov_sp_ax:
    return "mov sp, ax";
  case Opcode::mov_di_ax:
    return "mov di, ax";
  case Opcode::mov_ax_ds:
    return "mov ax, ds";
  case Opcode::mov_ax_ss:
    return "mov ax, ss";
  case Opcode::mov_ax_sp:
    return "mov ax, sp";
  case Opcode::mov_ax_di:
    return "mov ax, di";
  case Opcode::push_al:
    return "push al";
  case Opcode::pop_al:
    return "pop al";
  case Opcode::shl_al:
    return "shl al";
  case Opcode::sal_al:
    return "sal al";
  case Opcode::shr_al:
    return "shr al";
  case Opcode::sar_al:
    return "sar al";
  case Opcode::not_al:
    return "not al";
  case Opcode::retn:
    return "retn";
  case Opcode::retf:
    return "retf";
  case Opcode::nop:
    return "nop";
  case Opcode::htl:
    return "htl";
  case Opcode::iret:
    return "iret";
  case Opcode::cli:
    return "cli";
  case Opcode::sti:
    return "sti";
  case Opcode::ldpsr:
    return "ldpsr";
  case Opcode::stum:
    return "stum";
    // F1
  case Opcode::mov_ds$di_al:
    return "mov ds:(di), al";
  case Opcode::cmp_ds$di_al:
    return "cmp ds:(di), al";
  case Opcode::add_ds$di_al:
    return "add ds:(di), al";
  case Opcode::sub_ds$di_al:
    return "sub ds:(di), al";
  case Opcode::and_ds$di_al:
    return "and ds:(di), al";
  case Opcode::or_ds$di_al:
    return "or ds:(di), al";
    // F2
  case Opcode::mov_al_ds$di:
    return "mov al, ds:(di)";
    // F3
  case Opcode::mov_operand_al:
    return "mov $operand, al";
  case Opcode::cmp_operand_al:
    return "cmp $operand, al";
  case Opcode::add_operand_al:
    return "add $operand, al";
  case Opcode::sub_operand_al:
    return "sub $operand, al";
  case Opcode::and_operand_al:
    return "and $operand, al";
  case Opcode::or_operand_al:
    return "or $operand, al";
  case Opcode::int_operand:
    return "int $operand, al";
    // F4
  case Opcode::mov_ds$offset_al:
    return "mov ds:offset, al";
  case Opcode::cmp_ds$offset_al:
    return "cmp ds:offset, al";
  case Opcode::add_ds$offset_al:
    return "add ds:offset, al";
  case Opcode::sub_ds$offset_al:
    return "sub ds:offset, al";
  case Opcode::and_ds$offset_al:
    return "and ds:offset, al";
  case Opcode::or_ds$offset_al:
    return "or ds:offset, al";
  case Opcode::in_offset_al:
    return "in offset, al";
    // F5
  case Opcode::mov_al_ds$offset:
    return "mov al, offset";
  case Opcode::out_al_offset:
    return "out al, offset";
    // F6
  case Opcode::jmp_cs_offset:
    return "jmp cs, offset";
  case Opcode::ja_cs_offset:
    return "ja cs, offset";
  case Opcode::jae_cs_offset:
    return "jae cs, offset";
  case Opcode::jb_cs_offset:
    return "jb cs, offset";
  case Opcode::jbe_cs_offset:
    return "jbe cs, offset";
  case Opcode::jc_cs_offset:
    return "jc cs, offset";
  case Opcode::je_cs_offset:
    return "je cs, offset";
  case Opcode::jg_cs_offset:
    return "jg cs, offset";
  case Opcode::jge_cs_offset:
    return "jge cs, offset";
  case Opcode::jl_cs_offset:
    return "j; cs, offset";
  case Opcode::jle_cs_offset:
    return "jle cs, offset";
  case Opcode::jnc_cs_offset:
    return "jnc cs, offset";
  case Opcode::jne_cs_offset:
    return "jne cs, offset";
  case Opcode::jno_cs_offset:
    return "jno cs, offset";
  case Opcode::jns_cs_offset:
    return "jns cs, offset";
  case Opcode::jnz_cs_offset:
    return "jnz cs, offset";
  case Opcode::jo_cs_offset:
    return "jo cs, offset";
  case Opcode::js_cs_offset:
    return "js cs, offset";
  case Opcode::jz_cs_offset:
    return "jz cs, offset";
  case Opcode::call_cs_offset:
    return "call cs, offset";
    // F7
  case Opcode::jmp_selector$offset:
    return "jmp selector:offset";
  case Opcode::call_selector$offset:
    return "call selector:offset";
  default:
    break;
  }

  return "unknown";
}

std::unordered_map<std::string, Opcode> strToOpcode = {
    // F0
    {"mov al, ah", Opcode::mov_al_ah},
    {"mov ah, al", Opcode::mov_ah_al},
    {"mov ds, ax", Opcode::mov_ds_ax},
    {"mov ss, ax", Opcode::mov_ss_ax},
    {"mov sp, ax", Opcode::mov_sp_ax},
    {"mov di, ax", Opcode::mov_di_ax},
    {"mov ax, ds", Opcode::mov_ax_ds},
    {"mov ax, ss", Opcode::mov_ax_ss},
    {"mov ax, sp", Opcode::mov_ax_sp},
    {"mov ax, di", Opcode::mov_ax_di},
    {"push al", Opcode::push_al},
    {"pop al", Opcode::pop_al},
    {"shl al", Opcode::shl_al},
    {"sal al", Opcode::sal_al},
    {"shr al", Opcode::shr_al},
    {"sar al", Opcode::sar_al},
    {"not al", Opcode::not_al},
    {"retn", Opcode::retn},
    {"retf", Opcode::retf},
    {"nop", Opcode::nop},
    {"htl", Opcode::htl},
    {"iret", Opcode::iret},
    {"cli", Opcode::cli},
    {"sti", Opcode::sti},
    {"ldpsr", Opcode::ldpsr},
    {"stum", Opcode::stum},
    // F1
    {"mov ds:(di), al", Opcode::mov_ds$di_al},
    {"cmp ds:(di), al", Opcode::cmp_ds$di_al},
    {"add ds:(di), al", Opcode::add_ds$di_al},
    {"sub ds:(di), al", Opcode::sub_ds$di_al},
    {"and ds:(di), al", Opcode::and_ds$di_al},
    {"or ds:(di), al", Opcode::or_ds$di_al},
    // F2
    {"mov al, ds:(di)", Opcode::mov_al_ds$di},
    // F3
    {"mov $operand, al", Opcode::mov_operand_al},
    {"cmp $operand, al", Opcode::cmp_operand_al},
    {"add $operand, al", Opcode::add_operand_al},
    {"sub $operand, al", Opcode::sub_operand_al},
    {"and $operand, al", Opcode::and_operand_al},
    {"or $operand, al", Opcode::or_operand_al},
    {"int $operand", Opcode::int_operand},
    // F4
    {"mov ds:offset, al", Opcode::mov_ds$offset_al},
    {"cmp ds:offset, al", Opcode::cmp_ds$offset_al},
    {"add ds:offset, al", Opcode::add_ds$offset_al},
    {"sub ds:offset, al", Opcode::sub_ds$offset_al},
    {"and ds:offset, al", Opcode::and_ds$offset_al},
    {"or ds:offset, al", Opcode::or_ds$offset_al},
    {"in offset, al", Opcode::in_offset_al},
    // F5
    {"mov al, ds:offset", Opcode::mov_al_ds$offset},
    {"out al, offset", Opcode::out_al_offset},
    // F6
    {"jmp cs, offset", Opcode::jmp_cs_offset},
    {"ja cs, offset", Opcode::ja_cs_offset},
    {"jae cs, offset", Opcode::jae_cs_offset},
    {"jb cs, offset", Opcode::jb_cs_offset},
    {"jbe cs, offset", Opcode::jbe_cs_offset},
    {"jc cs, offset", Opcode::jc_cs_offset},
    {"je cs, offset", Opcode::je_cs_offset},
    {"jg cs, offset", Opcode::jg_cs_offset},
    {"jge cs, offset", Opcode::jge_cs_offset},
    {"jl cs, offset", Opcode::jl_cs_offset},
    {"jle cs, offset", Opcode::jle_cs_offset},
    {"jnc cs, offset", Opcode::jnc_cs_offset},
    {"jne cs, offset", Opcode::jne_cs_offset},
    {"jno cs, offset", Opcode::jno_cs_offset},
    {"jns cs, offset", Opcode::jns_cs_offset},
    {"jnz cs, offset", Opcode::jnz_cs_offset},
    {"jo cs, offset", Opcode::jo_cs_offset},
    {"js cs, offset", Opcode::js_cs_offset},
    {"jz cs, offset", Opcode::jz_cs_offset},
    {"call cs, offset", Opcode::call_cs_offset},
    //F7
    {"jmp selector:offset", Opcode::jmp_selector$offset},
    {"call selector:offset", Opcode::call_selector$offset}
};

std::tuple<bool, Opcode> StringToOpcode(std::string const &str) {

  if (strToOpcode.find(str) == strToOpcode.end()) {
    return {false, (Opcode)0xb11111111};
  }

  return {true, strToOpcode[str]};
}