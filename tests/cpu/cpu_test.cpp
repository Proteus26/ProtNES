#include "bus/bus.hpp"
#include "cpu/cpu.hpp"
#include <gtest/gtest.h>

TEST(CpuTest, Lda0xA9ImmediateLoadData) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa9, 0x05, 0x00});
  EXPECT_EQ(cpu.register_a, 5);
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Lda0xA9ZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa9, 0x00, 0x00});
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0b10);
}

TEST(CpuTest, Lda0xA9NegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa9, 0xff, 0x00});
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0b1000'0000);
}

TEST(CpuTest, Tax0xAAMoveAToX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xaa, 0x00});
  cpu.reset();
  cpu.register_a = 10;
  cpu.run();
  EXPECT_EQ(cpu.register_x, 10);
}

TEST(CpuTest, Test5OpsWorkingTogether) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa9, 0xc0, 0xaa, 0xe8, 0x00});
  EXPECT_EQ(cpu.register_x, 0xc1);
}

TEST(CpuTest, InxOverflow) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xe8, 0xe8, 0x00});
  cpu.reset();
  cpu.register_x = 0xff;
  cpu.run();
  EXPECT_EQ(cpu.register_x, 1);
}

TEST(CpuTest, Cpy0xC0Equal) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xc0, 0x10, 0x00});
  cpu.reset();
  cpu.register_y = 0x10;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) != 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) != 0);
}

TEST(CpuTest, Cpy0xC0LessThan) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xc0, 0x10, 0x00});
  cpu.reset();
  cpu.register_y = 0x05;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) == 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) != 0);
}

TEST(CpuTest, Cpy0xC0GreaterThan) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xc0, 0x10, 0x00});
  cpu.reset();
  cpu.register_y = 0x20;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) != 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Cpx0xE0Equal) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xe0, 0x42, 0x00});
  cpu.reset();
  cpu.register_x = 0x42;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) != 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) != 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Cpx0xE0LessThan) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xe0, 0x20, 0x00});
  cpu.reset();
  cpu.register_x = 0x10;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) == 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) != 0);
}

TEST(CpuTest, Cpx0xE0GreaterThan) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xe0, 0x10, 0x00});
  cpu.reset();
  cpu.register_x = 0x30;
  cpu.run();
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b0000'0001) != 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Ldx0xA2Immediate) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa2, 0x0F, 0x00});
  EXPECT_EQ(cpu.register_x, 0x0F);
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Ldx0xA2ZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa2, 0x00, 0x00});
  EXPECT_EQ(cpu.register_x, 0x00);
  EXPECT_TRUE((cpu.status & 0b0000'0010) != 0);
}

TEST(CpuTest, Ldx0xA2NegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa2, 0xFF, 0x00});
  EXPECT_EQ(cpu.register_x, 0xFF);
  EXPECT_TRUE((cpu.status & 0b1000'0000) != 0);
}

TEST(CpuTest, Ldy0xA0Immediate) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa0, 0x42, 0x00});
  EXPECT_EQ(cpu.register_y, 0x42);
  EXPECT_TRUE((cpu.status & 0b0000'0010) == 0);
  EXPECT_TRUE((cpu.status & 0b1000'0000) == 0);
}

TEST(CpuTest, Ldy0xA0ZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa0, 0x00, 0x00});
  EXPECT_EQ(cpu.register_y, 0x00);
  EXPECT_TRUE((cpu.status & 0b0000'0010) != 0);
}

TEST(CpuTest, Ldy0xA0NegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa0, 0x80, 0x00});
  EXPECT_EQ(cpu.register_y, 0x80);
  EXPECT_TRUE((cpu.status & 0b1000'0000) != 0);
}

TEST(CpuTest, InyBasicIncrement) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x10;
  cpu.iny();
  EXPECT_EQ(cpu.register_y, 0x11);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, InyOverflowWrapsToZero) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0xFF;
  cpu.iny();
  EXPECT_EQ(cpu.register_y, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, InySetsNegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x7F;
  cpu.iny();
  EXPECT_EQ(cpu.register_y, 0x80);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TyaTransfer) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x42;
  cpu.register_a = 0x00;
  cpu.tya();
  EXPECT_EQ(cpu.register_a, 0x42);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TyaSetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x00;
  cpu.register_a = 0xFF;
  cpu.tya();
  EXPECT_EQ(cpu.register_a, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TyaSetsNegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x80;
  cpu.register_a = 0x00;
  cpu.tya();
  EXPECT_EQ(cpu.register_a, 0x80);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TxaTransfer) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x42;
  cpu.register_a = 0x00;
  cpu.txa();
  EXPECT_EQ(cpu.register_a, 0x42);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TxaSetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x00;
  cpu.register_a = 0xFF;
  cpu.txa();
  EXPECT_EQ(cpu.register_a, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TxaSetsNegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x80;
  cpu.register_a = 0x00;
  cpu.txa();
  EXPECT_EQ(cpu.register_a, 0x80);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DexBasicDecrement) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x10;
  cpu.dex();
  EXPECT_EQ(cpu.register_x, 0x0F);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DexUnderflowWrapsToFf) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x00;
  cpu.dex();
  EXPECT_EQ(cpu.register_x, 0xFF);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DexSetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_x = 0x01;
  cpu.dex();
  EXPECT_EQ(cpu.register_x, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TayTransfer) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_a = 0x37;
  cpu.register_y = 0x00;
  cpu.tay();
  EXPECT_EQ(cpu.register_y, 0x37);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TaySetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_a = 0x00;
  cpu.register_y = 0xFF;
  cpu.tay();
  EXPECT_EQ(cpu.register_y, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, TaySetsNegativeFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_a = 0x80;
  cpu.register_y = 0x00;
  cpu.tay();
  EXPECT_EQ(cpu.register_y, 0x80);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DeyBasicDecrement) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x10;
  cpu.dey();
  EXPECT_EQ(cpu.register_y, 0x0F);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DeyWrapsToFf) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x00;
  cpu.dey();
  EXPECT_EQ(cpu.register_y, 0xFF);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, DeySetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.register_y = 0x01;
  cpu.dey();
  EXPECT_EQ(cpu.register_y, 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, LdaZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa5, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0x55);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x55);
}

TEST(CpuTest, LdaZeroPageX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb5, 0x10, 0x00});
  cpu.reset();
  cpu.register_x = 0x05;
  bus.write_u8(0x15, 0x77);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x77);
}

TEST(CpuTest, LdaZeroPageXWrapsWithinZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb5, 0xFF, 0x00});
  cpu.reset();
  cpu.register_x = 0x02;
  bus.write_u8(0x01, 0x99);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x99);
}

TEST(CpuTest, LdaAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xad, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0300, 0x42);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x42);
}

TEST(CpuTest, LdaAbsoluteX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xbd, 0x00, 0x03, 0x00});
  cpu.reset();
  cpu.register_x = 0x10;
  bus.write_u8(0x0310, 0x33);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x33);
}

TEST(CpuTest, LdaAbsoluteY) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb9, 0x00, 0x03, 0x00});
  cpu.reset();
  cpu.register_y = 0x20;
  bus.write_u8(0x0320, 0x11);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x11);
}

TEST(CpuTest, LdaIndirectX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa1, 0x20, 0x00});
  cpu.reset();
  cpu.register_x = 0x04;
  bus.write_u8(0x24, 0x00);
  bus.write_u8(0x25, 0x04);
  bus.write_u8(0x0400, 0x9A);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x9A);
}

TEST(CpuTest, LdaIndirectY) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb1, 0x20, 0x00});
  cpu.reset();
  cpu.register_y = 0x04;
  bus.write_u8(0x20, 0x00);
  bus.write_u8(0x21, 0x04);
  bus.write_u8(0x0404, 0x7B);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x7B);
}

TEST(CpuTest, LdxZeroPageY) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb6, 0x10, 0x00});
  cpu.reset();
  cpu.register_y = 0x02;
  bus.write_u8(0x12, 0x64);
  cpu.run();
  EXPECT_EQ(cpu.register_x, 0x64);
}

TEST(CpuTest, LdyZeroPageX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xb4, 0x10, 0x00});
  cpu.reset();
  cpu.register_x = 0x03;
  bus.write_u8(0x13, 0x21);
  cpu.run();
  EXPECT_EQ(cpu.register_y, 0x21);
}

TEST(CpuTest, StaZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x42, 0x85, 0x10, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0x42);
}

TEST(CpuTest, StaAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x77, 0x8d, 0x00, 0x03, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x0300), 0x77);
}

TEST(CpuTest, StxZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x33, 0x86, 0x10, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0x33);
}

TEST(CpuTest, StyZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa0, 0x99, 0x84, 0x10, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0x99);
}

TEST(CpuTest, LdaThenStaRoundTripAbsoluteX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x05, 0xbd, 0x00, 0x04, 0x9d, 0x00, 0x05, 0x00});
  cpu.reset();
  bus.write_u8(0x0405, 0xAB);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0xAB);
  EXPECT_EQ(bus.read_u8(0x0505), 0xAB);
}

TEST(CpuTest, StackPushPopByte) {
  Bus bus;
  CPU cpu(bus);
  cpu.reset();
  uint8_t sp_before = cpu.stack_pointer;
  cpu.stack_push(0x42);
  EXPECT_EQ(cpu.stack_pointer, static_cast<uint8_t>(sp_before - 1));
  uint8_t value = cpu.stack_pop();
  EXPECT_EQ(value, 0x42);
  EXPECT_EQ(cpu.stack_pointer, sp_before);
}

TEST(CpuTest, StackPushPopU16RoundTrip) {
  Bus bus;
  CPU cpu(bus);
  cpu.reset();
  cpu.stack_push_u16(0xBEEF);
  uint16_t value = cpu.stack_pop_u16();
  EXPECT_EQ(value, 0xBEEF);
}

TEST(CpuTest, PhaPlaRoundTrip) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x77, 0x48, 0xa9, 0x00, 0x68, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x77);
}

TEST(CpuTest, PhpPlpRoundTrip) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x38, 0x08, 0x18, 0x28, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_NE(cpu.status & 0b0000'0001, 0);
}

TEST(CpuTest, TsxTransfersStackPointerNotStatus) {
  Bus bus;
  CPU cpu(bus);
  cpu.reset();
  cpu.stack_pointer = 0x80;
  cpu.tsx();
  EXPECT_EQ(cpu.register_x, 0x80);
}

TEST(CpuTest, TxsTransfersToStackPointerNotStatus) {
  Bus bus;
  CPU cpu(bus);
  cpu.reset();
  cpu.register_x = 0x33;
  cpu.txs();
  EXPECT_EQ(cpu.stack_pointer, 0x33);
}

TEST(CpuTest, JmpAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x4c, 0x05, 0x80, 0xa9, 0xff, 0xa9, 0x42, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x42);
}

TEST(CpuTest, JmpIndirect) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x6c, 0x00, 0x02, 0xa9, 0xff, 0xa9, 0x42, 0x00});
  cpu.reset();
  bus.write_u8(0x0200, 0x05);
  bus.write_u8(0x0201, 0x80);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x42);
}

TEST(CpuTest, JmpIndirectPageBoundaryBug) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x6c, 0xff, 0x02, 0x00});
  cpu.reset();
  bus.write_u8(0x02FF, 0x05);
  bus.write_u8(0x0300, 0xCD);
  bus.write_u8(0x0200, 0x80);
  cpu.run();
  EXPECT_EQ(cpu.program_counter, 0x8006);
}

TEST(CpuTest, JsrRtsRoundTrip) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x20, 0x04, 0x80, 0x00, 0xa9, 0x99, 0x60});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x99);
}

TEST(CpuTest, JsrPushesCorrectReturnAddress) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x20, 0x04, 0x80, 0x00, 0x60});
  cpu.reset();
  uint16_t start = cpu.program_counter;
  cpu.run();
  EXPECT_EQ(cpu.program_counter, static_cast<uint16_t>(start + 3 + 1));
}

TEST(CpuTest, BeqBranchesWhenZeroSet) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x00, 0xf0, 0x02, 0xa9, 0xff, 0xa9, 0x11, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x11);
}

TEST(CpuTest, BeqDoesNotBranchWhenZeroClear) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x01, 0xf0, 0x02, 0xa9, 0xff, 0xa9, 0x11, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x11);
}

TEST(CpuTest, BneSkipsWhenZeroClear) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x01, 0xd0, 0x02, 0xa9, 0xff, 0xa9, 0x22, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x22);
}

TEST(CpuTest, BccBranchesWhenCarryClear) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x18, 0x90, 0x02, 0xa9, 0xff, 0xa9, 0x33, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x33);
}

TEST(CpuTest, BcsBranchesWhenCarrySet) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x38, 0xb0, 0x02, 0xa9, 0xff, 0xa9, 0x44, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x44);
}

TEST(CpuTest, LoopUsingDexAndBne) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x03, 0xca, 0xd0, 0xfd, 0x00});
  cpu.reset();
  cpu.run();
  EXPECT_EQ(cpu.register_x, 0x00);
}

TEST(CpuTest, BitZeroPageSetsOverflowAndNegativeFromMemory) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x24, 0x01, 0x00});
  cpu.reset();
  cpu.register_a = 0xFF;
  bus.write_u8(0x01, 0xC0);
  cpu.run();
  EXPECT_EQ(cpu.status & 0b1000'0000, 0b1000'0000);
  EXPECT_EQ(cpu.status & 0b0100'0000, 0b0100'0000);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
}

TEST(CpuTest, BitZeroPageSetsZeroFlagWhenMaskedResultIsZero) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x24, 0x01, 0x00});
  cpu.reset();
  cpu.register_a = 0x0F;
  bus.write_u8(0x01, 0xF0);
  cpu.run();
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
}

TEST(CpuTest, BitAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x2c, 0x00, 0x03, 0x00});
  cpu.reset();
  cpu.register_a = 0xFF;
  bus.write_u8(0x0300, 0x40);
  cpu.run();
  EXPECT_EQ(cpu.status & 0b1000'0000, 0);
  EXPECT_EQ(cpu.status & 0b0100'0000, 0b0100'0000);
}

TEST(CpuTest, AdcZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa9, 0x10, 0x65, 0x20, 0x00});
  cpu.reset();
  bus.write_u8(0x20, 0x05);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x15);
}

TEST(CpuTest, SbcAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x38, 0xa9, 0x10, 0xed, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0300, 0x05);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x0B);
}

TEST(CpuTest, AndZeroPageX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x02, 0xa9, 0xFF, 0x35, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x12, 0x0F);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0x0F);
}

TEST(CpuTest, EorAbsoluteX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x01, 0xa9, 0xFF, 0x5d, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0301, 0x0F);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0xF0);
}

TEST(CpuTest, OraIndirectY) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa0, 0x04, 0xa9, 0x0F, 0x11, 0x20, 0x00});
  cpu.reset();
  bus.write_u8(0x20, 0x00);
  bus.write_u8(0x21, 0x04);
  bus.write_u8(0x0404, 0xF0);
  cpu.run();
  EXPECT_EQ(cpu.register_a, 0xFF);
}

TEST(CpuTest, CmpIndirectX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x04, 0xa9, 0x10, 0xc1, 0x20, 0x00});
  cpu.reset();
  bus.write_u8(0x24, 0x00);
  bus.write_u8(0x25, 0x04);
  bus.write_u8(0x0400, 0x10);
  cpu.run();
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
  EXPECT_NE(cpu.status & 0b0000'0001, 0);
}

TEST(CpuTest, CpxAbsolute) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x20, 0xec, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0300, 0x10);
  cpu.run();
  EXPECT_NE(cpu.status & 0b0000'0001, 0);
  EXPECT_EQ(cpu.status & 0b0000'0010, 0);
}

TEST(CpuTest, CpyZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa0, 0x05, 0xc4, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0x0A);
  cpu.run();
  EXPECT_EQ(cpu.status & 0b0000'0001, 0);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, RtiRestoresStatusAndProgramCounterNoOffset) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x40});
  cpu.reset();
  uint8_t original_status = 0b00000001;
  cpu.stack_push_u16(0x1234);
  cpu.stack_push(original_status | 0b00110000);
  cpu.status = 0;

  std::vector<uint16_t> pcs_seen;
  cpu.run([&](CPU &c) { pcs_seen.push_back(c.program_counter); });

  ASSERT_GE(pcs_seen.size(), 2u);
  EXPECT_EQ(pcs_seen[1], 0x1234);
  EXPECT_NE(cpu.status & 0b00000001, 0);
}

TEST(CpuTest, IncZeroPage) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xe6, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0x05);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0x06);
}

TEST(CpuTest, IncWrapsToZeroAndSetsZeroFlag) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xee, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0300, 0xFF);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x0300), 0x00);
  EXPECT_NE(cpu.status & 0b0000'0010, 0);
}

TEST(CpuTest, DecZeroPageX) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x02, 0xd6, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x12, 0x05);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x12), 0x04);
}

TEST(CpuTest, DecUnderflowsToFf) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xc6, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0x00);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0xFF);
  EXPECT_NE(cpu.status & 0b1000'0000, 0);
}

TEST(CpuTest, AslZeroPageSetsCarryFromOldBit7) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x06, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0b10000001);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0b00000010);
  EXPECT_NE(cpu.status & 0b0000'0001, 0);
}

TEST(CpuTest, LsrAbsoluteXSetsCarryFromOldBit0) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x01, 0x5e, 0x00, 0x03, 0x00});
  cpu.reset();
  bus.write_u8(0x0301, 0b00000011);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x0301), 0b00000001);
  EXPECT_NE(cpu.status & 0b0000'0001, 0);
}

TEST(CpuTest, RolZeroPageBringsInOldCarry) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0x38, 0x26, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x10, 0b01000000);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x10), 0b10000001);
  EXPECT_EQ(cpu.status & 0b0000'0001, 0);
}

TEST(CpuTest, RorZeroPageXBringsInOldCarry) {
  Bus bus;
  CPU cpu(bus);
  cpu.load({0xa2, 0x01, 0x38, 0x76, 0x10, 0x00});
  cpu.reset();
  bus.write_u8(0x11, 0b00000010);
  cpu.run();
  EXPECT_EQ(bus.read_u8(0x11), 0b10000001);
  EXPECT_EQ(cpu.status & 0b0000'0001, 0);
}
