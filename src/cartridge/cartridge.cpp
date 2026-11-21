#include "cartridge/cartridge.hpp"
#include "cartridge/nrom.hpp"

#include <algorithm>
#include <format>
#include <vector>

Cartridge Cartridge::from_program(std::span<const std::uint8_t> program) {
  constexpr std::size_t PRG_SIZE = 2 * PRG_BANK_SIZE;
  if (program.size() > PRG_SIZE) {
    throw RomError(std::format("Program is {} bytes, PRG ROM holds {}",
                               program.size(), PRG_SIZE));
  }

  std::vector<std::uint8_t> prg(PRG_SIZE, 0);
  std::ranges::copy(program, prg.begin());

  // Reset vector -> $8000
  prg[0x7FFC] = 0x00;
  prg[0x7FFD] = 0x80;

  RomHeader header;
  header.prg_banks = 2;
  header.chr_banks = 0;

  auto mapper = std::make_unique<Nrom>(std::move(prg),
                                       std::vector<std::uint8_t>(CHR_BANK_SIZE),
                                       true, header.mirroring);
  return Cartridge(header, std::move(mapper));
}
