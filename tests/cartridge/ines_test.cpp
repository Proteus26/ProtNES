#include "cartridge/cartridge.hpp"
#include "test_rom.hpp"
#include <gtest/gtest.h>

TEST(InesTest, ParsesSizesAndFlags) {
  TestRom rom{.prg_banks = 2, .chr_banks = 1, .flags6 = 0b0000'0011};
  RomHeader header = parse_ines_header(rom.build());

  EXPECT_EQ(header.prg_banks, 2);
  EXPECT_EQ(header.chr_banks, 1);
  EXPECT_EQ(header.mapper, 0);
  EXPECT_EQ(header.mirroring, Mirroring::Vertical);
  EXPECT_TRUE(header.has_battery);
  EXPECT_FALSE(header.has_trainer);
  EXPECT_FALSE(header.is_nes2);
}

TEST(InesTest, HorizontalMirroring) {
  TestRom rom{.flags6 = 0};
  EXPECT_EQ(parse_ines_header(rom.build()).mirroring, Mirroring::Horizontal);
}

TEST(InesTest, FourScreenOverridesMirroringBit) {
  TestRom rom{.flags6 = 0b0000'1001};
  EXPECT_EQ(parse_ines_header(rom.build()).mirroring, Mirroring::FourScreen);
}

TEST(InesTest, MapperCombinesBothNibbles) {
  TestRom rom{.flags6 = 0x40, .flags7 = 0x10};
  EXPECT_EQ(parse_ines_header(rom.build()).mapper, 0x14);
}

TEST(InesTest, JunkInHeaderTailIgnoresMapperHighNibble) {
  TestRom rom{.flags6 = 0x10, .flags7 = 0x40};
  rom.header_tail = {0, 0, 0, 0, 'D', 'u', 'd', 'e'};
  EXPECT_EQ(parse_ines_header(rom.build()).mapper, 0x01);
}

TEST(InesTest, DetectsNes2) {
  TestRom rom{.flags7 = 0b0000'1000};
  EXPECT_TRUE(parse_ines_header(rom.build()).is_nes2);
}

TEST(InesTest, RejectsBadMagic) {
  auto bytes = TestRom{}.build();
  bytes[3] = 0x00;
  EXPECT_THROW((void)parse_ines_header(bytes), RomError);
}

TEST(InesTest, RejectsShortHeader) {
  std::vector<std::uint8_t> bytes = {'N', 'E', 'S', 0x1A, 1};
  EXPECT_THROW((void)parse_ines_header(bytes), RomError);
}

TEST(InesTest, RejectsZeroPrgBanks) {
  TestRom rom{.prg_banks = 0};
  EXPECT_THROW((void)parse_ines_header(rom.build()), RomError);
}

TEST(InesTest, RejectsTruncatedFile) {
  auto bytes = TestRom{.prg_banks = 2}.build();
  bytes.resize(bytes.size() - 1);
  EXPECT_THROW((void)Cartridge::from_ines(bytes), RomError);
}

TEST(InesTest, RejectsUnsupportedMapper) {
  TestRom rom{.flags6 = 0x40};
  try {
    (void)Cartridge::from_ines(rom.build());
    FAIL() << "expected RomError";
  } catch (const RomError &e) {
    EXPECT_STREQ(e.what(), "Mapper 4 is not supported");
  }
}

TEST(InesTest, SkipsTrainer) {
  TestRom rom{.flags6 = 0b0000'0100};
  Cartridge cart = Cartridge::from_ines(rom.build());
  EXPECT_TRUE(cart.header().has_trainer);
  EXPECT_EQ(cart.cpu_read(0x8000), 0xA0);
}

TEST(InesTest, LoadsChrRom) {
  Cartridge cart = Cartridge::from_ines(TestRom{}.build());
  EXPECT_EQ(cart.ppu_read(0x0000), 0xC0);
  EXPECT_EQ(cart.ppu_read(0x1FFF), 0xC0);
}

TEST(InesTest, MissingFileThrows) {
  EXPECT_THROW((void)Cartridge::from_file("/nonexistent/rom.nes"), RomError);
}
