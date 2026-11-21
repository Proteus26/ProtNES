#include "bus/bus.hpp"
#include "cartridge/cartridge.hpp"
#include "cpu/cpu.hpp"

#include <format>
#include <iostream>
#include <string>
#include <utility>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << std::format("usage: {} <rom.nes>\n", argv[0]);
    return 1;
  }

  try {
    Cartridge cart = Cartridge::from_file(argv[1]);
    const RomHeader &header = cart.header();

    std::string chr = header.chr_banks == 0
                          ? std::string("8KB RAM")
                          : std::format("{}KB", header.chr_banks * 8);
    std::cout << std::format("PRG: {}KB  CHR: {}  Mapper: {} ({})  Mirroring: {}\n",
                             header.prg_banks * 16, chr, header.mapper,
                             cart.mapper_name(), to_string(header.mirroring));

    Bus bus;
    bus.insert_cartridge(std::move(cart));
    CPU cpu(bus);
    cpu.reset();
    std::cout << std::format("Reset vector: ${:04X}\n", cpu.program_counter);
  } catch (const RomError &e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
