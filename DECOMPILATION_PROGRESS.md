# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 5.4%
* **Number of Verified Functions**: 65
* **Number of Decompiled Functions**: 65
* **Number Remaining**: ~1200+ functions
* **Current Subsystem**: Bank 0 - Piece of Heart Meter & Transition Audio Helpers
* **Current Task**: Completed and verified Piece of Heart meter VRAM loaders, transition noise/movement helpers, and Farcall trampoline
* **Last Completed Task**: Decompiled and verified `CopyTilesToPieceOfHeartMeter`, `LoadPieceOfHeartMeterTiles1`, `LoadPieceOfHeartMeterTiles2`, `ClearPieceOfHeartMeterTiles1`, `ClearPieceOfHeartMeterTiles2`, `playNoiseStairs`, `disableMovementInTransition`, and `Farcall_trampoline`
* **Next Task**: Select next unfinished Bank 0 subsystem (Entity spawning trampolines or Room transition routines)
* **Last Update Timestamp**: 2026-09-06T04:45:00+03:00

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
| `CopyTilesToPieceOfHeartMeter` | VERIFIED | PASS | PASS | Copies 3 tiles to VRAM, resets BG loading flags, selects bank $0C (`00:0080`) |
| `LoadPieceOfHeartMeterTiles1` | VERIFIED | PASS | PASS | Loads 3 Piece of Heart meter tiles to vTiles1 + $1A0 (`00:0062`) |
| `LoadPieceOfHeartMeterTiles2` | VERIFIED | PASS | PASS | Loads 3 Piece of Heart meter tiles to vTiles1 + $1D0 (`00:006A`) |
| `ClearPieceOfHeartMeterTiles1` | VERIFIED | PASS | PASS | Restores overwritten equipment tiles to vTiles1 + $1D0 (`00:0072`) |
| `ClearPieceOfHeartMeterTiles2` | VERIFIED | PASS | PASS | Restores overwritten equipment tiles to vTiles1 + $1A0 (`00:007A`) |
| `playNoiseStairs` | VERIFIED | PASS | PASS | Plays stairs noise SFX and disables transition motion (`00:0C9A`) |
| `disableMovementInTransition` | VERIFIED | PASS | PASS | Sets motion state to fade out and resets transition counters (`00:0C9E`) |
| `Farcall_trampoline` | VERIFIED | PASS | PASS | Resolves target function address from wFarcallAdressHigh/Low (`00:0BE7`) |

---

## Technical Discoveries

- **Header / UI VRAM Loaders (`code/home/header.asm`)**:
  - `CopyTilesToPieceOfHeartMeter` (`00:0080`): copies $30 bytes from `hl` to `de` using `CopyData`, sets `hNeedsUpdatingBGTiles = 0`, `hBGTilesLoadingStage = 0`, and selects ROM bank `$0C`.
  - `LoadPieceOfHeartMeterTiles1` (`00:0062`): loads $30 bytes from `PieceOfHeartMeterTiles` (`0x6900`) to `vTiles1 + $1A0` (`0x89A0`).
  - `LoadPieceOfHeartMeterTiles2` (`00:006A`): loads $30 bytes from `PieceOfHeartMeterTiles + $30` (`0x6930`) to `vTiles1 + $1D0` (`0x89D0`).
  - `ClearPieceOfHeartMeterTiles1` (`00:0072`): restores $30 bytes from `InventoryEquipmentItemsTiles + $1D0` (`0x49D0`) to `vTiles1 + $1D0` (`0x89D0`).
  - `ClearPieceOfHeartMeterTiles2` (`00:007A`): restores $30 bytes from `InventoryEquipmentItemsTiles + $1A0` (`0x49A0`) to `vTiles1 + $1A0` (`0x89A0`).
- **Transition Helpers (`code/bank0.asm`)**:
  - `playNoiseStairs` (`00:0C9A`): plays `NOISE_SFX_STAIRS` into `hNoiseSfx`, then chains into `disableMovementInTransition`.
  - `disableMovementInTransition` (`00:0C9E`): sets `wLinkMotionState` to `LINK_MOTION_MAP_FADE_OUT` (`$03`), resets `wTransitionSequenceCounter`, `wC16C`, and `wD478` to 0.
- **Farcall Trampoline (`code/bank0.asm`)**:
  - `Farcall_trampoline` (`00:0BE7`): reconstructs 16-bit address from `wFarcallAdressHigh` (`$DE02`) and `wFarcallAdressLow` (`$DE03`) and executes it.

---

## Verification Log

- 65 functions tested and verified with 100% pass rate.
