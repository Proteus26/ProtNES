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
    case 0xA9: { // LDA Immediate
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Immediate));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xA5: { // LDA Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xB5: { // LDA Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage_X));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xAD: { // LDA Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      lda(param);
      break;
    }
    case 0xBD: { // LDA Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      lda(param);
      break;
    }
    case 0xB9: { // LDA Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      lda(param);
      break;
    }
    case 0xA1: { // LDA (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xB1: { // LDA (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xA2: { // LDX Immediate
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Immediate));
      program_counter += 1;
      ldx(param);
      break;
    }
    case 0xA6: { // LDX Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage));
      program_counter += 1;
      ldx(param);
      break;
    }
    case 0xB6: { // LDX Zero Page,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage_Y));
      program_counter += 1;
      ldx(param);
      break;
    }
    case 0xAE: { // LDX Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      ldx(param);
      break;
    }
    case 0xBE: { // LDX Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      ldx(param);
      break;
    }
    case 0xA0: { // LDY Immediate
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Immediate));
      program_counter += 1;
      ldy(param);
      break;
    }
    case 0xA4: { // LDY Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage));
      program_counter += 1;
      ldy(param);
      break;
    }
    case 0xB4: { // LDY Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Zeropage_X));
      program_counter += 1;
      ldy(param);
      break;
    }
    case 0xAC: { // LDY Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      ldy(param);
      break;
    }
    case 0xBC: { // LDY Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      ldy(param);
      break;
    }
    case 0x85: { // STA Zero Page
      uint16_t addr = get_operand_address(AddressingMode::Zeropage);
      bus.write_u8(addr, register_a);
      program_counter += 1;
      break;
    }
    case 0x95: { // STA Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::Zeropage_X);
      bus.write_u8(addr, register_a);
      program_counter += 1;
      break;
    }
    case 0x8D: { // STA Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      bus.write_u8(addr, register_a);
      program_counter += 2;
      break;
    }
    case 0x9D: { // STA Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      bus.write_u8(addr, register_a);
      program_counter += 2;
      break;
    }
    case 0x99: { // STA Absolute,Y
      uint16_t addr = get_operand_address(AddressingMode::Absolute_Y);
      bus.write_u8(addr, register_a);
      program_counter += 2;
      break;
    }
    case 0x81: { // STA (Indirect,X)
      uint16_t addr = get_operand_address(AddressingMode::Indirect_X);
      bus.write_u8(addr, register_a);
      program_counter += 1;
      break;
    }
    case 0x91: { // STA (Indirect),Y
      uint16_t addr = get_operand_address(AddressingMode::Indirect_Y);
      bus.write_u8(addr, register_a);
      program_counter += 1;
      break;
    }
    case 0x86: { // STX Zero Page
      uint16_t addr = get_operand_address(AddressingMode::Zeropage);
      bus.write_u8(addr, register_x);
      program_counter += 1;
      break;
    }
    case 0x96: { // STX Zero Page,Y
      uint16_t addr = get_operand_address(AddressingMode::Zeropage_Y);
      bus.write_u8(addr, register_x);
      program_counter += 1;
      break;
    }
    case 0x8E: { // STX Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      bus.write_u8(addr, register_x);
      program_counter += 2;
      break;
    }
    case 0x84: { // STY Zero Page
      uint16_t addr = get_operand_address(AddressingMode::Zeropage);
      bus.write_u8(addr, register_y);
      program_counter += 1;
      break;
    }
    case 0x94: { // STY Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::Zeropage_X);
      bus.write_u8(addr, register_y);
      program_counter += 1;
      break;
    }
    case 0x8C: { // STY Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      bus.write_u8(addr, register_y);
      program_counter += 2;
      break;
    }
    case 0xAA: // TAX Implied
      tax();
      break;
    case 0xA8: // TAY Implied
      tay();
      break;
    case 0x8A: // TXA Implied
      txa();
      break;
    case 0x98: // TYA Implied
      tya();
      break;
    case 0xBA: // TSX Implied
      tsx();
      break;
    case 0x9A: // TXS Implied
      txs();
      break;
    case 0x48: // PHA Implied
      pha();
      break;
    case 0x68: // PLA Implied
      pla();
      break;
    case 0x08: // PHP Implied
      php();
      break;
    case 0x28: // PLP Implied
      plp();
      break;
    case 0x69: { // ADC Immediate
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
    case 0xE9: { // SBC Immediate
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
    case 0xE8: // INX Implied
      inx();
      break;
    case 0xC8: // INY Implied
      iny();
      break;
    case 0xCA: // DEX Implied
      dex();
      break;
    case 0x88: // DEY Implied
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
    case 0x0A: // ASL Accumulator
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
    case 0x4A: // LSR Accumulator
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
    case 0x2A: // ROL Accumulator
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
    case 0x6A: // ROR Accumulator
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
    case 0x60: // RTS Implied
      break;
    case 0x90: // BCC Relative
      break;
    case 0xB0: // BCS Relative
      break;
    case 0xF0: // BEQ Relative
      break;
    case 0xD0: // BNE Relative
      break;
    case 0x30: // BMI Relative
      break;
    case 0x10: // BPL Relative
      break;
    case 0x50: // BVC Relative
      break;
    case 0x70: // BVS Relative
      break;
    case 0x18: // CLC Implied
      clc();
      break;
    case 0xD8: // CLD Implied
      cld();
      break;
    case 0x58: // CLI Implied
      cli();
      break;
    case 0xB8: // CLV Implied
      clv();
      break;
    case 0x38: // SEC Implied
      sec();
      break;
    case 0xF8: // SED Implied
      sed();
      break;
    case 0x78: // SEI Implied
      sei();
      break;
    case 0x29: { // AND Immediate
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
    case 0x49: { // EOR Immediate
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
    case 0x09: { // ORA Immediate
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
    case 0xC9: { // CMP Immediate
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
    case 0xE0: { // CPX Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cpx(param);
      break;
    }
    case 0xE4: // CPX Zero Page
      break;
    case 0xEC: // CPX Absolute
      break;
    case 0xC0: { // CPY Immediate
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
    case 0x00: // BRK Implied
      return;
    case 0xEA: // NOP Implied
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
