# ProtNES
NES Emulator built using ~Rust~ C++ (Switched to C++ because I just don't want to do rust anymore lol)

# Building

Requires CMake 3.24+, Ninja, and a C++20 compiler. GoogleTest is used from
the system if installed, otherwise CMake downloads it.

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

Run it (prints the ROM header; only mapper 0 / NROM is supported so far):
```sh
./build/protnes path/to/rom.nes
```

Run the tests:
```sh
ctest --test-dir build
# or directly:
./build/protnes_tests
```

Pass `-DPROTNES_BUILD_TESTS=OFF` to skip building the tests.

# To-do:
* PPU
* "Illegal" Opcodes
