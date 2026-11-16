#ifndef OPCODES_HPP
#define OPCODES_HPP

#include "cpu/cpu.hpp"
#include <array>
#include <cstdint>

struct Opcode {
  const char *mnemonic = "???";
  std::uint8_t bytes = 1;  // instruction length
  std::uint8_t cycles = 0; // base cycle count
  AddressingMode mode = AddressingMode::Implied;
  void (CPU::*handler)(AddressingMode) = nullptr;
};

extern const std::array<Opcode, 256> OPCODES;

#endif // OPCODES_HPP
