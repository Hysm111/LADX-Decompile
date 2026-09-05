# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 4.7%
* **Number of Verified Functions**: 57
* **Number of Decompiled Functions**: 57
* **Number Remaining**: ~1200+ functions
* **Current Subsystem**: Bank 0 - Foundational Routines
* **Current Task**: Completed and verified Bank 0 bank restoration, farcall, object backup, and sword poke VFX routines
* **Last Completed Task**: Decompiled and verified `RestoreBankAndReturn`, `LoadBank1AndReturn`, `RestoreStackedBankAndReturn`, `RestoreStackedBank`, `Farcall`, `func_BC5`, `CopyColorDungeonSymbols`, `BackupObjectInRAM2`, `label_D07`, and `label_D15`
* **Next Task**: Select next unfinished Bank 0 or Bank 1 subsystem
* **Last Update Timestamp**: 2026-09-06T04:20:00+03:00

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
| `ReadValueInDialogsBank` | VERIFIED | PASS | PASS | Reads byte at HL+BC in dialogs bank ($1C), restores bank 1 (`00:0C2D`) |
| `ReadTileValueFromAsciiTable` | VERIFIED | PASS | PASS | Reads tile index from CodepointToTileMap in bank $1C (`00:0C25`) |
| `ReadTileValueFromDiacriticsTable` | VERIFIED | PASS | PASS | Reads diacritic index from CodepointToDiacritic in bank $1C (`00:0C2A`) |
| `MarkTriggerAsResolved` | VERIFIED | PASS | PASS | Resolves room puzzle trigger, plays puzzle solved jingle (`00:0C60`) |
| `ApplyMapFadeOutTransitionWithNoise` | VERIFIED | PASS | PASS | Starts map fade out with timer $30 and stairs noise (`00:0C7D`) |
| `ApplyMapFadeOutTransition` | VERIFIED | PASS | PASS | Starts map fade out with timer $30 without sound (`00:0C83`) |
| `ApplyMapFadeOutTransitionWithSound` | VERIFIED | PASS | PASS | Starts map fade out, preserving music if indoors (`00:0C89`) |
| `ResetSpinAttack` | VERIFIED | PASS | PASS | Clears spin attack & sword charge, then resets pegasus boots (`00:0CAF`) |
| `ResetPegasusBoots` | VERIFIED | PASS | PASS | Clears pegasus boots charge meter & running flag (`00:0CB6`) |
| `CopyLinkFinalPositionToPosition` | VERIFIED | PASS | PASS | Copies hLinkFinalPositionX/Y to hLinkPositionX/Y (`00:0CBE`) |
| `AddTranscientVfx` | VERIFIED | PASS | PASS | Allocates slot and registers temporary visual effect sprite (`00:0CC7`) |
| `RestoreBankAndReturn` | VERIFIED | PASS | PASS | Restores wCurrentBank into rSelectROMBank (`00:08DF`) |
| `LoadBank1AndReturn` | VERIFIED | PASS | PASS | Loads bank 1 into rSelectROMBank and returns (`00:0917`) |
| `RestoreStackedBankAndReturn` | VERIFIED | PASS | PASS | Restores stacked bank into rSelectROMBank and returns (`00:0973`) |
| `RestoreStackedBank` | VERIFIED | PASS | PASS | Switches bank and saves to wCurrentBank from stacked bank (`00:0AB0`) |
| `Farcall` | VERIFIED | PASS | PASS | Dispatches call to wFarcallBank and returns to wFarcallReturnBank (`00:0BD7`) |
| `func_BC5` | VERIFIED | PASS | PASS | Copies bytes using bank w2_D16A and restores bank $28 (`00:0BC5`) |
| `CopyColorDungeonSymbols` | VERIFIED | PASS | PASS | Copies 32 bytes from ColorDungeonNpcTiles to animated tile buffer (`00:0A32`) |
| `BackupObjectInRAM2` | VERIFIED | PASS | PASS | Backs up overworld object to WRAM bank 2 with ignore list filtering (`00:0B2F`) |
| `label_D07` | VERIFIED | PASS | PASS | Sets up sword poke VFX position from wC140/wC142 minus 8 (`00:0D07`) |
| `label_D15` | VERIFIED | PASS | PASS | Plays sword poking jingle and adds TRANSCIENT_VFX_SWORD_POKE (`00:0D15`) |

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
- **`ReadValueInDialogsBank` (`00:0C2D`)**: Status: `VERIFIED`.
- **`ReadTileValueFromAsciiTable` (`00:0C25`)**: Status: `VERIFIED`.
- **`ReadTileValueFromDiacriticsTable` (`00:0C2A`)**: Status: `VERIFIED`.
- **`MarkTriggerAsResolved` (`00:0C60`)**: Status: `VERIFIED`.
- **`ApplyMapFadeOutTransitionWithNoise` (`00:0C7D`)**: Status: `VERIFIED`.
- **`ApplyMapFadeOutTransition` (`00:0C83`)**: Status: `VERIFIED`.
- **`ApplyMapFadeOutTransitionWithSound` (`00:0C89`)**: Status: `VERIFIED`.
- **`ResetSpinAttack` (`00:0CAF`)**: Status: `VERIFIED`.
- **`ResetPegasusBoots` (`00:0CB6`)**: Status: `VERIFIED`.
- **`CopyLinkFinalPositionToPosition` (`00:0CBE`)**: Status: `VERIFIED`.
- **`AddTranscientVfx` (`00:0CC7`)**: Status: `VERIFIED`.
- **`RestoreBankAndReturn` (`00:08DF`)**: Status: `VERIFIED`.
- **`LoadBank1AndReturn` (`00:0917`)**: Status: `VERIFIED`.
- **`RestoreStackedBankAndReturn` (`00:0973`)**: Status: `VERIFIED`.
- **`RestoreStackedBank` (`00:0AB0`)**: Status: `VERIFIED`.
- **`Farcall` (`00:0BD7`)**: Status: `VERIFIED`.
- **`func_BC5` (`00:0BC5`)**: Status: `VERIFIED`.
- **`CopyColorDungeonSymbols` (`00:0A32`)**: Status: `VERIFIED`.
- **`BackupObjectInRAM2` (`00:0B2F`)**: Status: `VERIFIED`.
- **`label_D07` (`00:0D07`)**: Status: `VERIFIED`.
- **`label_D15` (`00:0D15`)**: Status: `VERIFIED`.

---

## Technical Discoveries

- **Assembly & Memory Verification**:
  - Bank restoration & farcall: `RestoreBankAndReturn` reloads `wCurrentBank` into `rSelectROMBank`. `LoadBank1AndReturn` loads 1 into `rSelectROMBank`. `RestoreStackedBankAndReturn` pops stacked bank into `rSelectROMBank`. `RestoreStackedBank` pops stacked bank and calls `SwitchBank` (updating both `wCurrentBank` and `rSelectROMBank`).
  - Farcall dispatch: `wFarcallBank` ($DE01), `wFarcallAdressHigh` ($DE02), `wFarcallAdressLow` ($DE03), `wFarcallReturnBank` ($DE04).
  - Copy helpers: `func_BC5` reads ROM bank from `w2_D16A` ($D16A), copies `b` bytes from `hl` to `de`, and restores bank `$28`.
  - Color dungeon symbol copy: `CopyColorDungeonSymbols` copies 32 bytes from `ColorDungeonNpcTiles + $F00` ($4F00 in bank $35) to `wAnimatedScrollingTilesStorage` ($DCC0), restoring the stacked bank.
  - Object RAM2 backup: `BackupObjectInRAM2` checks `hIsGBC` and `wIsIndoor` (outdoor GBC only); if bit 7 of `a` is 0, checks `OverworldObjectIgnoreList` (14 bytes in bank 20: 0x03, 0x04, 0x09, 0x5E, 0x91, 0xA1, 0xAA, 0xC4, 0xC6, 0xCC, 0xDB, 0xE1, 0xE3, 0xE8); copies `[hl]` into WRAM bank 2 (`rSVBK = 2`), restores `rSVBK = 0`, and switches ROM bank to `a & 0x7F`.
  - Sword poke VFX: `wC140` (X) - 8, `wC142` (Y) - 8; `label_D15` plays `JINGLE_SWORD_POKING` ($07) into `hJingle` and calls `AddTranscientVfx(TRANSCIENT_VFX_SWORD_POKE = 5)`.

---

## Verification Log

- 57 functions tested and verified with 100% pass rate.
