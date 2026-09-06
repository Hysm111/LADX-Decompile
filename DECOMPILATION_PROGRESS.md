# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 7.08%
* **Number of Verified Functions**: 85
* **Number of Decompiled Functions**: 85
* **Number Remaining**: ~1180+ functions
* **Current Subsystem**: Bank 0 - Graphics, Dialog & Rubble Dispatchers
* **Current Task**: Completed and verified `ChangeBGColumnPaletteAndExecuteDrawCommands`, `func_A9B`, and 8 Bank 0 trampolines
* **Last Completed Task**: Decompiled and verified `ChangeBGColumnPaletteAndExecuteDrawCommands`, `func_A9B`, `Spawn2x2RubbleEntities_trampoline`, `func_A5F`, `func_036_703E_trampoline`, `cycleInstrumentItemColor_trampoline`, `func_036_4A77_trampoline`, `GetOwlStatueDialogId_trampoline`, `SpawnPhotographer_trampoline`, and `LoadPhotoBgMap_trampoline`
* **Next Task**: Proceed to the next Bank 0 subsystem (`func_020_6D0E_trampoline`, `func_983`, `func_999`, `GetEntityInitHandler_trampoline`, `func_020_4874_trampoline`, `func_020_4954_trampoline`, `ReplaceObjects56and57_trampoline`)
* **Last Update Timestamp**: 2026-09-06T06:22:00+03:00

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
| `func_BB5` | VERIFIED | PASS | PASS | Selects bank, copies 0x100 bytes to VRAM, restores bank 0x20 (`00:0BB5`) |
| `CopyBGMapFromBank` | VERIFIED | PASS | PASS | Selects bank, copies 0x100 bytes from $6800 to VRAM, restores bank $24 (`00:0B52`) |
| `CopySirenInstrumentTiles` | VERIFIED | PASS | PASS | Copies Siren Instrument tiles to vTiles2 with bank switching (`00:0B67`) |
| `func_BC5` | VERIFIED | PASS | PASS | Copies 128 bytes from $6B60 in bank $12 to $C500 in WRAM, restores bank 1 (`00:0BC5`) |
| `CopyColorDungeonSymbols` | VERIFIED | PASS | PASS | Copies 0x20 bytes from bank $35 to animated scroll storage (`00:0A32`) |
| `RestoreBankAndReturn` | VERIFIED | PASS | PASS | Restores wCurrentBank into rSelectROMBank (`00:08DF`) |
| `LoadBank1AndReturn` | VERIFIED | PASS | PASS | Loads bank 1 into rSelectROMBank and returns (`00:0917`) |
| `RestoreStackedBankAndReturn` | VERIFIED | PASS | PASS | Pops stacked bank into rSelectROMBank without modifying wCurrentBank (`00:0973`) |
| `RestoreStackedBank` | VERIFIED | PASS | PASS | Pops stacked bank into both rSelectROMBank and wCurrentBank (`00:0AB0`) |
| `Farcall` | VERIFIED | PASS | PASS | Farcall dispatcher switching to wFarcallBank and returning to wFarcallReturnBank (`00:0BD7`) |
| `BackupObjectInRAM2` | VERIFIED | PASS | PASS | Backs up object to WRAM bank 2 on GBC with ignore list filtering (`00:0B2F`) |
| `NoRoomTransitionDrawLoop` | VERIFIED | PASS | PASS | Processes queue of draw commands in WRAM until terminator (`00:0BA6`) |
| `LCDOff` | VERIFIED | PASS | PASS | Safely waits for VBlank and disables LCD (`00:0B7C`) |
| `IsZero` | VERIFIED | PASS | PASS | Checks if 16-byte entity structure is entirely zero (`00:0B8E`) |
| `DecrementEntityCountdown` | VERIFIED | PASS | PASS | Decrements active countdown timer for an entity slot (`00:0BDB`) |
| `PlayWrongAnswerJingle` | VERIFIED | PASS | PASS | Triggers error buzzer sound effect (`00:0C8A`) |
| `AlertSwordMoblins` | VERIFIED | PASS | PASS | Triggers sword poking sound and transient effect (`00:0C8F`) |
| `PlayBombExplosionSfx` | VERIFIED | PASS | PASS | Triggers bomb explosion sound (`00:0C94`) |
| `GetDialogRequest_trampoline` | VERIFIED | PASS | PASS | Calls GetDialogRequest in bank $3E and restores bank 1 (`00:0BF7`) |
| `OpenDialogInTable07_trampoline` | VERIFIED | PASS | PASS | Calls OpenDialogInTable07 in bank $3E and restores bank 1 (`00:0BFD`) |
| `GetDialogIndex_trampoline` | VERIFIED | PASS | PASS | Calls GetDialogIndex in bank $3E and restores bank 1 (`00:0C03`) |
| `MarkRoomTriggerAsResolved` | VERIFIED | PASS | PASS | Sets bit in room resolved flags array in WRAM (`00:0C84`) |
| `MarkRoomTriggerAsResolved_trampoline` | VERIFIED | PASS | PASS | Calls MarkRoomTriggerAsResolved and restores stacked bank (`00:0C80`) |
| `ClearLinkPositionHistory` | VERIFIED | PASS | PASS | Clears 48-byte Link position and speed history table (`00:0CA3`) |
| `ResetLinkPosition` | VERIFIED | PASS | PASS | Resets Link X and Y positions to default screen center (`00:0CA9`) |
| `SetLinkPositionToCenter` | VERIFIED | PASS | PASS | Resets Link position to default screen center ($48, $40) (`00:0CB1`) |
| `ClearLinkInvulnerability` | VERIFIED | PASS | PASS | Clears invulnerability countdown (`00:0CB8`) |
| `ClearLinkMotionState` | VERIFIED | PASS | PASS | Clears Link motion flags and state (`00:0CBD`) |
| `ClearLinkState` | VERIFIED | PASS | PASS | Fully resets Link state (`00:0CC2`) |
| `AddTranscientVfx` | VERIFIED | PASS | PASS | Spawns transient visual effect into first free slot (`00:0CC7`) |
| `label_D07` | VERIFIED | PASS | PASS | Offsets effect coordinates by -8 and triggers sword poke VFX (`00:0D07`) |
| `label_D15` | VERIFIED | PASS | PASS | Sets sword poke jingle and spawns poke VFX (`00:0D15`) |
| `LoadPieceOfHeartMeterTiles` | VERIFIED | PASS | PASS | Dispatches heart meter tile loading based on pieces count (`00:0055`) |
| `ClearPieceOfHeartMeterTiles` | VERIFIED | PASS | PASS | Restores equipment tiles for heart meter slots (`00:005B`) |
| `LoadPieceOfHeartMeterTiles1` | VERIFIED | PASS | PASS | Loads 3 Piece of Heart meter tiles to vTiles1 + $1A0 (`00:0062`) |
| `LoadPieceOfHeartMeterTiles2` | VERIFIED | PASS | PASS | Loads 3 Piece of Heart meter tiles to vTiles1 + $1D0 (`00:006A`) |
| `ClearPieceOfHeartMeterTiles1` | VERIFIED | PASS | PASS | Restores overwritten equipment tiles to vTiles1 + $1D0 (`00:0072`) |
| `ClearPieceOfHeartMeterTiles2` | VERIFIED | PASS | PASS | Restores overwritten equipment tiles to vTiles1 + $1A0 (`00:007A`) |
| `playNoiseStairs` | VERIFIED | PASS | PASS | Plays stairs noise SFX and disables transition motion (`00:0C9A`) |
| `disableMovementInTransition` | VERIFIED | PASS | PASS | Sets motion state to fade out and resets transition counters (`00:0C9E`) |
| `Farcall_trampoline` | VERIFIED | PASS | PASS | Resolves target function address from wFarcallAdressHigh/Low (`00:0BE7`) |
| `LoadDungeonMinimapTiles` | VERIFIED | PASS | PASS | Loads minimap tiles (stages 0-7) and executes palette stages 8-11 (`00:0826`) |
| `PlayAudioStep` | VERIFIED | PASS | PASS | Executes audio step: PlaySfx in bank 1F and music tracks in 1B/1E (`00:08A4`) |
| `UpdateLinkWalkingAnimation_trampoline` | VERIFIED | PASS | PASS | Switches to bank 2, calls animation update, and reloads saved bank (`00:0BF0`) |
| `func_020_6A30_trampoline` | VERIFIED | PASS | PASS | Calls target in bank $20 and restores saved bank (`00:08D7`) |
| `func_020_6AC1_trampoline` | VERIFIED | PASS | PASS | Calls target in bank $20 and restores saved bank (`00:08E6`) |
| `UpdateIntroSeaBGPalettes_trampoline` | VERIFIED | PASS | PASS | Calls UpdateIntroSeaBGPalettes in bank $20 and restores saved bank (`00:08F0`) |
| `ClearFileMenuBG_trampoline` | VERIFIED | PASS | PASS | Calls ClearFileMenuBG in bank $20 and restores stacked bank (`00:08FA`) |
| `LoadFileMenuBG_trampoline` | VERIFIED | PASS | PASS | Calls LoadFileMenuBG in bank $20 and loads bank 1 (`00:0905`) |
| `CopyLinkTunicPalette_trampoline` | VERIFIED | PASS | PASS | Calls CopyLinkTunicPalette in bank $20 and loads bank 1 (`00:090F`) |
| `LoadBaseTiles_trampoline` | VERIFIED | PASS | PASS | Calls LoadBaseTiles and restores stacked bank (`00:0BBE`) |
| `ChangeBGColumnPaletteAndExecuteDrawCommands` | VERIFIED | PASS | PASS | Calls ChangeBGColumnPalette in bank $24, runs draw queue, restores stacked bank (`00:0AB5`) |
| `func_A9B` | VERIFIED | PASS | PASS | Switches to BANK(FontTiles) ($0F), executes dialog, restores stacked bank (`00:0A9B`) |
| `Spawn2x2RubbleEntities_trampoline` | VERIFIED | PASS | PASS | Switches to bank $36, spawns rubble entities, restores stacked bank (`00:0AA7`) |
| `func_A5F` | VERIFIED | PASS | PASS | Selects bank $20, calls RenderActiveEntitySpritesRect, restores stacked bank (`00:0A5F`) |
| `func_036_703E_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls target, restores stacked bank (`00:0AC6`) |
| `cycleInstrumentItemColor_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls color cycler, restores stacked bank (`00:0AD2`) |
| `func_036_4A77_trampoline` | VERIFIED | PASS | PASS | Switches to bank $36, calls target, restores stacked bank (`00:0ADE`) |
| `GetOwlStatueDialogId_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls GetOwlStatueDialogId, restores stacked bank (`00:0AEA`) |
| `SpawnPhotographer_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls SpawnPhotographer, restores stacked bank (`00:0AF6`) |
| `LoadPhotoBgMap_trampoline` | VERIFIED | PASS | PASS | Selects bank $3D and calls LoadPhotoBgMap (`00:0B02`) |

---

## Technical Discoveries

- **Minimap Tile Loader (`code/bank0.asm:00:0826`)**:
  - Incremental loader using `hBGTilesLoadingStage`:
    - Stages 0 to 7: copies 64 bytes (`$40`) from `DungeonMinimapTiles` (`$7E00` in bank `$12`, adjusted for GBC) to `vTiles1 + $500` (`$8D00`) at offset `stage * 0x40`.
    - Stage 8: calls `CopyDungeonMinimapPalette` in bank 2, increments stage.
    - Stage 9: calls `label_002_6827` in bank 2, increments stage.
    - Stage 10: calls `label_002_680B` in bank 2, increments stage.
    - Stage 11+: calls `label_002_67E5` in bank 2, clears `hNeedsUpdatingBGTiles` and `hBGTilesLoadingStage`.
- **Audio Step (`code/bank0.asm:00:08A4`)**:\
  - Always executes SFX handler via `SwitchBank(0x1F)`.
  - If `hWaveSfx` != 0, halts step.
  - If `wMusicTrackTiming == 0`: standard speed (calls 0x1B and 0x1E tracks once).
  - If `wMusicTrackTiming == 2`: half speed (executes only when `hFrameCounter & 1 == 0`).
  - Otherwise (timing != 0 && timing != 2): double speed (calls 0x1B and 0x1E tracks twice).
- **Bank Trampolines and Graphic/Dialog Handlers**:
  - Trampolines switch `rSelectROMBank` or execute `SwitchBank` to target banks (such as `$0F` for font dialog, `$20` for palettes/active sprite rects, `$24` for BG columns, `$36` for rubble/items/photographer, and `$3D` for photo background map).
  - Stacked bank restoration is handled via `RestoreStackedBankAndReturn` (restores `rSelectROMBank` only) or `RestoreStackedBank` (updates both `rSelectROMBank` and `wCurrentBank`).

---

## Verification Log

- 85 functions tested and verified with 100% pass rate.
