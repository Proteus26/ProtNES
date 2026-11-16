#include "bus/bus.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>

std::uint8_t Bus::read_u8(std::uint16_t addr) const {
  if (addr <= RAM_MIRRORS_END) {
    return cpu_vram[addr & 0x07FF];
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
    cpu_vram[addr & 0x07FF] = data;
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
  }
}

std::uint16_t Bus::read_u16(std::uint16_t addr) const {
  return make_u16(read_u8(addr), read_u8(static_cast<std::uint16_t>(addr + 1)));
}

void Bus::write_u16(std::uint16_t addr, std::uint16_t data) {
  write_u8(addr, static_cast<std::uint8_t>(data & 0xFF));
  write_u8(static_cast<std::uint16_t>(addr + 1),
           static_cast<std::uint8_t>(data >> 8));
}

void Bus::load_program(std::span<const std::uint8_t> program) {
  if (program.size() > prg_rom.size()) {
    throw std::length_error(std::format("Program is {} bytes, PRG ROM holds {}",
                                        program.size(), prg_rom.size()));
  }
  std::ranges::copy(program, prg_rom.begin());
  write_u16(RESET_VECTOR, PRG_ROM_START);
}
