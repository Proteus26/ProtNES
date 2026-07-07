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
