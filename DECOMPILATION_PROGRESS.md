# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 3.0%
* **Number of Verified Functions**: 36
* **Number of Decompiled Functions**: 36
* **Number Remaining**: ~1200+ functions
* **Current Subsystem**: Bank 0 - Entity & Audio Utilities + LCD Control (10 functions)
* **Current Task**: Completed and verified Bank 0 utilities batch
* **Last Completed Task**: Decompiled and verified `LCDOff`, `IsZero`, `GetEntitySlowTransitionCountdown`, `GetEntityPrivateCountdown1`, `GetEntityTransitionCountdown`, `DecrementEntityIgnoreHitsCountdown`, `PlayWrongAnswerJingle`, `AlertSwordMoblins`, `PlayBombExplosionSfx`, and `CopySirenInstrumentTiles`
* **Next Task**: Identify and begin next unfinished Bank 0 subsystem
* **Last Update Timestamp**: 2026-09-06T03:40:00+03:00

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
| `LCDOff` | VERIFIED | PASS | PASS | Waits for VBlank line 145 and disables LCD safely (`00:28CF`) |
| `IsZero` | VERIFIED | PASS | PASS | Checks whether byte at address HL+BC is zero (`00:0C08`) |
| `GetEntitySlowTransitionCountdown` | VERIFIED | PASS | PASS | Reads slow transition countdown for entity (`00:0BFB`) |
| `GetEntityPrivateCountdown1` | VERIFIED | PASS | PASS | Reads private countdown 1 for entity (`00:0C00`) |
| `GetEntityTransitionCountdown` | VERIFIED | PASS | PASS | Reads transition countdown for entity (`00:0C05`) |
| `DecrementEntityIgnoreHitsCountdown` | VERIFIED | PASS | PASS | Decrements ignore hits countdown for entity if nonzero (`00:0C56`) |
| `PlayWrongAnswerJingle` | VERIFIED | PASS | PASS | Writes JINGLE_WRONG_ANSWER to hJingle (`00:0C20`) |
| `AlertSwordMoblins` | VERIFIED | PASS | PASS | Sets wSwordMoblinAlertingSoundCounter to 4 (`00:0C50`) |
| `PlayBombExplosionSfx` | VERIFIED | PASS | PASS | Writes NOISE_SFX_EXPLOSION to hNoiseSfx and alerts moblins (`00:0C4B`) |
| `CopySirenInstrumentTiles` | VERIFIED | PASS | PASS | Copies 4 tiles (64 bytes) from bank $0C to destination (`00:0C3A`) |

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
- **`LCDOff` (`00:28CF`)**: Status: `VERIFIED`.
- **`IsZero` (`00:0C08`)**: Status: `VERIFIED`.
- **`GetEntitySlowTransitionCountdown` (`00:0BFB`)**: Status: `VERIFIED`.
- **`GetEntityPrivateCountdown1` (`00:0C00`)**: Status: `VERIFIED`.
- **`GetEntityTransitionCountdown` (`00:0C05`)**: Status: `VERIFIED`.
- **`DecrementEntityIgnoreHitsCountdown` (`00:0C56`)**: Status: `VERIFIED`.
- **`PlayWrongAnswerJingle` (`00:0C20`)**: Status: `VERIFIED`.
- **`AlertSwordMoblins` (`00:0C50`)**: Status: `VERIFIED`.
- **`PlayBombExplosionSfx` (`00:0C4B`)**: Status: `VERIFIED`.
- **`CopySirenInstrumentTiles` (`00:0C3A`)**: Status: `VERIFIED`.

---

## Technical Discoveries

- **Assembly & Memory Verification**:
  - `hInterrupts` is at `$FFD2` (5-byte array storing interrupt state).
  - `LCDOff` preserves `rIE` into `hInterrupts`, clears `IEF_VBLANK` to prevent VBlank IRQ during disable, waits until `rLY == 145` (`SCRN_Y + 1`), clears `LCDCF_ON` in `rLCDC`, then restores `rIE` from `hInterrupts`.
  - `wEntitiesTransitionCountdownTable` is at `$C2E0`.
  - `wEntitiesPrivateCountdown1Table` is at `$C2F0`.
  - `wEntitiesIgnoreHitsCountdownTable` is at `$C410`.
  - `wEntitiesSlowTransitionCountdownTable` is at `$C450`.
  - `wSwordMoblinAlertingSoundCounter` is at `$C502`.
  - `hJingle` is at `$FFF2`, `JINGLE_WRONG_ANSWER` is `$1D`.
  - `hNoiseSfx` is at `$FFF4`, `NOISE_SFX_EXPLOSION` is `$0C`.
  - `PlayBombExplosionSfx` sets `hNoiseSfx` and directly falls through into `AlertSwordMoblins`.
  - `BANK(SirenInstrumentsTiles)` is bank `$0C`.
  - `CopySirenInstrumentTiles` copies 64 bytes ($40) using `CopyData` from bank $0C, then restores ROM bank 1.

---

## Verification Log

- All 36 functions tested and verified with 100% pass rate.
