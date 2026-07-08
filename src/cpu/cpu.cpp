#include "cpu/cpu.hpp"
#include "bus/bus.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

CPU::CPU(Bus &bus)
    : register_a(0), register_x(0), register_y(0), status(0),
      program_counter(0), bus(bus) {}

void CPU::reset() {
  register_a = 0;
  register_x = 0;
  register_y = 0;
  status = 0;
  program_counter = bus.read_u16(0xFFFC);
}

void CPU::load(const std::vector<uint8_t> &program) {
  bus.load_program(program);
}

void CPU::load_and_run(const std::vector<uint8_t> &program) {
  load(program);
  reset();
  run();
}

uint16_t CPU::get_operand_address(AddressingMode mode) const {
  switch (mode) {
  case AddressingMode::Immediate:
    return program_counter;

  case AddressingMode::Zeropage:
    return bus.read_u8(program_counter);

  case AddressingMode::Zeropage_X: {
    uint8_t pos = bus.read_u8(program_counter);
    return static_cast<uint8_t>(pos + register_x);
  }

  case AddressingMode::Zeropage_Y: {
    uint8_t pos = bus.read_u8(program_counter);
    return static_cast<uint8_t>(pos + register_y);
  }

  case AddressingMode::Absolute:
    return bus.read_u16(program_counter);

  case AddressingMode::Absolute_X: {
    uint16_t base = bus.read_u16(program_counter);
    return static_cast<uint16_t>(base + register_x);
  }

  case AddressingMode::Absolute_Y: {
    uint16_t base = bus.read_u16(program_counter);
    return static_cast<uint16_t>(base + register_y);
  }

  case AddressingMode::Indirect_X: {
    uint8_t base = bus.read_u8(program_counter);
    uint8_t ptr = static_cast<uint8_t>(base + register_x);
    uint8_t lo = bus.read_u8(ptr);
    uint8_t hi = bus.read_u8(static_cast<uint8_t>(ptr + 1));
    return static_cast<uint16_t>(lo | (hi << 8));
  }

  case AddressingMode::Indirect_Y: {
    uint8_t base = bus.read_u8(program_counter);
    uint8_t lo = bus.read_u8(base);
    uint8_t hi = bus.read_u8(static_cast<uint8_t>(base + 1));
    uint16_t deref_base = static_cast<uint16_t>(lo | (hi << 8));
    return static_cast<uint16_t>(deref_base + register_y);
  }

  case AddressingMode::NoneAddressing:
  default:
    throw std::runtime_error("AddressingMode not supported for this operation");
  }
}

void CPU::run() {
  while (true) {

    uint8_t opcode = bus.read_u8(program_counter);
    program_counter++;

    switch (opcode) {
    case 0xA9: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      lda(param);
      break;
    }
    case 0xA5: // LDA Zero Page
      break;
    case 0xB5: // LDA Zero Page,X
      break;
    case 0xAD: // LDA Absolute
      break;
    case 0xBD: // LDA Absolute,X
      break;
    case 0xB9: // LDA Absolute,Y
      break;
    case 0xA1: // LDA (Indirect,X)
      break;
    case 0xB1: // LDA (Indirect),Y
      break;
    case 0xA2: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      ldx(param);
      break;
    }
    case 0xA6: // LDX Zero Page
      break;
    case 0xB6: // LDX Zero Page,Y
      break;
    case 0xAE: // LDX Absolute
      break;
    case 0xBE: // LDX Absolute,Y
      break;
    case 0xA0: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      ldy(param);
      break;
    }
    case 0xA4: // LDY Zero Page
      break;
    case 0xB4: // LDY Zero Page,X
      break;
    case 0xAC: // LDY Absolute
      break;
    case 0xBC: // LDY Absolute,X
      break;
    case 0x85: // STA Zero Page
      break;
    case 0x95: // STA Zero Page,X
      break;
    case 0x8D: // STA Absolute
      break;
    case 0x9D: // STA Absolute,X
      break;
    case 0x99: // STA Absolute,Y
      break;
    case 0x81: // STA (Indirect,X)
      break;
    case 0x91: // STA (Indirect),Y
      break;
    case 0x86: // STX Zero Page
      break;
    case 0x96: // STX Zero Page,Y
      break;
    case 0x8E: // STX Absolute
      break;
    case 0x84: // STY Zero Page
      break;
    case 0x94: // STY Zero Page,X
      break;
    case 0x8C: // STY Absolute
      break;
    case 0xAA:
      tax();
      break;
    case 0xA8:
      tay();
      break;
    case 0x8A:
      txa();
      break;
    case 0x98:
      tya();
      break;
    case 0xBA:
      tsx();
      break;
    case 0x9A:
      txs();
      break;
    case 0x48:
      pha();
      break;
    case 0x68:
      pla();
      break;
    case 0x08:
      php();
      break;
    case 0x28:
      plp();
      break;
    case 0x69: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      adc(param);
      break;
    }
    case 0x65: // ADC Zero Page
      break;
    case 0x75: // ADC Zero Page,X
      break;
    case 0x6D: // ADC Absolute
      break;
    case 0x7D: // ADC Absolute,X
      break;
    case 0x79: // ADC Absolute,Y
      break;
    case 0x61: // ADC (Indirect,X)
      break;
    case 0x71: // ADC (Indirect),Y
      break;
    case 0xE9: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      sbc(param);
      break;
    }
    case 0xE5: // SBC Zero Page
      break;
    case 0xF5: // SBC Zero Page,X
      break;
    case 0xED: // SBC Absolute
      break;
    case 0xFD: // SBC Absolute,X
      break;
    case 0xF9: // SBC Absolute,Y
      break;
    case 0xE1: // SBC (Indirect,X)
      break;
    case 0xF1: // SBC (Indirect),Y
      break;
    case 0xE8:
      inx();
      break;
    case 0xC8:
      iny();
      break;
    case 0xCA:
      dex();
      break;
    case 0x88:
      dey();
      break;
    case 0xE6: // INC Zero Page
      break;
    case 0xF6: // INC Zero Page,X
      break;
    case 0xEE: // INC Absolute
      break;
    case 0xFE: // INC Absolute,X
      break;
    case 0xC6: // DEC Zero Page
      break;
    case 0xD6: // DEC Zero Page,X
      break;
    case 0xCE: // DEC Absolute
      break;
    case 0xDE: // DEC Absolute,X
      break;
    case 0x0A:
      asl_accumulator();
      break;
    case 0x06: // ASL Zero Page
      break;
    case 0x16: // ASL Zero Page,X
      break;
    case 0x0E: // ASL Absolute
      break;
    case 0x1E: // ASL Absolute,X
      break;
    case 0x4A:
      lsr_accumulator();
      break;
    case 0x46: // LSR Zero Page
      break;
    case 0x56: // LSR Zero Page,X
      break;
    case 0x4E: // LSR Absolute
      break;
    case 0x5E: // LSR Absolute,X
      break;
    case 0x2A:
      rol_accumulator();
      break;
    case 0x26: // ROL Zero Page
      break;
    case 0x36: // ROL Zero Page,X
      break;
    case 0x2E: // ROL Absolute
      break;
    case 0x3E: // ROL Absolute,X
      break;
    case 0x6A:
      ror_accumulator();
      break;
    case 0x66: // ROR Zero Page
      break;
    case 0x76: // ROR Zero Page,X
      break;
    case 0x6E: // ROR Absolute
      break;
    case 0x7E: // ROR Absolute,X
      break;
    case 0x4C: // JMP Absolute
      break;
    case 0x6C: // JMP Indirect
      break;
    case 0x20: // JSR Absolute
      break;
    case 0x60: // RTS
      break;
    case 0x90: // BCC
      break;
    case 0xB0: // BCS
      break;
    case 0xF0: // BEQ
      break;
    case 0xD0: // BNE
      break;
    case 0x30: // BMI
      break;
    case 0x10: // BPL
      break;
    case 0x50: // BVC
      break;
    case 0x70: // BVS
      break;
    case 0x18:
      clc();
      break;
    case 0xD8:
      cld();
      break;
    case 0x58:
      cli();
      break;
    case 0xB8:
      clv();
      break;
    case 0x38:
      sec();
      break;
    case 0xF8:
      sed();
      break;
    case 0x78:
      sei();
      break;
    case 0x29: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      and_op(param);
      break;
    }
    case 0x25: // AND Zero Page
      break;
    case 0x35: // AND Zero Page,X
      break;
    case 0x2D: // AND Absolute
      break;
    case 0x3D: // AND Absolute,X
      break;
    case 0x39: // AND Absolute,Y
      break;
    case 0x21: // AND (Indirect,X)
      break;
    case 0x31: // AND (Indirect),Y
      break;
    case 0x49: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      eor(param);
      break;
    }
    case 0x45: // EOR Zero Page
      break;
    case 0x55: // EOR Zero Page,X
      break;
    case 0x4D: // EOR Absolute
      break;
    case 0x5D: // EOR Absolute,X
      break;
    case 0x59: // EOR Absolute,Y
      break;
    case 0x41: // EOR (Indirect,X)
      break;
    case 0x51: // EOR (Indirect),Y
      break;
    case 0x09: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      ora(param);
      break;
    }
    case 0x05: // ORA Zero Page
      break;
    case 0x15: // ORA Zero Page,X
      break;
    case 0x0D: // ORA Absolute
      break;
    case 0x1D: // ORA Absolute,X
      break;
    case 0x19: // ORA Absolute,Y
      break;
    case 0x01: // ORA (Indirect,X)
      break;
    case 0x11: // ORA (Indirect),Y
      break;
    case 0xC9: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cmp(param);
      break;
    }
    case 0xC5: // CMP Zero Page
      break;
    case 0xD5: // CMP Zero Page,X
      break;
    case 0xCD: // CMP Absolute
      break;
    case 0xDD: // CMP Absolute,X
      break;
    case 0xD9: // CMP Absolute,Y
      break;
    case 0xC1: // CMP (Indirect,X)
      break;
    case 0xD1: // CMP (Indirect),Y
      break;
    case 0xE0: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cpx(param);
      break;
    }
    case 0xE4: // CPX Zero Page
      break;
    case 0xEC: // CPX Absolute
      break;
    case 0xC0: {
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cpy(param);
      break;
    }
    case 0xC4: // CPY Zero Page
      break;
    case 0xCC: // CPY Absolute
      break;
    case 0x24: // BIT Zero Page
      break;
    case 0x2C: // BIT Absolute
      break;
    case 0x00:
      return;
    case 0xEA:
      break;
    default:
      throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
    }
  }
}

void CPU::lda(uint8_t value) {
  register_a = value;
  update_zero_and_negative_flags(register_a);
}

void CPU::ldx(uint8_t value) {
  register_x = value;
  update_zero_and_negative_flags(register_x);
}

void CPU::ldy(uint8_t value) {
  register_y = value;
  update_zero_and_negative_flags(register_y);
}

void CPU::tax() {
  register_x = register_a;
  update_zero_and_negative_flags(register_x);
}

void CPU::tay() {
  register_y = register_a;
  update_zero_and_negative_flags(register_y);
}

void CPU::txa() {
  register_a = register_x;
  update_zero_and_negative_flags(register_a);
}

void CPU::tya() {
  register_a = register_y;
  update_zero_and_negative_flags(register_a);
}

void CPU::tsx() {
  register_x = status;
  update_zero_and_negative_flags(register_x);
}

void CPU::txs() { status = register_x; }

void CPU::pha() {}

void CPU::pla() { update_zero_and_negative_flags(register_a); }

void CPU::php() {}

void CPU::plp() {}

void CPU::adc(uint8_t value) {
  uint16_t sum = static_cast<uint16_t>(register_a) + value + (status & 1);
  bool carry = sum > 0xFF;
  if (carry) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
  uint8_t result = static_cast<uint8_t>(sum);
  if ((register_a ^ result) & (value ^ result) & 0x80) {
    status |= 0b01000000;
  } else {
    status &= 0b10111111;
  }
  register_a = result;
  update_zero_and_negative_flags(register_a);
}

void CPU::sbc(uint8_t value) { adc(static_cast<uint8_t>(-value - 1)); }

void CPU::inx() {
  register_x++;
  update_zero_and_negative_flags(register_x);
}

void CPU::iny() {
  register_y++;
  update_zero_and_negative_flags(register_y);
}

void CPU::dex() {
  register_x--;
  update_zero_and_negative_flags(register_x);
}

void CPU::dey() {
  register_y--;
  update_zero_and_negative_flags(register_y);
}

void CPU::asl_accumulator() {
  uint8_t old_a = register_a;
  register_a <<= 1;
  update_zero_and_negative_flags(register_a);
  if (old_a & 0b10000000) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::lsr_accumulator() {
  uint8_t old_a = register_a;
  register_a >>= 1;
  update_zero_and_negative_flags(register_a);
  if (old_a & 0b00000001) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::rol_accumulator() {
  uint8_t old_carry = status & 1;
  uint8_t old_a = register_a;
  register_a <<= 1;
  register_a |= old_carry;
  update_zero_and_negative_flags(register_a);
  if (old_a & 0b10000000) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::ror_accumulator() {
  uint8_t old_carry = status & 1;
  uint8_t old_a = register_a;
  register_a >>= 1;
  register_a |= (old_carry << 7);
  update_zero_and_negative_flags(register_a);
  if (old_a & 0b00000001) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::clc() { status &= 0b11111110; }

void CPU::cld() { status &= 0b11110111; }

void CPU::cli() { status &= 0b11111011; }

void CPU::clv() { status &= 0b10111111; }

void CPU::sec() { status |= 0b00000001; }

void CPU::sed() { status |= 0b00001000; }

void CPU::sei() { status |= 0b00000100; }

void CPU::and_op(uint8_t value) {
  register_a &= value;
  update_zero_and_negative_flags(register_a);
}

void CPU::eor(uint8_t value) {
  register_a ^= value;
  update_zero_and_negative_flags(register_a);
}

void CPU::ora(uint8_t value) {
  register_a |= value;
  update_zero_and_negative_flags(register_a);
}

void CPU::cpy(uint8_t value) {
  uint8_t result = register_y - value;
  update_zero_and_negative_flags(result);

  if (register_y >= value) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::cpx(uint8_t value) {
  uint8_t result = register_x - value;
  update_zero_and_negative_flags(result);

  if (register_x >= value) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::cmp(uint8_t value) {
  uint8_t result = register_a - value;
  update_zero_and_negative_flags(result);

  if (register_a >= value) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::bit(uint8_t value) {
  uint8_t result = register_a & value;
  if (result == 0) {
    status |= 0b00000010;
  } else {
    status &= 0b11111101;
  }
  status = (status & 0b00111111) | (value & 0b11000000);
}

void CPU::update_zero_and_negative_flags(uint8_t result) {
  if (result == 0) {
    status |= 0b00000010;
  } else {
    status &= 0b11111101;
  }

  if (result & 0b10000000) {
    status |= 0b10000000;
  } else {
    status &= 0b01111111;
  }
}
