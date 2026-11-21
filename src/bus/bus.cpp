#include "bus/bus.hpp"

#include <utility>

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

  if (addr >= CARTRIDGE_START && cartridge) {
    return cartridge->cpu_read(addr);
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

  if (addr >= CARTRIDGE_START && cartridge) {
    cartridge->cpu_write(addr, data);
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

void Bus::insert_cartridge(Cartridge cart) {
  cartridge = std::move(cart);
}

void Bus::load_program(std::span<const std::uint8_t> program) {
  insert_cartridge(Cartridge::from_program(program));
}
