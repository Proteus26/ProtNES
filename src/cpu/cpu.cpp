#include "cpu/cpu.hpp"
#include "cpu/opcodes.hpp"
#include <cstdint>
#include <format>
#include <stdexcept>
#include <vector>

void CPU::reset() {
  register_a = 0;
  register_x = 0;
  register_y = 0;
  status = 0;
  stack_pointer = STACK_RESET;
  program_counter = bus.read_u16(Bus::RESET_VECTOR);
  halted = false;
}

void CPU::load(const std::vector<std::uint8_t> &program) {
  bus.load_program(program);
}

void CPU::load_and_run(const std::vector<std::uint8_t> &program) {
  load(program);
  reset();
  run();
}

// Execution

void CPU::run(const std::function<void(CPU &)> &trace_callback) {
  halted = false;
  while (!halted) {
    if (trace_callback) {
      trace_callback(*this);
    }
    step();
  }
}

void CPU::step() {
  std::uint8_t code = bus.read_u8(program_counter);
  const Opcode &op = OPCODES[code];

  if (!op.implemented()) [[unlikely]] {
    throw std::runtime_error(std::format("Unknown opcode ${:02X} at ${:04X}",
                                         code, program_counter));
  }

  operand_pc = static_cast<std::uint16_t>(program_counter + 1);
  program_counter = static_cast<std::uint16_t>(program_counter + op.bytes);

  (this->*op.handler)(op.mode);
}

std::uint16_t CPU::get_operand_address(AddressingMode mode) const {
  switch (mode) {
  case Mode::Immediate:
  case Mode::Relative:
    return operand_pc;

  case Mode::ZeroPage:
    return bus.read_u8(operand_pc);

  case Mode::ZeroPage_X:
    return static_cast<std::uint8_t>(bus.read_u8(operand_pc) + register_x);

  case Mode::ZeroPage_Y:
    return static_cast<std::uint8_t>(bus.read_u8(operand_pc) + register_y);

  case Mode::Absolute:
    return bus.read_u16(operand_pc);

  case Mode::Absolute_X:
    return static_cast<std::uint16_t>(bus.read_u16(operand_pc) + register_x);

  case Mode::Absolute_Y:
    return static_cast<std::uint16_t>(bus.read_u16(operand_pc) + register_y);

  case Mode::Indirect: {
    std::uint16_t ptr = bus.read_u16(operand_pc);
    std::uint16_t hi_addr =
        static_cast<std::uint16_t>((ptr & 0xFF00) | ((ptr + 1) & 0x00FF));
    return make_u16(bus.read_u8(ptr), bus.read_u8(hi_addr));
  }

  case Mode::Indirect_X: {
    std::uint8_t ptr =
        static_cast<std::uint8_t>(bus.read_u8(operand_pc) + register_x);
    return make_u16(bus.read_u8(ptr),
                    bus.read_u8(static_cast<std::uint8_t>(ptr + 1)));
  }

  case Mode::Indirect_Y: {
    std::uint8_t ptr = bus.read_u8(operand_pc);
    std::uint16_t base = make_u16(
        bus.read_u8(ptr), bus.read_u8(static_cast<std::uint8_t>(ptr + 1)));
    return static_cast<std::uint16_t>(base + register_y);
  }

  case Mode::Implied:
  case Mode::Accumulator:
  default:
    throw std::runtime_error("Addressing mode has no operand address");
  }
}

// Stack and flag helpers

void CPU::stack_push(std::uint8_t value) {
  bus.write_u8(static_cast<std::uint16_t>(STACK_BASE + stack_pointer), value);
  stack_pointer--;
}

std::uint8_t CPU::stack_pop() {
  stack_pointer++;
  return bus.read_u8(static_cast<std::uint16_t>(STACK_BASE + stack_pointer));
}

void CPU::stack_push_u16(std::uint16_t value) {
  stack_push(static_cast<std::uint8_t>(value >> 8));
  stack_push(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint16_t CPU::stack_pop_u16() {
  std::uint8_t lo = stack_pop();
  std::uint8_t hi = stack_pop();
  return make_u16(lo, hi);
}

void CPU::set_flag(std::uint8_t flag, bool on) {
  status = on ? (status | flag) : (status & ~flag);
}

bool CPU::get_flag(std::uint8_t flag) const { return (status & flag) != 0; }

void CPU::update_zero_and_negative_flags(std::uint8_t result) {
  set_flag(Flag::Zero, result == 0);
  set_flag(Flag::Negative, result & 0x80);
}

// Shared instruction logic

void CPU::branch(bool condition) {
  if (condition) {
    auto offset = static_cast<std::int8_t>(bus.read_u8(operand_pc));
    program_counter = static_cast<std::uint16_t>(program_counter + offset);
  }
}

void CPU::compare(std::uint8_t reg, std::uint8_t value) {
  set_flag(Flag::Carry, reg >= value);
  update_zero_and_negative_flags(static_cast<std::uint8_t>(reg - value));
}

void CPU::add_to_a(std::uint8_t value) {
  std::uint16_t sum = register_a + value + (status & Flag::Carry);
  auto result = static_cast<std::uint8_t>(sum);
  set_flag(Flag::Carry, sum > 0xFF);
  set_flag(Flag::Overflow, (register_a ^ result) & (value ^ result) & 0x80);
  register_a = result;
  update_zero_and_negative_flags(register_a);
}

template <typename Op> void CPU::modify(Mode mode, Op op) {
  if (mode == Mode::Accumulator) {
    register_a = op(register_a);
    update_zero_and_negative_flags(register_a);
    return;
  }
  std::uint16_t addr = get_operand_address(mode);
  std::uint8_t value = op(bus.read_u8(addr));
  bus.write_u8(addr, value);
  update_zero_and_negative_flags(value);
}

// Loads, stores, transfers

void CPU::lda(Mode mode) {
  register_a = bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_a);
}

void CPU::ldx(Mode mode) {
  register_x = bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_x);
}

void CPU::ldy(Mode mode) {
  register_y = bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_y);
}

void CPU::sta(Mode mode) { bus.write_u8(get_operand_address(mode), register_a); }
void CPU::stx(Mode mode) { bus.write_u8(get_operand_address(mode), register_x); }
void CPU::sty(Mode mode) { bus.write_u8(get_operand_address(mode), register_y); }

void CPU::tax(Mode) {
  register_x = register_a;
  update_zero_and_negative_flags(register_x);
}

void CPU::tay(Mode) {
  register_y = register_a;
  update_zero_and_negative_flags(register_y);
}

void CPU::txa(Mode) {
  register_a = register_x;
  update_zero_and_negative_flags(register_a);
}

void CPU::tya(Mode) {
  register_a = register_y;
  update_zero_and_negative_flags(register_a);
}

void CPU::tsx(Mode) {
  register_x = stack_pointer;
  update_zero_and_negative_flags(register_x);
}

void CPU::txs(Mode) { stack_pointer = register_x; }

// Stack

void CPU::pha(Mode) { stack_push(register_a); }

void CPU::pla(Mode) {
  register_a = stack_pop();
  update_zero_and_negative_flags(register_a);
}

void CPU::php(Mode) { stack_push(status | Flag::Break | Flag::Unused); }

void CPU::plp(Mode) {
  status = (stack_pop() & ~Flag::Break) | Flag::Unused;
}

// Arithmetic and logic

void CPU::adc(Mode mode) { add_to_a(bus.read_u8(get_operand_address(mode))); }

void CPU::sbc(Mode mode) {
  add_to_a(static_cast<std::uint8_t>(~bus.read_u8(get_operand_address(mode))));
}

void CPU::and_op(Mode mode) {
  register_a &= bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_a);
}

void CPU::eor(Mode mode) {
  register_a ^= bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_a);
}

void CPU::ora(Mode mode) {
  register_a |= bus.read_u8(get_operand_address(mode));
  update_zero_and_negative_flags(register_a);
}

void CPU::cmp(Mode mode) {
  compare(register_a, bus.read_u8(get_operand_address(mode)));
}

void CPU::cpx(Mode mode) {
  compare(register_x, bus.read_u8(get_operand_address(mode)));
}

void CPU::cpy(Mode mode) {
  compare(register_y, bus.read_u8(get_operand_address(mode)));
}

void CPU::bit(Mode mode) {
  std::uint8_t value = bus.read_u8(get_operand_address(mode));
  set_flag(Flag::Zero, (register_a & value) == 0);
  set_flag(Flag::Overflow, value & Flag::Overflow);
  set_flag(Flag::Negative, value & Flag::Negative);
}

// Increments, decrements, shifts, rotates

void CPU::inc(Mode mode) {
  modify(mode, [](std::uint8_t v) { return static_cast<std::uint8_t>(v + 1); });
}

void CPU::dec(Mode mode) {
  modify(mode, [](std::uint8_t v) { return static_cast<std::uint8_t>(v - 1); });
}

void CPU::inx(Mode) {
  register_x++;
  update_zero_and_negative_flags(register_x);
}

void CPU::iny(Mode) {
  register_y++;
  update_zero_and_negative_flags(register_y);
}

void CPU::dex(Mode) {
  register_x--;
  update_zero_and_negative_flags(register_x);
}

void CPU::dey(Mode) {
  register_y--;
  update_zero_and_negative_flags(register_y);
}

void CPU::asl(Mode mode) {
  modify(mode, [this](std::uint8_t v) {
    set_flag(Flag::Carry, v & 0x80);
    return static_cast<std::uint8_t>(v << 1);
  });
}

void CPU::lsr(Mode mode) {
  modify(mode, [this](std::uint8_t v) {
    set_flag(Flag::Carry, v & 0x01);
    return static_cast<std::uint8_t>(v >> 1);
  });
}

void CPU::rol(Mode mode) {
  modify(mode, [this](std::uint8_t v) {
    std::uint8_t carry_in = status & Flag::Carry;
    set_flag(Flag::Carry, v & 0x80);
    return static_cast<std::uint8_t>((v << 1) | carry_in);
  });
}

void CPU::ror(Mode mode) {
  modify(mode, [this](std::uint8_t v) {
    std::uint8_t carry_in = status & Flag::Carry;
    set_flag(Flag::Carry, v & 0x01);
    return static_cast<std::uint8_t>((v >> 1) | (carry_in << 7));
  });
}

// Jumps, subroutines, branches

void CPU::jmp(Mode mode) { program_counter = get_operand_address(mode); }

void CPU::jsr(Mode mode) {
  stack_push_u16(static_cast<std::uint16_t>(program_counter - 1));
  program_counter = get_operand_address(mode);
}

void CPU::rts(Mode) {
  program_counter = static_cast<std::uint16_t>(stack_pop_u16() + 1);
}

void CPU::rti(Mode) {
  plp();
  program_counter = stack_pop_u16();
}

void CPU::bcc(Mode) { branch(!get_flag(Flag::Carry)); }
void CPU::bcs(Mode) { branch(get_flag(Flag::Carry)); }
void CPU::beq(Mode) { branch(get_flag(Flag::Zero)); }
void CPU::bne(Mode) { branch(!get_flag(Flag::Zero)); }
void CPU::bmi(Mode) { branch(get_flag(Flag::Negative)); }
void CPU::bpl(Mode) { branch(!get_flag(Flag::Negative)); }
void CPU::bvc(Mode) { branch(!get_flag(Flag::Overflow)); }
void CPU::bvs(Mode) { branch(get_flag(Flag::Overflow)); }

// Flags and system

void CPU::clc(Mode) { set_flag(Flag::Carry, false); }
void CPU::cld(Mode) { set_flag(Flag::Decimal, false); }
void CPU::cli(Mode) { set_flag(Flag::InterruptDisable, false); }
void CPU::clv(Mode) { set_flag(Flag::Overflow, false); }
void CPU::sec(Mode) { set_flag(Flag::Carry, true); }
void CPU::sed(Mode) { set_flag(Flag::Decimal, true); }
void CPU::sei(Mode) { set_flag(Flag::InterruptDisable, true); }

// TODO: real BRK pushes PC+2 and status, sets I, and jumps via $FFFE.
// For now it just stops `run()`, which the tests rely on.
void CPU::brk(Mode) { halted = true; }

void CPU::nop(Mode) {}
