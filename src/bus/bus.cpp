#include "bus/bus.hpp"
#include <cstdint>

Bus::Bus() = default;

std::uint8_t Bus::read_u8(std::uint16_t addr) const {
  if (addr <= RAM_MIRRORS_END) {
    std::uint16_t mirrored = addr & 0x07FF;
    return cpu_vram[mirrored];
  }

  if (addr >= PPU_REGISTERS_START && addr <= PPU_REGISTERS_MIRRORS_END) {
    // add ppu bit here
    return 0;
  }

  if (addr >= APU_IO_START && addr <= APU_IO_END) {
    // add apu bit here
    return 0;
  }

  if (addr >= PRG_ROM_START) {
    return prg_rom[addr - PRG_ROM_START];
  }

  return 0;
}

void Bus::write_u8(std::uint16_t addr, std::uint8_t data) {
  if (addr <= RAM_MIRRORS_END) {
    std::uint16_t mirrored = addr & 0x07FF;
    cpu_vram[mirrored] = data;
    return;
  }

  if (addr >= PPU_REGISTERS_START && addr <= PPU_REGISTERS_MIRRORS_END) {
    // add ppu bit here
    return;
  }

  if (addr >= APU_IO_START && addr <= APU_IO_END) {
    // add apu bit here
    return;
  }

  if (addr >= PRG_ROM_START) {
    prg_rom[addr - PRG_ROM_START] = data;
    return;
  }
}

std::uint16_t Bus::read_u16(std::uint16_t addr) const {
  std::uint16_t lo = read_u8(addr);
  std::uint16_t hi = read_u8(static_cast<std::uint16_t>(addr + 1));
  return static_cast<std::uint16_t>(lo | (hi << 8));
}

void Bus::write_u16(std::uint16_t addr, std::uint16_t data) {
  write_u8(addr, static_cast<std::uint8_t>(data & 0xFF));
  write_u8(static_cast<std::uint16_t>(addr + 1),
           static_cast<std::uint8_t>(data >> 8));
}

void Bus::load_program(const std::vector<std::uint8_t> &program) {
  for (std::size_t i = 0; i < program.size(); ++i) {
    write_u8(static_cast<std::uint16_t>(PRG_ROM_START + i), program[i]);
  }
  write_u16(0xFFFC, PRG_ROM_START);
}
