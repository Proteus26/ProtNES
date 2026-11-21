#include "cartridge/cartridge.hpp"
#include "cartridge/nrom.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <vector>

namespace {

constexpr std::array<std::uint8_t, 4> INES_MAGIC = {'N', 'E', 'S', 0x1A};

std::unique_ptr<Mapper> make_mapper(const RomHeader &header,
                                    std::vector<std::uint8_t> prg,
                                    std::vector<std::uint8_t> chr,
                                    bool chr_is_ram) {
  switch (header.mapper) {
  case 0:
    return std::make_unique<Nrom>(std::move(prg), std::move(chr), chr_is_ram,
                                  header.mirroring);
  default:
    throw RomError(std::format("Mapper {} is not supported", header.mapper));
  }
}

} // namespace

RomHeader parse_ines_header(std::span<const std::uint8_t> file) {
  if (file.size() < Cartridge::HEADER_SIZE) {
    throw RomError(std::format("File is {} bytes, too small for an iNES header",
                               file.size()));
  }
  if (!std::ranges::equal(file.first(4), INES_MAGIC)) {
    throw RomError("Not an iNES file (missing NES<EOF> magic)");
  }

  std::uint8_t flags6 = file[6];
  std::uint8_t flags7 = file[7];

  RomHeader header;
  header.prg_banks = file[4];
  header.chr_banks = file[5];
  header.is_nes2 = (flags7 & 0x0C) == 0x08;
  header.has_battery = flags6 & 0x02;
  header.has_trainer = flags6 & 0x04;

  if (flags6 & 0x08) {
    header.mirroring = Mirroring::FourScreen;
  } else {
    header.mirroring =
        (flags6 & 0x01) ? Mirroring::Vertical : Mirroring::Horizontal;
  }

  // Old dumpers wrote junk like "DiskDude!" into bytes 7-15
  bool has_junk = !header.is_nes2 &&
                  std::ranges::any_of(file.subspan(12, 4),
                                      [](std::uint8_t b) { return b != 0; });
  std::uint8_t mapper_lo = flags6 >> 4;
  std::uint8_t mapper_hi = has_junk ? 0 : (flags7 & 0xF0);
  header.mapper = mapper_hi | mapper_lo;

  if (header.prg_banks == 0) {
    throw RomError("Header declares 0 PRG ROM banks");
  }

  return header;
}

Cartridge Cartridge::from_ines(std::span<const std::uint8_t> file) {
  RomHeader header = parse_ines_header(file);

  std::size_t prg_offset =
      HEADER_SIZE + (header.has_trainer ? TRAINER_SIZE : 0);
  std::size_t prg_size = header.prg_banks * PRG_BANK_SIZE;
  std::size_t chr_size = header.chr_banks * CHR_BANK_SIZE;
  std::size_t expected = prg_offset + prg_size + chr_size;

  if (file.size() < expected) {
    throw RomError(std::format("File is {} bytes, header expects at least {}",
                               file.size(), expected));
  }

  auto prg_data = file.subspan(prg_offset, prg_size);
  std::vector<std::uint8_t> prg(prg_data.begin(), prg_data.end());

  bool chr_is_ram = header.chr_banks == 0;
  std::vector<std::uint8_t> chr;
  if (chr_is_ram) {
    chr.resize(CHR_BANK_SIZE);
  } else {
    auto chr_data = file.subspan(prg_offset + prg_size, chr_size);
    chr.assign(chr_data.begin(), chr_data.end());
  }

  auto mapper = make_mapper(header, std::move(prg), std::move(chr), chr_is_ram);
  return Cartridge(header, std::move(mapper));
}

Cartridge Cartridge::from_file(const std::filesystem::path &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    throw RomError(std::format("Could not open {}", path.string()));
  }
  std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)),
                                  std::istreambuf_iterator<char>());
  return from_ines(bytes);
}
