#include "bus/bus.hpp"
#include "cartridge/cartridge.hpp"
#include "cpu/cpu.hpp"
#include "test_rom.hpp"
#include <gtest/gtest.h>

TEST(NromTest, Nrom128MirrorsPrgAtC000) {
  Cartridge cart = Cartridge::from_ines(TestRom{.prg_banks = 1}.build());
  EXPECT_EQ(cart.mapper_name(), "NROM");
  EXPECT_EQ(cart.cpu_read(0x8000), 0xA0);
  EXPECT_EQ(cart.cpu_read(0xC000), 0xA0);
  EXPECT_EQ(cart.cpu_read(0xFFFF), 0xA0);
}

TEST(NromTest, Nrom256MapsBothBanks) {
  Cartridge cart = Cartridge::from_ines(TestRom{.prg_banks = 2}.build());
  EXPECT_EQ(cart.cpu_read(0x8000), 0xA0);
  EXPECT_EQ(cart.cpu_read(0xBFFF), 0xA0);
  EXPECT_EQ(cart.cpu_read(0xC000), 0xA1);
  EXPECT_EQ(cart.cpu_read(0xFFFF), 0xA1);
}

TEST(NromTest, RejectsTooMuchPrg) {
  EXPECT_THROW((void)Cartridge::from_ines(TestRom{.prg_banks = 3}.build()),
               RomError);
}

TEST(NromTest, IgnoresWritesToPrgRom) {
  Cartridge cart = Cartridge::from_ines(TestRom{}.build());
  cart.cpu_write(0x8000, 0x42);
  EXPECT_EQ(cart.cpu_read(0x8000), 0xA0);
}

TEST(NromTest, PrgRamIsReadWrite) {
  Cartridge cart = Cartridge::from_ines(TestRom{}.build());
  cart.cpu_write(0x6000, 0x12);
  cart.cpu_write(0x7FFF, 0x34);
  EXPECT_EQ(cart.cpu_read(0x6000), 0x12);
  EXPECT_EQ(cart.cpu_read(0x7FFF), 0x34);
}

TEST(NromTest, ChrRomIsReadOnly) {
  Cartridge cart = Cartridge::from_ines(TestRom{.chr_banks = 1}.build());
  cart.ppu_write(0x0010, 0x55);
  EXPECT_EQ(cart.ppu_read(0x0010), 0xC0);
}

TEST(NromTest, ChrRamWhenNoChrBanks) {
  Cartridge cart = Cartridge::from_ines(TestRom{.chr_banks = 0}.build());
  cart.ppu_write(0x0010, 0x55);
  EXPECT_EQ(cart.ppu_read(0x0010), 0x55);
}

TEST(NromTest, KeepsMirroringFromHeader) {
  Cartridge cart = Cartridge::from_ines(TestRom{.flags6 = 0x01}.build());
  EXPECT_EQ(cart.mirroring(), Mirroring::Vertical);
}

TEST(BusCartridgeTest, ReadsPrgThroughBus) {
  Bus bus;
  bus.insert_cartridge(Cartridge::from_ines(TestRom{.prg_banks = 2}.build()));
  EXPECT_EQ(bus.read_u8(0x8000), 0xA0);
  EXPECT_EQ(bus.read_u8(0xC000), 0xA1);
}

TEST(BusCartridgeTest, PrgRamThroughBus) {
  Bus bus;
  bus.insert_cartridge(Cartridge::from_ines(TestRom{}.build()));
  bus.write_u8(0x6000, 0x99);
  EXPECT_EQ(bus.read_u8(0x6000), 0x99);
}

TEST(BusCartridgeTest, NoCartridgeReadsZero) {
  Bus bus;
  EXPECT_EQ(bus.read_u8(0x8000), 0x00);
  EXPECT_EQ(bus.read_u16(Bus::RESET_VECTOR), 0x0000);
}

TEST(BusCartridgeTest, CpuResetUsesCartridgeVector) {
  auto bytes = TestRom{.prg_banks = 1}.build();
  // Reset vector bytes at the end of the 16KB bank -> $C123
  std::size_t vector = 16 + 0x3FFC;
  bytes[vector] = 0x23;
  bytes[vector + 1] = 0xC1;

  Bus bus;
  bus.insert_cartridge(Cartridge::from_ines(bytes));
  CPU cpu(bus);
  cpu.reset();
  EXPECT_EQ(cpu.program_counter, 0xC123);
}

TEST(BusCartridgeTest, LoadProgramSetsResetVector) {
  Bus bus;
  bus.load_program(std::vector<std::uint8_t>{0xA9, 0x05, 0x00});
  EXPECT_EQ(bus.read_u16(Bus::RESET_VECTOR), 0x8000);
  EXPECT_EQ(bus.read_u8(0x8000), 0xA9);
}

TEST(BusCartridgeTest, LoadProgramRejectsOversizedProgram) {
  Bus bus;
  EXPECT_THROW(bus.load_program(std::vector<std::uint8_t>(0x8001, 0xEA)),
               RomError);
}
