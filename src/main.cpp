#include "bus/bus.hpp"
#include "cpu/cpu.hpp"
#include <iostream>

int main() {
  Bus bus;
  CPU cpu(bus);
  cpu.load_and_run({0xa9, 0x05, 0x00});
  std::cout << "register_a = " << static_cast<int>(cpu.register_a) << '\n';
  return 0;
}
