#include "cartridge/nrom.hpp"
#include "cartridge/cartridge.hpp"

#include <format>
#include <utility>

Nrom::Nrom(std::vector<std::uint8_t> prg_rom, std::vector<std::uint8_t> chr,
           bool chr_is_ram, Mirroring mirroring)
    : prg_rom_(std::move(prg_rom)), chr_(std::move(chr)),
      prg_mask_(static_cast<std::uint16_t>(prg_rom_.size() - 1)),
      chr_is_ram_(chr_is_ram), mirroring_(mirroring) {
  if (prg_rom_.size() != 0x4000 && prg_rom_.size() != 0x8000) {
    throw RomError(std::format(
        "NROM needs 16KB or 32KB of PRG ROM, got {}KB", prg_rom_.size() / 1024));
  }
  if (chr_.size() != 0x2000) {
    throw RomError(
        std::format("NROM needs 8KB of CHR, got {}KB", chr_.size() / 1024));
  }
}

std::uint8_t Nrom::cpu_read(std::uint16_t addr) const {
  if (addr >= 0x8000) {
    return prg_rom_[addr & prg_mask_];
  }
  if (addr >= 0x6000) {
    return prg_ram_[addr - 0x6000];
  }
  return 0;
}

void Nrom::cpu_write(std::uint16_t addr, std::uint8_t value) {
  if (addr >= 0x6000 && addr < 0x8000) {
    prg_ram_[addr - 0x6000] = value;
  }
}

std::uint8_t Nrom::ppu_read(std::uint16_t addr) const {
  return chr_[addr & 0x1FFF];
}

void Nrom::ppu_write(std::uint16_t addr, std::uint8_t value) {
  if (chr_is_ram_) {
    chr_[addr & 0x1FFF] = value;
  }
}
