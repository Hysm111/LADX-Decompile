# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 2.1%
* **Number of Verified Functions**: 26
* **Number of Decompiled Functions**: 26
* **Number Remaining**: ~1200+ functions
* **Current Subsystem**: Bank 0 - BGMap & ROM Bank Transfer Trampolines (`CopyData_trampoline`, `func_BB5`, `CopyBGMapFromBank` complete)
* **Current Task**: Subsystem completed and verified
* **Last Completed Task**: Verified `CopyData_trampoline`, `func_BB5`, and `CopyBGMapFromBank`
* **Next Task**: Select next Bank 0 subsystem
* **Last Update Timestamp**: 2026-09-06T03:20:00+03:00

---

## Status Table

| Section | Status | Build | Verification | Notes |
| :--- | :--- | :--- | :--- | :--- |
| `ClearBytes` | VERIFIED | PASS | PASS | Foundational byte-clearing routine (`00:29DF`) |
| `ClearWRAMBytes` | VERIFIED | PASS | PASS | Clears BC bytes of WRAM starting from $C000 (`00:29DC`) |
| `ClearHRAMBytesAndWRAM` | VERIFIED | PASS | PASS | Clears BC bytes in HRAM ($FF90) and all WRAM ($C000-$DEFF) (`00:29D3`) |
| `ClearHRAMAndWRAM` | VERIFIED | PASS | PASS | Clears all game HRAM ($FF90-$FFFC) and WRAM, preserving `hIsGBC` (`00:29D0`) |
| `ClearWRAMAndLowerHRAM` | VERIFIED | PASS | PASS | Clears lower HRAM ($FF90-$FFBE) and WRAM (`00:29CB`) |
| `ClearLowerWRAM` | VERIFIED | PASS | PASS | Clears lower WRAM ($C000-$D2FF, 0x1300 bytes) (`00:29C6`) |
| `ClearLowerAndMiddleWRAM` | VERIFIED | PASS | PASS | Clears lower & middle WRAM ($C000-$D5FF, 0x1600 bytes) (`00:29C1`) |
| `FillBGMapBlack` | VERIFIED | PASS | PASS | Fills 0x400 bytes of vBGMap0 with DIALOG_BG_TILE_DARK ($7E) (`00:28F0`) |
| `FillBGMapWhite` | VERIFIED | PASS | PASS | Fills 0x800 bytes of vBGMap0 with $7F (`00:28F7`) |
| `FillBGMap` | VERIFIED | PASS | PASS | Fills BC bytes of vBGMap0 with value A (`00:28FC`) |
| `CopyData` | VERIFIED | PASS | PASS | Byte copy routine DE <- HL, BC bytes (`00:2914`) |
| `CopyDataFromBank` | VERIFIED | PASS | PASS | Switch bank in `rSelectROMBank`, copy BC bytes, restore bank 1 (`00:2908`) |
| `DrawCommandToVRAM` | VERIFIED | PASS | PASS | VRAM draw command dispatcher: copy/fill row/column (`00:2941`) |
| `DrawCommandToVRAMDuringRoomTransition` | VERIFIED | PASS | PASS | Specialized transition draw routine with $EE transparency (`00:2991`) |
| `ExecuteDrawCommands` | VERIFIED | PASS | PASS | Process draw command queue in WRAM (`00:2927`) |
| `AdjustBankNumberForGBC` | VERIFIED | PASS | PASS | Adjusts bank for GBC by setting bit 5 if hIsGBC != 0 (`00:0B0B`) |
| `SwitchBank` | VERIFIED | PASS | PASS | Writes bank to wCurrentBank and rSelectROMBank (`00:080C`) |
| `SwitchAdjustedBank` | VERIFIED | PASS | PASS | Adjusts bank for GBC, then switches bank (`00:0813`) |
| `ReloadSavedBank` | VERIFIED | PASS | PASS | Reloads wCurrentBank into rSelectROMBank (`00:081D`) |
| `CopyObjectsAttributesToWRAM2` | VERIFIED | PASS | PASS | Copies data from bank hMultiPurpose0 to WRAM2, restores bank $20 (`00:0B1A`) |
| `CopyToBGMap0` | VERIFIED | PASS | PASS | Copies 20x18 tiles (360 bytes) to vBGMap0 with 32-tile stride (`00:0B96`) |
| `CopyDataToVRAM_noDMA` | VERIFIED | PASS | PASS | CPU copy of 0x100 bytes to $8000+(${c}00) and restores bank (`00:0A01`) |
| `CopyDataToVRAM` | VERIFIED | PASS | PASS | Selects bank, copies 0x100 bytes using GDMA on CGB or CPU on DMG (`00:0A13`) |
| `CopyData_trampoline` | VERIFIED | PASS | PASS | Copies data from specified bank, restores bank $28 (`00:0B5D`) |
| `func_BB5` | VERIFIED | PASS | PASS | Copies $168 (360) bytes to wIsFileSelectionArrowShifted ($D000) (`00:0BB5`) |
| `CopyBGMapFromBank` | VERIFIED | PASS | PASS | Copies screen tiles & GBC attributes to vBGMap0, handles photo album (`00:0B69`) |

---

## Completed Work

- **`ClearBytes` (`00:29DF`)**: Status: `VERIFIED`.
- **`ClearWRAMBytes` (`00:29DC`)**: Status: `VERIFIED`.
- **`ClearHRAMBytesAndWRAM` (`00:29D3`)**: Status: `VERIFIED`.
- **`ClearHRAMAndWRAM` (`00:29D0`)**: Status: `VERIFIED`.
- **`ClearWRAMAndLowerHRAM` (`00:29CB`)**: Status: `VERIFIED`.
- **`ClearLowerWRAM` (`00:29C6`)**: Status: `VERIFIED`.
- **`ClearLowerAndMiddleWRAM` (`00:29C1`)**: Status: `VERIFIED`.
- **`FillBGMapBlack` (`00:28F0`)**: Status: `VERIFIED`.
- **`FillBGMapWhite` (`00:28F7`)**: Status: `VERIFIED`.
- **`FillBGMap` (`00:28FC`)**: Status: `VERIFIED`.
- **`CopyData` (`00:2914`)**: Status: `VERIFIED`.
- **`CopyDataFromBank` (`00:2908`)**: Status: `VERIFIED`.
- **`DrawCommandToVRAM` (`00:2941`)**: Status: `VERIFIED`.
- **`DrawCommandToVRAMDuringRoomTransition` (`00:2991`)**: Status: `VERIFIED`.
- **`ExecuteDrawCommands` (`00:2927`)**: Status: `VERIFIED`.
- **`AdjustBankNumberForGBC` (`00:0B0B`)**: Status: `VERIFIED`.
- **`SwitchBank` (`00:080C`)**: Status: `VERIFIED`.
- **`SwitchAdjustedBank` (`00:0813`)**: Status: `VERIFIED`.
- **`ReloadSavedBank` (`00:081D`)**: Status: `VERIFIED`.
- **`CopyObjectsAttributesToWRAM2` (`00:0B1A`)**: Status: `VERIFIED`.
- **`CopyToBGMap0` (`00:0B96`)**: Status: `VERIFIED`.
- **`CopyDataToVRAM_noDMA` (`00:0A01`)**: Status: `VERIFIED`.
- **`CopyDataToVRAM` (`00:0A13`)**: Status: `VERIFIED`.
- **`CopyData_trampoline` (`00:0B5D`)**: Status: `VERIFIED`.
- **`func_BB5` (`00:0BB5`)**: Status: `VERIFIED`.
- **`CopyBGMapFromBank` (`00:0B69`)**: Status: `VERIFIED`.

---

## Current Work

- **Task**: Selection of next Bank 0 subsystem

---

## Technical Discoveries

- **Assembly & Memory Verification**:
  - `wGameplayType` is at `$DB95`, `GAMEPLAY_PHOTO_ALBUM` is `$0D`.
  - `wIsFileSelectionArrowShifted` is at `$D000`.
  - `hMultiPurposeF` is at `$FFE6`.
  - `CopyBGMapFromBank`: copies a full 20x18 screen of tiles and (on GBC) attributes stored immediately after the 360 tile indices at `hl + 0x168`.
  - `wCurrentBank` is at `$DBAF`.
  - `hMultiPurpose0` is at `$FFD7`.
  - `AdjustBankNumberForGBC`: LADX uses banks `$00`-$`1F` for DMG and banks `$20`-$`3F` for GBC color/palette data and code. Bit 5 (`$20`) toggles the bank selection between DMG and GBC banks.
  - `vBGMap0` is `$9800`, `vBGMap1` is `$9C00`.
  - `CopyToBGMap0`: The Game Boy screen is 20x18 tiles (160x144 pixels). Each BG map row is 32 bytes (256 pixels). After copying 20 bytes, the code adds `$0C` (12 bytes) to `e` with carry to `d`. It iterates until `de == $9A40` (18 rows of 32 bytes = 576 bytes offset from `$9800`).
  - `CopyDataToVRAM`: On CGB, sets `rHDMA1 = b`, `rHDMA2 = $00`, `rHDMA3 = c`, `rHDMA4 = $00`, `rHDMA5 = $0F` (GDMA 256 bytes).
  - `FillBGMap`: Fills `bc` bytes at `vBGMap0` with value `a`.
  - `FillBGMapBlack`: fills `$400` bytes (1 map = 1024 bytes) with `DIALOG_BG_TILE_DARK` (`$7E`).
  - `FillBGMapWhite`: fills `$800` bytes (2 maps = 2048 bytes) with `$7F`.
  - `wram0Section` starts at `$C000`.
  - `wAudioSection` starts at `$D300`, so `wAudioSection - wram0Section = 0x1300`.
  - `wDrawCommandsSection` starts at `$D600`, so `wDrawCommandsSection - wram0Section = 0x1600`.
  - `$DF00 - wram0Section = 0x1F00`. Stack pointer is initialized at `wStackTop` (`$DFFF`), so WRAM clearing up to `$DF00` deliberately leaves the stack frame area intact!
  - `hGameValuesSection` starts at `$FF90`.
  - `hNextDefaultMusicTrack` is at `$FFBF`, difference = `0x2F` bytes.
  - `hIsComputingFrame` is at `$FFFD`, difference = `0x6D` bytes.
  - `hIsGBC` is located at `$FFFE`. In `ClearBytes`, the function explicitly reads `hIsGBC`, pushes AF, clears BC bytes in loop, pops AF, and writes back `hIsGBC`, preserving hardware identification even when HRAM blocks are wiped.
  - `rSelectROMBank` is MBC1/MBC3/MBC5 register `$2100`. `CopyDataFromBank` writes bank, copies, then restores bank 1 with `ld a, $01; ld [rSelectROMBank], a`.
  - Draw command format:
    - Byte 0: Destination address high byte (`h`). If zero, terminates command list.
    - Byte 1: Destination address low byte (`l`).
    - Byte 2: Command byte `a` = `(mode << 6) | (length - 1)`. Bits 0-5 = actual count - 1. Bits 6-7 = command type.
    - Bytes 3+: Payload data (for copy commands: `length` bytes; for fill commands: 1 byte).
    - Wrapping behavior in row mode: If `(hl & 0x1F) == 0` after incrementing, it wraps back to start of current row (`hl -= 32`)!
    - Column mode: Step is `+32` (`0x20`) per tile via `add hl, bc`.
    - Room transition mode: Tile `$EE` is transparent and not written.
- **ROM Verification**:
  - Original ROM `azle.gbc` MD5 is `07c211479386825042efb4ad31bb525f`.
  - `make azle.gbc` with RGBDS v1.0.3 successfully generates bit-identical ROM and symbol map `azle.sym`.

---

## Verification Log

- All 26 functions tested and verified.

---

## Problems and Blockers

- None currently.

---

## Failed Attempts

- None.

---

## Decisions

- **Architectural Design**:
  - C/C++ decompilation is structured with explicit Game Boy architecture state (`GBState`), allowing each function to operate on a memory-mapped state directly or via clean C pointers/references.
  - Memory offsets, constants, and label names strictly match `LADX-Disassembly/src/constants/` to maintain 1:1 mapping with original assembly.
  - Comprehensive unit testing in `tests/` will validate memory alterations byte-for-byte against the assembly's documented preconditions and postconditions.
