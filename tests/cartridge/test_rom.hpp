#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

struct TestRom {
  std::uint8_t prg_banks = 1;
  std::uint8_t chr_banks = 1;
  std::uint8_t flags6 = 0;
  std::uint8_t flags7 = 0;
  std::vector<std::uint8_t> header_tail = std::vector<std::uint8_t>(8, 0);

  // Fills each PRG bank with its index + 0xA0 and each CHR bank with index + 0xC0
  [[nodiscard]] std::vector<std::uint8_t> build() const {
    std::vector<std::uint8_t> rom = {'N', 'E', 'S', 0x1A, prg_banks, chr_banks,
                                     flags6, flags7};
    rom.insert(rom.end(), header_tail.begin(), header_tail.end());

    if (flags6 & 0x04) {
      rom.insert(rom.end(), 512, 0xEE);
    }
    for (std::size_t bank = 0; bank < prg_banks; ++bank) {
      rom.insert(rom.end(), 0x4000, static_cast<std::uint8_t>(0xA0 + bank));
    }
    for (std::size_t bank = 0; bank < chr_banks; ++bank) {
      rom.insert(rom.end(), 0x2000, static_cast<std::uint8_t>(0xC0 + bank));
    }
    return rom;
  }
};
