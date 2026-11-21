#pragma once

#include <cstdint>
#include <string_view>

enum class Mirroring : std::uint8_t { Horizontal, Vertical, FourScreen };

[[nodiscard]] constexpr std::string_view to_string(Mirroring mirroring) {
  switch (mirroring) {
  case Mirroring::Horizontal:
    return "Horizontal";
  case Mirroring::Vertical:
    return "Vertical";
  case Mirroring::FourScreen:
    return "Four-screen";
  }
  return "Unknown";
}

class Mapper {
public:
  virtual ~Mapper() = default;

  // CPU $4020-$FFFF
  [[nodiscard]] virtual std::uint8_t cpu_read(std::uint16_t addr) const = 0;
  virtual void cpu_write(std::uint16_t addr, std::uint8_t value) = 0;

  // PPU $0000-$1FFF
  [[nodiscard]] virtual std::uint8_t ppu_read(std::uint16_t addr) const = 0;
  virtual void ppu_write(std::uint16_t addr, std::uint8_t value) = 0;

  [[nodiscard]] virtual Mirroring mirroring() const = 0;
  [[nodiscard]] virtual std::string_view name() const = 0;
};
