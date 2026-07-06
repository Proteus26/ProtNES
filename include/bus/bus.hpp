#ifndef BUS_HPP
#define BUS_HPP

#include <array>
#include <cstdint>
#include <vector>

struct Bus {
public:
  Bus();

  std::uint8_t read_u8(std::uint16_t addr) const;
  void write_u8(std::uint16_t addr, std::uint8_t data);

  std::uint16_t read_u16(std::uint16_t addr) const;
  void write_u16(std::uint16_t addr, std::uint16_t data);

  void load_program(const std::vector<std::uint8_t> &program);

  static constexpr std::uint16_t RAM_START = 0x0000;
  static constexpr std::uint16_t RAM_MIRRORS_END = 0x1FFF;
  static constexpr std::uint16_t PPU_REGISTERS_START = 0x2000;
  static constexpr std::uint16_t PPU_REGISTERS_MIRRORS_END = 0x3FFF;
  static constexpr std::uint16_t APU_IO_START = 0x4000;
  static constexpr std::uint16_t APU_IO_END = 0x401F;
  static constexpr std::uint16_t PRG_ROM_START = 0x8000;

private:
  std::array<std::uint8_t, 2048> cpu_vram{};
  std::array<std::uint8_t, 0x8000> prg_rom{};
};

#endif // BUS_HPP
