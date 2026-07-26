#ifndef CPU_HPP
#define CPU_HPP

#include "bus/bus.hpp"
#include <cstdint>
#include <functional>
#include <vector>

enum class AddressingMode {
  Immediate,
  ZeroPage,
  ZeroPage_X,
  ZeroPage_Y,
  Absolute,
  Absolute_X,
  Absolute_Y,
  Indirect_X,
  Indirect_Y,
  NoneAddressing,
};

struct CPU {
  uint8_t register_a;
  uint8_t register_x;
  uint8_t register_y;
  uint8_t status;
  uint16_t program_counter;
  uint8_t stack_pointer;
  Bus &bus;

  static constexpr uint16_t STACK_BASE = 0x0100;
  static constexpr uint8_t STACK_RESET = 0xFD;

  explicit CPU(Bus &bus);

  void load(const std::vector<uint8_t> &program);
  void load_and_run(const std::vector<uint8_t> &program);

  void reset();

  void run(const std::function<void(CPU &)> &trace_callback = nullptr);

  uint16_t get_operand_address(AddressingMode mode) const;

  void stack_push(uint8_t value);
  uint8_t stack_pop();
  void stack_push_u16(uint16_t value);
  uint16_t stack_pop_u16();

  void branch(bool condition);

  void lda(uint8_t value);
  void ldx(uint8_t value);
  void ldy(uint8_t value);
  void tax();
  void tay();
  void txa();
  void tya();
  void tsx();
  void txs();
  void pha();
  void pla();
  void php();
  void plp();

  void adc(uint8_t value);
  void sbc(uint8_t value);

  void inx();
  void iny();
  void dex();
  void dey();

  void asl_accumulator();
  void lsr_accumulator();
  void rol_accumulator();
  void ror_accumulator();

  void inc(uint16_t addr);
  void dec(uint16_t addr);
  void asl(uint16_t addr);
  void lsr(uint16_t addr);
  void rol(uint16_t addr);
  void ror(uint16_t addr);

  void clc();
  void cld();
  void cli();
  void clv();
  void sec();
  void sed();
  void sei();

  void and_op(uint8_t value);
  void eor(uint8_t value);
  void ora(uint8_t value);

  void cpy(uint8_t value);
  void cpx(uint8_t value);
  void cmp(uint8_t value);
  void bit(uint8_t value);

  void update_zero_and_negative_flags(uint8_t result);
};

#endif // CPU_HPP
