#pragma once

#include "cpu/cpu.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

struct Opcode {
  std::string_view mnemonic = "???";
  std::uint8_t bytes = 1;  // instruction length
  std::uint8_t cycles = 0; // base cycle count
  AddressingMode mode = AddressingMode::Implied;
  void (CPU::*handler)(AddressingMode) = nullptr;

  [[nodiscard]] constexpr bool implemented() const { return handler != nullptr; }
};

namespace detail {

consteval std::array<Opcode, 256> build_opcode_table() {
  using M = AddressingMode;
  std::array<Opcode, 256> t{};

  auto op = [&t](std::uint8_t code, std::string_view name, std::uint8_t bytes,
                 std::uint8_t cycles, M mode, void (CPU::*fn)(M)) {
    t[code] = {name, bytes, cycles, mode, fn};
  };

  // clang-format off

  // Loads
  op(0xA9, "LDA", 2, 2, M::Immediate,  &CPU::lda);
  op(0xA5, "LDA", 2, 3, M::ZeroPage,   &CPU::lda);
  op(0xB5, "LDA", 2, 4, M::ZeroPage_X, &CPU::lda);
  op(0xAD, "LDA", 3, 4, M::Absolute,   &CPU::lda);
  op(0xBD, "LDA", 3, 4, M::Absolute_X, &CPU::lda);
  op(0xB9, "LDA", 3, 4, M::Absolute_Y, &CPU::lda);
  op(0xA1, "LDA", 2, 6, M::Indirect_X, &CPU::lda);
  op(0xB1, "LDA", 2, 5, M::Indirect_Y, &CPU::lda);

  op(0xA2, "LDX", 2, 2, M::Immediate,  &CPU::ldx);
  op(0xA6, "LDX", 2, 3, M::ZeroPage,   &CPU::ldx);
  op(0xB6, "LDX", 2, 4, M::ZeroPage_Y, &CPU::ldx);
  op(0xAE, "LDX", 3, 4, M::Absolute,   &CPU::ldx);
  op(0xBE, "LDX", 3, 4, M::Absolute_Y, &CPU::ldx);

  op(0xA0, "LDY", 2, 2, M::Immediate,  &CPU::ldy);
  op(0xA4, "LDY", 2, 3, M::ZeroPage,   &CPU::ldy);
  op(0xB4, "LDY", 2, 4, M::ZeroPage_X, &CPU::ldy);
  op(0xAC, "LDY", 3, 4, M::Absolute,   &CPU::ldy);
  op(0xBC, "LDY", 3, 4, M::Absolute_X, &CPU::ldy);

  // Stores
  op(0x85, "STA", 2, 3, M::ZeroPage,   &CPU::sta);
  op(0x95, "STA", 2, 4, M::ZeroPage_X, &CPU::sta);
  op(0x8D, "STA", 3, 4, M::Absolute,   &CPU::sta);
  op(0x9D, "STA", 3, 5, M::Absolute_X, &CPU::sta);
  op(0x99, "STA", 3, 5, M::Absolute_Y, &CPU::sta);
  op(0x81, "STA", 2, 6, M::Indirect_X, &CPU::sta);
  op(0x91, "STA", 2, 6, M::Indirect_Y, &CPU::sta);

  op(0x86, "STX", 2, 3, M::ZeroPage,   &CPU::stx);
  op(0x96, "STX", 2, 4, M::ZeroPage_Y, &CPU::stx);
  op(0x8E, "STX", 3, 4, M::Absolute,   &CPU::stx);

  op(0x84, "STY", 2, 3, M::ZeroPage,   &CPU::sty);
  op(0x94, "STY", 2, 4, M::ZeroPage_X, &CPU::sty);
  op(0x8C, "STY", 3, 4, M::Absolute,   &CPU::sty);

  // Transfers
  op(0xAA, "TAX", 1, 2, M::Implied, &CPU::tax);
  op(0xA8, "TAY", 1, 2, M::Implied, &CPU::tay);
  op(0x8A, "TXA", 1, 2, M::Implied, &CPU::txa);
  op(0x98, "TYA", 1, 2, M::Implied, &CPU::tya);
  op(0xBA, "TSX", 1, 2, M::Implied, &CPU::tsx);
  op(0x9A, "TXS", 1, 2, M::Implied, &CPU::txs);

  // Stack
  op(0x48, "PHA", 1, 3, M::Implied, &CPU::pha);
  op(0x08, "PHP", 1, 3, M::Implied, &CPU::php);
  op(0x68, "PLA", 1, 4, M::Implied, &CPU::pla);
  op(0x28, "PLP", 1, 4, M::Implied, &CPU::plp);

  // Arithmetic
  op(0x69, "ADC", 2, 2, M::Immediate,  &CPU::adc);
  op(0x65, "ADC", 2, 3, M::ZeroPage,   &CPU::adc);
  op(0x75, "ADC", 2, 4, M::ZeroPage_X, &CPU::adc);
  op(0x6D, "ADC", 3, 4, M::Absolute,   &CPU::adc);
  op(0x7D, "ADC", 3, 4, M::Absolute_X, &CPU::adc);
  op(0x79, "ADC", 3, 4, M::Absolute_Y, &CPU::adc);
  op(0x61, "ADC", 2, 6, M::Indirect_X, &CPU::adc);
  op(0x71, "ADC", 2, 5, M::Indirect_Y, &CPU::adc);

  op(0xE9, "SBC", 2, 2, M::Immediate,  &CPU::sbc);
  op(0xE5, "SBC", 2, 3, M::ZeroPage,   &CPU::sbc);
  op(0xF5, "SBC", 2, 4, M::ZeroPage_X, &CPU::sbc);
  op(0xED, "SBC", 3, 4, M::Absolute,   &CPU::sbc);
  op(0xFD, "SBC", 3, 4, M::Absolute_X, &CPU::sbc);
  op(0xF9, "SBC", 3, 4, M::Absolute_Y, &CPU::sbc);
  op(0xE1, "SBC", 2, 6, M::Indirect_X, &CPU::sbc);
  op(0xF1, "SBC", 2, 5, M::Indirect_Y, &CPU::sbc);

  // Logic
  op(0x29, "AND", 2, 2, M::Immediate,  &CPU::and_op);
  op(0x25, "AND", 2, 3, M::ZeroPage,   &CPU::and_op);
  op(0x35, "AND", 2, 4, M::ZeroPage_X, &CPU::and_op);
  op(0x2D, "AND", 3, 4, M::Absolute,   &CPU::and_op);
  op(0x3D, "AND", 3, 4, M::Absolute_X, &CPU::and_op);
  op(0x39, "AND", 3, 4, M::Absolute_Y, &CPU::and_op);
  op(0x21, "AND", 2, 6, M::Indirect_X, &CPU::and_op);
  op(0x31, "AND", 2, 5, M::Indirect_Y, &CPU::and_op);

  op(0x49, "EOR", 2, 2, M::Immediate,  &CPU::eor);
  op(0x45, "EOR", 2, 3, M::ZeroPage,   &CPU::eor);
  op(0x55, "EOR", 2, 4, M::ZeroPage_X, &CPU::eor);
  op(0x4D, "EOR", 3, 4, M::Absolute,   &CPU::eor);
  op(0x5D, "EOR", 3, 4, M::Absolute_X, &CPU::eor);
  op(0x59, "EOR", 3, 4, M::Absolute_Y, &CPU::eor);
  op(0x41, "EOR", 2, 6, M::Indirect_X, &CPU::eor);
  op(0x51, "EOR", 2, 5, M::Indirect_Y, &CPU::eor);

  op(0x09, "ORA", 2, 2, M::Immediate,  &CPU::ora);
  op(0x05, "ORA", 2, 3, M::ZeroPage,   &CPU::ora);
  op(0x15, "ORA", 2, 4, M::ZeroPage_X, &CPU::ora);
  op(0x0D, "ORA", 3, 4, M::Absolute,   &CPU::ora);
  op(0x1D, "ORA", 3, 4, M::Absolute_X, &CPU::ora);
  op(0x19, "ORA", 3, 4, M::Absolute_Y, &CPU::ora);
  op(0x01, "ORA", 2, 6, M::Indirect_X, &CPU::ora);
  op(0x11, "ORA", 2, 5, M::Indirect_Y, &CPU::ora);

  op(0x24, "BIT", 2, 3, M::ZeroPage, &CPU::bit);
  op(0x2C, "BIT", 3, 4, M::Absolute, &CPU::bit);

  // Comparisons
  op(0xC9, "CMP", 2, 2, M::Immediate,  &CPU::cmp);
  op(0xC5, "CMP", 2, 3, M::ZeroPage,   &CPU::cmp);
  op(0xD5, "CMP", 2, 4, M::ZeroPage_X, &CPU::cmp);
  op(0xCD, "CMP", 3, 4, M::Absolute,   &CPU::cmp);
  op(0xDD, "CMP", 3, 4, M::Absolute_X, &CPU::cmp);
  op(0xD9, "CMP", 3, 4, M::Absolute_Y, &CPU::cmp);
  op(0xC1, "CMP", 2, 6, M::Indirect_X, &CPU::cmp);
  op(0xD1, "CMP", 2, 5, M::Indirect_Y, &CPU::cmp);

  op(0xE0, "CPX", 2, 2, M::Immediate, &CPU::cpx);
  op(0xE4, "CPX", 2, 3, M::ZeroPage,  &CPU::cpx);
  op(0xEC, "CPX", 3, 4, M::Absolute,  &CPU::cpx);

  op(0xC0, "CPY", 2, 2, M::Immediate, &CPU::cpy);
  op(0xC4, "CPY", 2, 3, M::ZeroPage,  &CPU::cpy);
  op(0xCC, "CPY", 3, 4, M::Absolute,  &CPU::cpy);

  // Increments and decrements
  op(0xE6, "INC", 2, 5, M::ZeroPage,   &CPU::inc);
  op(0xF6, "INC", 2, 6, M::ZeroPage_X, &CPU::inc);
  op(0xEE, "INC", 3, 6, M::Absolute,   &CPU::inc);
  op(0xFE, "INC", 3, 7, M::Absolute_X, &CPU::inc);

  op(0xC6, "DEC", 2, 5, M::ZeroPage,   &CPU::dec);
  op(0xD6, "DEC", 2, 6, M::ZeroPage_X, &CPU::dec);
  op(0xCE, "DEC", 3, 6, M::Absolute,   &CPU::dec);
  op(0xDE, "DEC", 3, 7, M::Absolute_X, &CPU::dec);

  op(0xE8, "INX", 1, 2, M::Implied, &CPU::inx);
  op(0xC8, "INY", 1, 2, M::Implied, &CPU::iny);
  op(0xCA, "DEX", 1, 2, M::Implied, &CPU::dex);
  op(0x88, "DEY", 1, 2, M::Implied, &CPU::dey);

  // Shifts and rotates
  op(0x0A, "ASL", 1, 2, M::Accumulator, &CPU::asl);
  op(0x06, "ASL", 2, 5, M::ZeroPage,    &CPU::asl);
  op(0x16, "ASL", 2, 6, M::ZeroPage_X,  &CPU::asl);
  op(0x0E, "ASL", 3, 6, M::Absolute,    &CPU::asl);
  op(0x1E, "ASL", 3, 7, M::Absolute_X,  &CPU::asl);

  op(0x4A, "LSR", 1, 2, M::Accumulator, &CPU::lsr);
  op(0x46, "LSR", 2, 5, M::ZeroPage,    &CPU::lsr);
  op(0x56, "LSR", 2, 6, M::ZeroPage_X,  &CPU::lsr);
  op(0x4E, "LSR", 3, 6, M::Absolute,    &CPU::lsr);
  op(0x5E, "LSR", 3, 7, M::Absolute_X,  &CPU::lsr);

  op(0x2A, "ROL", 1, 2, M::Accumulator, &CPU::rol);
  op(0x26, "ROL", 2, 5, M::ZeroPage,    &CPU::rol);
  op(0x36, "ROL", 2, 6, M::ZeroPage_X,  &CPU::rol);
  op(0x2E, "ROL", 3, 6, M::Absolute,    &CPU::rol);
  op(0x3E, "ROL", 3, 7, M::Absolute_X,  &CPU::rol);

  op(0x6A, "ROR", 1, 2, M::Accumulator, &CPU::ror);
  op(0x66, "ROR", 2, 5, M::ZeroPage,    &CPU::ror);
  op(0x76, "ROR", 2, 6, M::ZeroPage_X,  &CPU::ror);
  op(0x6E, "ROR", 3, 6, M::Absolute,    &CPU::ror);
  op(0x7E, "ROR", 3, 7, M::Absolute_X,  &CPU::ror);

  // Jumps and subroutines
  op(0x4C, "JMP", 3, 3, M::Absolute, &CPU::jmp);
  op(0x6C, "JMP", 3, 5, M::Indirect, &CPU::jmp);
  op(0x20, "JSR", 3, 6, M::Absolute, &CPU::jsr);
  op(0x60, "RTS", 1, 6, M::Implied,  &CPU::rts);
  op(0x40, "RTI", 1, 6, M::Implied,  &CPU::rti);

  // Branches
  op(0x90, "BCC", 2, 2, M::Relative, &CPU::bcc);
  op(0xB0, "BCS", 2, 2, M::Relative, &CPU::bcs);
  op(0xF0, "BEQ", 2, 2, M::Relative, &CPU::beq);
  op(0xD0, "BNE", 2, 2, M::Relative, &CPU::bne);
  op(0x30, "BMI", 2, 2, M::Relative, &CPU::bmi);
  op(0x10, "BPL", 2, 2, M::Relative, &CPU::bpl);
  op(0x50, "BVC", 2, 2, M::Relative, &CPU::bvc);
  op(0x70, "BVS", 2, 2, M::Relative, &CPU::bvs);

  // Flags
  op(0x18, "CLC", 1, 2, M::Implied, &CPU::clc);
  op(0xD8, "CLD", 1, 2, M::Implied, &CPU::cld);
  op(0x58, "CLI", 1, 2, M::Implied, &CPU::cli);
  op(0xB8, "CLV", 1, 2, M::Implied, &CPU::clv);
  op(0x38, "SEC", 1, 2, M::Implied, &CPU::sec);
  op(0xF8, "SED", 1, 2, M::Implied, &CPU::sed);
  op(0x78, "SEI", 1, 2, M::Implied, &CPU::sei);

  // System
  op(0x00, "BRK", 1, 7, M::Implied, &CPU::brk);
  op(0xEA, "NOP", 1, 2, M::Implied, &CPU::nop);

  // clang-format on

  return t;
}

} // namespace detail

inline constexpr std::array<Opcode, 256> OPCODES = detail::build_opcode_table();

// Compile-time checks
static_assert(std::ranges::count_if(OPCODES, &Opcode::implemented) == 151);
static_assert(std::ranges::all_of(OPCODES, [](const Opcode &op) {
  return op.bytes >= 1 && op.bytes <= 3;
}));
static_assert(std::ranges::all_of(OPCODES, [](const Opcode &op) {
  return !op.implemented() || op.cycles >= 2;
}));
