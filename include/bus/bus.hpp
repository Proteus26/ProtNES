#pragma once

#include "cartridge/cartridge.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <span>

[[nodiscard]] constexpr std::uint16_t make_u16(std::uint8_t lo, std::uint8_t hi) {
  return static_cast<std::uint16_t>(lo | (hi << 8));
}

class Bus {
public:
  [[nodiscard]] std::uint8_t read_u8(std::uint16_t addr) const;
  void write_u8(std::uint16_t addr, std::uint8_t data);

  [[nodiscard]] std::uint16_t read_u16(std::uint16_t addr) const;
  void write_u16(std::uint16_t addr, std::uint16_t data);

  void insert_cartridge(Cartridge cart);
  void load_program(std::span<const std::uint8_t> program);

  static constexpr std::uint16_t RAM_START = 0x0000;
  static constexpr std::uint16_t RAM_MIRRORS_END = 0x1FFF;
  static constexpr std::uint16_t PPU_REGISTERS_START = 0x2000;
  static constexpr std::uint16_t PPU_REGISTERS_MIRRORS_END = 0x3FFF;
  static constexpr std::uint16_t APU_IO_START = 0x4000;
  static constexpr std::uint16_t APU_IO_END = 0x401F;
  static constexpr std::uint16_t CARTRIDGE_START = 0x4020;
  static constexpr std::uint16_t PRG_ROM_START = 0x8000;
  static constexpr std::uint16_t RESET_VECTOR = 0xFFFC;

private:
  std::array<std::uint8_t, 0x0800> cpu_vram{};
  std::optional<Cartridge> cartridge;
};
