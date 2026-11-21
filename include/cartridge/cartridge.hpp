#pragma once

#include "cartridge/mapper.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <stdexcept>

class RomError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

struct RomHeader {
  std::uint8_t prg_banks = 0; // 16KB units
  std::uint8_t chr_banks = 0; // 8KB units, 0 = CHR RAM
  std::uint16_t mapper = 0;
  Mirroring mirroring = Mirroring::Horizontal;
  bool has_battery = false;
  bool has_trainer = false;
  bool is_nes2 = false;
};

[[nodiscard]] RomHeader parse_ines_header(std::span<const std::uint8_t> file);

class Cartridge {
public:
  static constexpr std::size_t HEADER_SIZE = 16;
  static constexpr std::size_t TRAINER_SIZE = 512;
  static constexpr std::size_t PRG_BANK_SIZE = 0x4000;
  static constexpr std::size_t CHR_BANK_SIZE = 0x2000;

  [[nodiscard]] static Cartridge from_ines(std::span<const std::uint8_t> file);
  [[nodiscard]] static Cartridge from_file(const std::filesystem::path &path);
  [[nodiscard]] static Cartridge
  from_program(std::span<const std::uint8_t> program);

  [[nodiscard]] const RomHeader &header() const { return header_; }
  [[nodiscard]] std::string_view mapper_name() const { return mapper_->name(); }
  [[nodiscard]] Mirroring mirroring() const { return mapper_->mirroring(); }

  [[nodiscard]] std::uint8_t cpu_read(std::uint16_t addr) const {
    return mapper_->cpu_read(addr);
  }
  void cpu_write(std::uint16_t addr, std::uint8_t value) {
    mapper_->cpu_write(addr, value);
  }

  [[nodiscard]] std::uint8_t ppu_read(std::uint16_t addr) const {
    return mapper_->ppu_read(addr);
  }
  void ppu_write(std::uint16_t addr, std::uint8_t value) {
    mapper_->ppu_write(addr, value);
  }

private:
  Cartridge(RomHeader header, std::unique_ptr<Mapper> mapper)
      : header_(header), mapper_(std::move(mapper)) {}

  RomHeader header_;
  std::unique_ptr<Mapper> mapper_;
};
