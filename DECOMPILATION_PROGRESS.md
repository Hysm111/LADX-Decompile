# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 11.83%
* **Number of Verified Functions**: 142
* **Number of Decompiled Functions**: 142
* **Number Remaining**: ~1058+ functions
* **Current Subsystem**: Bank 0 - Room Trampolines, Base/Menu Tile Loaders & Credits Sequences
* **Current Task**: Completed and verified LoadCreditsRollTiles, LoadCreditsLinkFaceCloseUpTiles, LoadCreditsLinkSeatedOnLogTiles, func_2B92, GetRoomStatusAddressForMapPosition_trampoline, LoadBaseTiles, LoadMenuTiles
* **Last Completed Task**: Decompiled and verified Bank 0 ending scenes, credits roll, room status address trampoline, and base/menu tile loading routines (`00:2B26` - `00:2C27`)
* **Next Task**: Continue Bank 0 graphic loaders (`LoadIntroSequenceTiles`, `LoadTitleScreenTiles`, `LoadWorldMapTiles`, `LoadStaticPictureTiles`, `LoadEaglesTowerTopTiles`, `LoadMarinBeachTiles`, `LoadSaveMenuTiles`)
* **Last Update Timestamp**: 2026-09-06T16:25:00+03:00

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
| `LoadTileset0F_trampoline` | VERIFIED | PASS | PASS | Switches to bank $01 via SwitchBank and jumps to LoadTileset0F (`00:28E8`) |
| `GetChestsStatusForRoom_trampoline` | VERIFIED | PASS | PASS | Farcalls GetChestsStatusForRoom in bank $14, reloads saved bank (`00:29ED`) |
| `PlayBoomerangSfx_trampoline` | VERIFIED | PASS | PASS | Farcalls PlayBoomerangSfx in bank $20, restores bank from wCurrentBank (`00:29F8`) |
| `func_2A07` | VERIFIED | PASS | PASS | Farcalls func_001_5A59 in bank $01, reloads saved bank (`00:2A07`) |
| `GetObjectPhysicsFlags` | VERIFIED | PASS | PASS | Reads physics flags from Overworld or Color Dungeon tables in Bank $08 (`00:2A12`) |
| `GetObjectPhysicsFlags_trampoline` | VERIFIED | PASS | PASS | Calls GetObjectPhysicsFlags, reloads saved bank via ReloadSavedBank (`00:2A26`) |
| `GetObjectPhysicsFlagsAndRestoreBank3` | VERIFIED | PASS | PASS | Calls GetObjectPhysicsFlags, restores ROM bank 3 (`00:2A2C`) |
| `LoadCreditsKoholintDisappearingTiles` | VERIFIED | PASS | PASS | Copies Koholint island disappearing animation tiles to vTiles2/vTiles1 (`00:2A37`) |
| `LoadCreditsStairsTiles` | VERIFIED | PASS | PASS | Calls LoadTileset15, loads credits stairs tiles to vTiles0+$400 (`00:2A57`) |
| `LoadTileset15` | VERIFIED | PASS | PASS | Loads credits sequence tileset across Ending, Overworld1, and Npc4 tiles (`00:2A66`) |
| `LoadCreditsKoholintViewsTiles` | VERIFIED | PASS | PASS | Copies Koholint scenic view tiles to vTiles2/vTiles0/vTiles1 (`00:2AAE`) |
| `LoadCreditsLinkOnSeaCloseTiles` | VERIFIED | PASS | PASS | Loads Link on sea close tiles (DMG: EndingTiles, GBC: PhotoAlbumTiles bank $35) (`00:2AEA`) |
| `LoadCreditsSunAboveTiles` | VERIFIED | PASS | PASS | Loads sun above sea view tiles to vTiles0 and vTiles1 (`00:2AF9`) |
| `LoadCreditsLinkOnSeaLargeTiles` | VERIFIED | PASS | PASS | Loads large Link on sea view tiles to vTiles0 and vTiles1 (`00:2AFE`) |
| `LoadCreditsRollTiles` | VERIFIED | PASS | PASS | Loads rolling credits font and NPC tiles with audio steps interspersed (`00:2B26`) |
| `LoadCreditsLinkFaceCloseUpTiles` | VERIFIED | PASS | PASS | Loads Link face close-up ending scene (DMG: EndingTiles, GBC: EndingCGBAltTiles) (`00:2B72`) |
| `LoadCreditsLinkSeatedOnLogTiles` | VERIFIED | PASS | PASS | Loads Link seated on log ending scene (DMG: EndingTiles, GBC: PhotoAlbumTiles) (`00:2B81`) |
| `func_2B92` | VERIFIED | PASS | PASS | Helper loading 0x80 tiles to vTiles0, and 0x80 tiles each to vTiles1 and vTiles2 (`00:2B92`) |
| `GetRoomStatusAddressForMapPosition_trampoline` | VERIFIED | PASS | PASS | Farcalls GetRoomStatusAddressForMapPosition in bank $14, reloads saved bank (`00:2BC1`) |
| `LoadBaseTiles` | VERIFIED | PASS | PASS | Loads Link sprites and inventory equipment tiles, restores bank 1 (`00:2BCF`) |
| `LoadMenuTiles` | VERIFIED | PASS | PASS | Calls LoadBaseTiles, then loads menu UI tiles and font tiles (`00:2C03`) |

---

## Technical Notes & Implementation Details

1. **Static Object Physics Table (`00:2A12`)**:
   - Switches ROM bank to `BANK(OverworldObjectPhysicFlags)` (`$08`).
   - If `hMapId == MAP_COLOR_DUNGEON` (`$FF`), indexes into `Indoors1ObjectPhysicFlags` (`08:4BD4`).
   - Otherwise, indexes into `OverworldObjectPhysicFlags` (`08:4AD4`).
   - `GetObjectPhysicsFlags_trampoline` preserves and restores the calling bank via `ReloadSavedBank`.
   - `GetObjectPhysicsFlagsAndRestoreBank3` specifically restores ROM bank `0x03`.

2. **Credits & Ending Graphic Loaders (`00:2A37`-`00:2BBF`)**:
   - `LoadCreditsRollTiles` coordinates audio stepping via `PlayAudioStep` between copying large font tiles, Npc3 tiles, and credits roll text tiles.
   - `LoadCreditsLinkFaceCloseUpTiles` and `LoadCreditsLinkSeatedOnLogTiles` handle DMG vs CGB tile asset selection:
     - Close-up: uses `EndingTiles + $3800` (bank `$13`) on DMG, `EndingCGBAltTiles` (bank `$35`) on CGB.
     - Seated on log: uses `EndingTiles + $800` (bank `$13`) on DMG, `PhotoAlbumTiles + $800` (bank `$35`) on CGB.
     - Common ending tile layers are copied to `vTiles1` (`EndingTiles + $3000`) and `vTiles2` (`EndingTiles + $2800`).

3. **Base and Menu Tile Loaders (`00:2BCF`-`00:2C25`)**:
   - `LoadBaseTiles` loads core gameplay tiles:
     - `LinkCharacterTiles` (`0x4000`, bank `$0C`) to `vTiles0` (`0x400` bytes)
     - `InventoryEquipmentItemsTiles` (`0x4800`, bank `$0C`) to `vTiles1` (`0x1000` bytes)
     - `Items1Tiles + $3A0` (`0x47A0`, bank `$0C`) to `vTiles1 + $600` (`0x20` bytes)
     - Explicitly resets bank to `$01` via `SwitchBank`.
   - `LoadMenuTiles` chains from `LoadBaseTiles`, loading `MenuTiles` (`0x4000`, bank `$0F`, `0x400` bytes) to `vTiles1` and `FontTiles` (`0x5000`, bank `$0F`, `0x800` bytes) to `vTiles2`.
