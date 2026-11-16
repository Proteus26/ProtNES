#ifndef CPU_HPP
#define CPU_HPP

#include "bus/bus.hpp"
#include <cstdint>
#include <functional>
#include <vector>

enum class AddressingMode {
  Implied,
  Accumulator,
  Immediate,
  Relative,
  ZeroPage,
  ZeroPage_X,
  ZeroPage_Y,
  Absolute,
  Absolute_X,
  Absolute_Y,
  Indirect,
  Indirect_X,
  Indirect_Y,
};

// Status register bits
struct Flag {
  static constexpr std::uint8_t Carry = 1 << 0;
  static constexpr std::uint8_t Zero = 1 << 1;
  static constexpr std::uint8_t InterruptDisable = 1 << 2;
  static constexpr std::uint8_t Decimal = 1 << 3;
  static constexpr std::uint8_t Break = 1 << 4;
  static constexpr std::uint8_t Unused = 1 << 5;
  static constexpr std::uint8_t Overflow = 1 << 6;
  static constexpr std::uint8_t Negative = 1 << 7;
};

struct CPU {
  std::uint8_t register_a;
  std::uint8_t register_x;
  std::uint8_t register_y;
  std::uint8_t status;
  std::uint16_t program_counter;
  std::uint8_t stack_pointer;
  Bus &bus;

  static constexpr std::uint16_t STACK_BASE = 0x0100;
  static constexpr std::uint8_t STACK_RESET = 0xFD;

  explicit CPU(Bus &bus);

  void load(const std::vector<std::uint8_t> &program);
  void load_and_run(const std::vector<std::uint8_t> &program);

  void reset();

  void run(const std::function<void(CPU &)> &trace_callback = nullptr);

  void step();

  std::uint16_t get_operand_address(AddressingMode mode) const;

  void stack_push(std::uint8_t value);
  std::uint8_t stack_pop();
  void stack_push_u16(std::uint16_t value);
  std::uint16_t stack_pop_u16();

  void set_flag(std::uint8_t flag, bool on);
  bool get_flag(std::uint8_t flag) const;
  void update_zero_and_negative_flags(std::uint8_t result);

  // Instruction handlers
  using Mode = AddressingMode;
  static constexpr Mode IMPLIED = Mode::Implied;

  // Loads, stores, transfers
  void lda(Mode mode);
  void ldx(Mode mode);
  void ldy(Mode mode);
  void sta(Mode mode);
  void stx(Mode mode);
  void sty(Mode mode);
  void tax(Mode mode = IMPLIED);
  void tay(Mode mode = IMPLIED);
  void txa(Mode mode = IMPLIED);
  void tya(Mode mode = IMPLIED);
  void tsx(Mode mode = IMPLIED);
  void txs(Mode mode = IMPLIED);

  // Stack
  void pha(Mode mode = IMPLIED);
  void pla(Mode mode = IMPLIED);
  void php(Mode mode = IMPLIED);
  void plp(Mode mode = IMPLIED);

  // Arithmetic and logic
  void adc(Mode mode);
  void sbc(Mode mode);
  void and_op(Mode mode);
  void eor(Mode mode);
  void ora(Mode mode);
  void cmp(Mode mode);
  void cpx(Mode mode);
  void cpy(Mode mode);
  void bit(Mode mode);

  // Increments and decrements
  void inc(Mode mode);
  void dec(Mode mode);
  void inx(Mode mode = IMPLIED);
  void iny(Mode mode = IMPLIED);
  void dex(Mode mode = IMPLIED);
  void dey(Mode mode = IMPLIED);

  // Shifts and rotates
  void asl(Mode mode);
  void lsr(Mode mode);
  void rol(Mode mode);
  void ror(Mode mode);

  // Jumps and subroutines
  void jmp(Mode mode);
  void jsr(Mode mode);
  void rts(Mode mode = IMPLIED);
  void rti(Mode mode = IMPLIED);

  // Branches
  void bcc(Mode mode);
  void bcs(Mode mode);
  void beq(Mode mode);
  void bne(Mode mode);
  void bmi(Mode mode);
  void bpl(Mode mode);
  void bvc(Mode mode);
  void bvs(Mode mode);

  // Flag operations
  void clc(Mode mode = IMPLIED);
  void cld(Mode mode = IMPLIED);
  void cli(Mode mode = IMPLIED);
  void clv(Mode mode = IMPLIED);
  void sec(Mode mode = IMPLIED);
  void sed(Mode mode = IMPLIED);
  void sei(Mode mode = IMPLIED);

  // System
  void brk(Mode mode = IMPLIED);
  void nop(Mode mode = IMPLIED);

private:
  std::uint16_t operand_pc = 0;
  bool halted = false;

  void branch(bool condition);
  void compare(std::uint8_t reg, std::uint8_t value);
  void add_to_a(std::uint8_t value);

  template <typename Op> void modify(Mode mode, Op op);
};

#endif // CPU_HPP
