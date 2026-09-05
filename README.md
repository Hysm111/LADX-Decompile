# LADX-Decompile

C/C++ decompilation of **The Legend of Zelda: Link's Awakening DX** (Game Boy Color), aiming for a 1:1 behavior-accurate, readable, and verifiable modern codebase.

## Project Structure

- `include/`: Header files, hardware constants, memory map addresses, and function declarations.
  - `constants/`: Game Boy hardware registers, memory maps, gameplay enums, and graphics definitions.
  - `home/`: Headers for bank 0 / home bank routines.
- `src/`: Decompiled C source implementations.
  - `gb.c`: Game Boy hardware, memory mapping, and DMA emulation state.
  - `home/`: Implementations of bank 0 routines (`clear_memory.c`, `copy_data.c`, `bank.c`).
- `tests/`: Comprehensive unit and verification test suite validating memory alterations byte-for-byte against original assembly behavior.
- `LADX-Disassembly/`: Git submodule pointing to the source of truth disassembly ([zladx/LADX-Disassembly](https://github.com/zladx/LADX-Disassembly)).
- `DECOMPILATION_PROGRESS.md`: Detailed tracking document of decompiled functions, verification status, and technical findings.

## Building and Testing

### Prerequisites

- CMake 3.15 or newer
- C11 / C++17 compatible compiler (e.g. GCC, Clang)
- Git

### Build Instructions

```bash
# Clone with submodules
git clone --recursive git@github.com:Hysm111/LADX-Decompile.git
cd LADX-Decompile

# Configure and build
cmake -B build -S .
cmake --build build

# Run unit verification tests
./build/ladx_tests
```

## Progress and Verification

Progress is tracked function-by-function with strict verification against original assembly logic. See [DECOMPILATION_PROGRESS.md](DECOMPILATION_PROGRESS.md) for details.
