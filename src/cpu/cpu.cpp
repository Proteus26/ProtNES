#include "cpu/cpu.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

CPU::CPU(Bus &bus)
    : register_a(0), register_x(0), register_y(0), status(0),
      program_counter(0), stack_pointer(STACK_RESET), bus(bus) {}

void CPU::reset() {
  register_a = 0;
  register_x = 0;
  register_y = 0;
  status = 0;
  stack_pointer = STACK_RESET;
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

  case AddressingMode::ZeroPage:
    return bus.read_u8(program_counter);

  case AddressingMode::Absolute:
    return bus.read_u16(program_counter);

  case AddressingMode::ZeroPage_X: {
    uint8_t pos = bus.read_u8(program_counter);
    return static_cast<uint8_t>(pos + register_x);
  }

  case AddressingMode::ZeroPage_Y: {
    uint8_t pos = bus.read_u8(program_counter);
    return static_cast<uint8_t>(pos + register_y);
  }

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

void CPU::stack_push(uint8_t value) {
  bus.write_u8(static_cast<uint16_t>(STACK_BASE + stack_pointer), value);
  stack_pointer--;
}

uint8_t CPU::stack_pop() {
  stack_pointer++;
  return bus.read_u8(static_cast<uint16_t>(STACK_BASE + stack_pointer));
}

void CPU::stack_push_u16(uint16_t value) {
  uint8_t hi = static_cast<uint8_t>(value >> 8);
  uint8_t lo = static_cast<uint8_t>(value & 0xFF);
  stack_push(hi);
  stack_push(lo);
}

uint16_t CPU::stack_pop_u16() {
  uint8_t lo = stack_pop();
  uint8_t hi = stack_pop();
  return static_cast<uint16_t>(lo | (static_cast<uint16_t>(hi) << 8));
}

void CPU::branch(bool condition) {
  int8_t offset = static_cast<int8_t>(bus.read_u8(program_counter));
  program_counter += 1;
  if (condition) {
    program_counter = static_cast<uint16_t>(program_counter + offset);
  }
}

void CPU::run(const std::function<void(CPU &)> &trace_callback) {
  while (true) {
    if (trace_callback) {
      trace_callback(*this);
    }

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
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      lda(param);
      break;
    }
    case 0xB5: { // LDA Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
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
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      ldx(param);
      break;
    }
    case 0xB6: { // LDX Zero Page,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_Y));
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
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      ldy(param);
      break;
    }
    case 0xB4: { // LDY Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
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
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      bus.write_u8(addr, register_a);
      program_counter += 1;
      break;
    }
    case 0x95: { // STA Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
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
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      bus.write_u8(addr, register_x);
      program_counter += 1;
      break;
    }
    case 0x96: { // STX Zero Page,Y
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_Y);
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
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      bus.write_u8(addr, register_y);
      program_counter += 1;
      break;
    }
    case 0x94: { // STY Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
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
    case 0xAA: // TAX
      tax();
      break;
    case 0xA8: // TAY
      tay();
      break;
    case 0x8A: // TXA
      txa();
      break;
    case 0x98: // TYA
      tya();
      break;
    case 0xBA: // TSX
      tsx();
      break;
    case 0x9A: // TXS
      txs();
      break;
    case 0x48: // PHA
      pha();
      break;
    case 0x68: // PLA
      pla();
      break;
    case 0x08: // PHP
      php();
      break;
    case 0x28: // PLP
      plp();
      break;
    case 0x69: { // ADC Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      adc(param);
      break;
    }
    case 0x65: { // ADC Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      adc(param);
      break;
    }
    case 0x75: { // ADC Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      adc(param);
      break;
    }
    case 0x6D: { // ADC Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      adc(param);
      break;
    }
    case 0x7D: { // ADC Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      adc(param);
      break;
    }
    case 0x79: { // ADC Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      adc(param);
      break;
    }
    case 0x61: { // ADC (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      adc(param);
      break;
    }
    case 0x71: { // ADC (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      adc(param);
      break;
    }
    case 0xE9: { // SBC Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      sbc(param);
      break;
    }
    case 0xE5: { // SBC Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      sbc(param);
      break;
    }
    case 0xF5: { // SBC Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      sbc(param);
      break;
    }
    case 0xED: { // SBC Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      sbc(param);
      break;
    }
    case 0xFD: { // SBC Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      sbc(param);
      break;
    }
    case 0xF9: { // SBC Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      sbc(param);
      break;
    }
    case 0xE1: { // SBC (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      sbc(param);
      break;
    }
    case 0xF1: { // SBC (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      sbc(param);
      break;
    }
    case 0xE8: // INX
      inx();
      break;
    case 0xC8: // INY
      iny();
      break;
    case 0xCA: // DEX
      dex();
      break;
    case 0x88: // DEY
      dey();
      break;
    case 0xE6: { // INC Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      inc(addr);
      program_counter += 1;
      break;
    }
    case 0xF6: { // INC Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      inc(addr);
      program_counter += 1;
      break;
    }
    case 0xEE: { // INC Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      inc(addr);
      program_counter += 2;
      break;
    }
    case 0xFE: { // INC Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      inc(addr);
      program_counter += 2;
      break;
    }
    case 0xC6: { // DEC Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      dec(addr);
      program_counter += 1;
      break;
    }
    case 0xD6: { // DEC Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      dec(addr);
      program_counter += 1;
      break;
    }
    case 0xCE: { // DEC Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      dec(addr);
      program_counter += 2;
      break;
    }
    case 0xDE: { // DEC Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      dec(addr);
      program_counter += 2;
      break;
    }
    case 0x0A: // ASL Accumulator
      asl_accumulator();
      break;
    case 0x06: { // ASL Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      asl(addr);
      program_counter += 1;
      break;
    }
    case 0x16: { // ASL Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      asl(addr);
      program_counter += 1;
      break;
    }
    case 0x0E: { // ASL Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      asl(addr);
      program_counter += 2;
      break;
    }
    case 0x1E: { // ASL Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      asl(addr);
      program_counter += 2;
      break;
    }
    case 0x4A: // LSR Accumulator
      lsr_accumulator();
      break;
    case 0x46: { // LSR Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      lsr(addr);
      program_counter += 1;
      break;
    }
    case 0x56: { // LSR Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      lsr(addr);
      program_counter += 1;
      break;
    }
    case 0x4E: { // LSR Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      lsr(addr);
      program_counter += 2;
      break;
    }
    case 0x5E: { // LSR Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      lsr(addr);
      program_counter += 2;
      break;
    }
    case 0x2A: // ROL Accumulator
      rol_accumulator();
      break;
    case 0x26: { // ROL Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      rol(addr);
      program_counter += 1;
      break;
    }
    case 0x36: { // ROL Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      rol(addr);
      program_counter += 1;
      break;
    }
    case 0x2E: { // ROL Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      rol(addr);
      program_counter += 2;
      break;
    }
    case 0x3E: { // ROL Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      rol(addr);
      program_counter += 2;
      break;
    }
    case 0x6A: // ROR Accumulator
      ror_accumulator();
      break;
    case 0x66: { // ROR Zero Page
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage);
      ror(addr);
      program_counter += 1;
      break;
    }
    case 0x76: { // ROR Zero Page,X
      uint16_t addr = get_operand_address(AddressingMode::ZeroPage_X);
      ror(addr);
      program_counter += 1;
      break;
    }
    case 0x6E: { // ROR Absolute
      uint16_t addr = get_operand_address(AddressingMode::Absolute);
      ror(addr);
      program_counter += 2;
      break;
    }
    case 0x7E: { // ROR Absolute,X
      uint16_t addr = get_operand_address(AddressingMode::Absolute_X);
      ror(addr);
      program_counter += 2;
      break;
    }
    case 0x4C: { // JMP Absolute
      uint16_t addr = bus.read_u16(program_counter);
      program_counter = addr;
      break;
    }
    case 0x6C: { // JMP Indirect
      uint16_t ptr = bus.read_u16(program_counter);
      uint16_t target;
      if ((ptr & 0x00FF) == 0x00FF) {
        uint8_t lo = bus.read_u8(ptr);
        uint8_t hi = bus.read_u8(ptr & 0xFF00);
        target = static_cast<uint16_t>(lo | (hi << 8));
      } else {
        target = bus.read_u16(ptr);
      }
      program_counter = target;
      break;
    }
    case 0x20: { // JSR Absolute
      uint16_t addr = bus.read_u16(program_counter);
      stack_push_u16(static_cast<uint16_t>(program_counter + 1));
      program_counter = addr;
      break;
    }
    case 0x60: { // RTS
      uint16_t addr = stack_pop_u16();
      program_counter = static_cast<uint16_t>(addr + 1);
      break;
    }
    case 0x40: { // RTI
      status = (stack_pop() & 0b11101111) | 0b00100000;
      program_counter = stack_pop_u16();
      break;
    }
    case 0x90: // BCC
      branch((status & 0b00000001) == 0);
      break;
    case 0xB0: // BCS
      branch((status & 0b00000001) != 0);
      break;
    case 0xF0: // BEQ
      branch((status & 0b00000010) != 0);
      break;
    case 0xD0: // BNE
      branch((status & 0b00000010) == 0);
      break;
    case 0x30: // BMI
      branch((status & 0b10000000) != 0);
      break;
    case 0x10: // BPL
      branch((status & 0b10000000) == 0);
      break;
    case 0x50: // BVC
      branch((status & 0b01000000) == 0);
      break;
    case 0x70: // BVS
      branch((status & 0b01000000) != 0);
      break;
    case 0x18: // CLC
      clc();
      break;
    case 0xD8: // CLD
      cld();
      break;
    case 0x58: // CLI
      cli();
      break;
    case 0xB8: // CLV
      clv();
      break;
    case 0x38: // SEC
      sec();
      break;
    case 0xF8: // SED
      sed();
      break;
    case 0x78: // SEI
      sei();
      break;
    case 0x29: { // AND Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      and_op(param);
      break;
    }
    case 0x25: { // AND Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      and_op(param);
      break;
    }
    case 0x35: { // AND Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      and_op(param);
      break;
    }
    case 0x2D: { // AND Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      and_op(param);
      break;
    }
    case 0x3D: { // AND Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      and_op(param);
      break;
    }
    case 0x39: { // AND Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      and_op(param);
      break;
    }
    case 0x21: { // AND (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      and_op(param);
      break;
    }
    case 0x31: { // AND (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      and_op(param);
      break;
    }
    case 0x49: { // EOR Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      eor(param);
      break;
    }
    case 0x45: { // EOR Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      eor(param);
      break;
    }
    case 0x55: { // EOR Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      eor(param);
      break;
    }
    case 0x4D: { // EOR Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      eor(param);
      break;
    }
    case 0x5D: { // EOR Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      eor(param);
      break;
    }
    case 0x59: { // EOR Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      eor(param);
      break;
    }
    case 0x41: { // EOR (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      eor(param);
      break;
    }
    case 0x51: { // EOR (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      eor(param);
      break;
    }
    case 0x09: { // ORA Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      ora(param);
      break;
    }
    case 0x05: { // ORA Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      ora(param);
      break;
    }
    case 0x15: { // ORA Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      ora(param);
      break;
    }
    case 0x0D: { // ORA Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      ora(param);
      break;
    }
    case 0x1D: { // ORA Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      ora(param);
      break;
    }
    case 0x19: { // ORA Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      ora(param);
      break;
    }
    case 0x01: { // ORA (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      ora(param);
      break;
    }
    case 0x11: { // ORA (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      ora(param);
      break;
    }
    case 0xC9: { // CMP Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cmp(param);
      break;
    }
    case 0xC5: { // CMP Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      cmp(param);
      break;
    }
    case 0xD5: { // CMP Zero Page,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage_X));
      program_counter += 1;
      cmp(param);
      break;
    }
    case 0xCD: { // CMP Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      cmp(param);
      break;
    }
    case 0xDD: { // CMP Absolute,X
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_X));
      program_counter += 2;
      cmp(param);
      break;
    }
    case 0xD9: { // CMP Absolute,Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute_Y));
      program_counter += 2;
      cmp(param);
      break;
    }
    case 0xC1: { // CMP (Indirect,X)
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_X));
      program_counter += 1;
      cmp(param);
      break;
    }
    case 0xD1: { // CMP (Indirect),Y
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Indirect_Y));
      program_counter += 1;
      cmp(param);
      break;
    }
    case 0xE0: { // CPX Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cpx(param);
      break;
    }
    case 0xE4: { // CPX Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      cpx(param);
      break;
    }
    case 0xEC: { // CPX Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      cpx(param);
      break;
    }
    case 0xC0: { // CPY Immediate
      uint8_t param = bus.read_u8(program_counter);
      program_counter++;
      cpy(param);
      break;
    }
    case 0xC4: { // CPY Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      cpy(param);
      break;
    }
    case 0xCC: { // CPY Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      cpy(param);
      break;
    }
    case 0x24: { // BIT Zero Page
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::ZeroPage));
      program_counter += 1;
      bit(param);
      break;
    }
    case 0x2C: { // BIT Absolute
      uint8_t param =
          bus.read_u8(get_operand_address(AddressingMode::Absolute));
      program_counter += 2;
      bit(param);
      break;
    }
    case 0x00: // BRK
      return;
    case 0xEA: // NOP
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
  register_x = stack_pointer;
  update_zero_and_negative_flags(register_x);
}

void CPU::txs() { stack_pointer = register_x; }

void CPU::pha() { stack_push(register_a); }

void CPU::pla() {
  register_a = stack_pop();
  update_zero_and_negative_flags(register_a);
}

void CPU::php() { stack_push(status | 0b00110000); }

void CPU::plp() { status = (stack_pop() & 0b11101111) | 0b00100000; }

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

void CPU::inc(uint16_t addr) {
  uint8_t value = static_cast<uint8_t>(bus.read_u8(addr) + 1);
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
}

void CPU::dec(uint16_t addr) {
  uint8_t value = static_cast<uint8_t>(bus.read_u8(addr) - 1);
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
}

void CPU::asl(uint16_t addr) {
  uint8_t old_value = bus.read_u8(addr);
  uint8_t value = static_cast<uint8_t>(old_value << 1);
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
  if (old_value & 0b10000000) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::lsr(uint16_t addr) {
  uint8_t old_value = bus.read_u8(addr);
  uint8_t value = static_cast<uint8_t>(old_value >> 1);
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
  if (old_value & 0b00000001) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::rol(uint16_t addr) {
  uint8_t old_carry = status & 1;
  uint8_t old_value = bus.read_u8(addr);
  uint8_t value = static_cast<uint8_t>(old_value << 1);
  value |= old_carry;
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
  if (old_value & 0b10000000) {
    status |= 0b00000001;
  } else {
    status &= 0b11111110;
  }
}

void CPU::ror(uint16_t addr) {
  uint8_t old_carry = status & 1;
  uint8_t old_value = bus.read_u8(addr);
  uint8_t value = static_cast<uint8_t>(old_value >> 1);
  value = static_cast<uint8_t>(value | (old_carry << 7));
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
  if (old_value & 0b00000001) {
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
