#pragma once

#include "cartridge/mapper.hpp"
#include <array>
#include <cstdint>
#include <vector>

// Mapper 0
class Nrom final : public Mapper {
public:
  Nrom(std::vector<std::uint8_t> prg_rom, std::vector<std::uint8_t> chr,
       bool chr_is_ram, Mirroring mirroring);

  [[nodiscard]] std::uint8_t cpu_read(std::uint16_t addr) const override;
  void cpu_write(std::uint16_t addr, std::uint8_t value) override;

  [[nodiscard]] std::uint8_t ppu_read(std::uint16_t addr) const override;
  void ppu_write(std::uint16_t addr, std::uint8_t value) override;

  [[nodiscard]] Mirroring mirroring() const override { return mirroring_; }
  [[nodiscard]] std::string_view name() const override { return "NROM"; }

private:
  std::vector<std::uint8_t> prg_rom_;
  std::vector<std::uint8_t> chr_;
  std::array<std::uint8_t, 0x2000> prg_ram_{};
  std::uint16_t prg_mask_;
  bool chr_is_ram_;
  Mirroring mirroring_;
};
