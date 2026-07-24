# ProtNES
NES Emulator built using ~Rust~ C++ (Switched to C++ because I just don't want to do rust anymore lol)

# Building

Uses CMake + Ninja. If you're on the Nix flake dev shell (`nix develop`), everything below is already on `PATH`.

```sh
mkdir -p build && cd build
cmake -G Ninja ..
ninja
```

Run it:
```sh
./protnes
```

Run the tests:
```sh
ctest
# or directly:
./protnes_tests
```

Rebuilding after changes is just `ninja` again from `build/` — no need to
re-run `cmake` unless you add/remove source files or touch `CMakeLists.txt`.

# To-do:
* Make the tests use the new bus since it doesn't work now
* Add Memory Addressing Modes
* Add Stack Functionality to opcodes (empty as of right now, need functionality, tsx and txs need to change functionality)
* Verify adc and sbc functionality
