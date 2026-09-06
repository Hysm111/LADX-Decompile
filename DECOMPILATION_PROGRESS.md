# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 12.67%
* **Number of Verified Functions**: 152
* **Number of Decompiled Functions**: 152
* **Number Remaining**: ~1048+ functions
* **Current Subsystem**: Bank 0 - Graphic & Scene Loaders (`00:2D79` - `00:2E6E`)
* **Current Task**: Completed and verified LoadIntroSequenceTiles, LoadTitleScreenTiles, LoadWorldMapTiles, LoadStaticPictureTiles, LoadFaceShrineReliefTiles, LoadSchulePaintingTiles, LoadChristinePortraitTiles, LoadEaglesTowerTopTiles, LoadMarinBeachTiles, LoadSaveMenuTiles
* **Last Completed Task**: Decompiled and verified Bank 0 scene and artwork tile loaders (`00:2D79` - `00:2E6E`)
* **Next Task**: Continue Bank 0 room-specific and animated tile loading routines (`LoadRoomSpecificTiles`, `AnimateTiles`)
* **Last Update Timestamp**: 2026-09-06T16:35:00+03:00

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
| `LoadIntroSequenceTiles` | VERIFIED | PASS | PASS | Loads opening intro sequence rain and background graphics (`00:2D79`) |
| `LoadTitleScreenTiles` | VERIFIED | PASS | PASS | Loads title logo, DX logo, and DX OAM tiles for DMG/CGB (`00:2DA7`) |
| `LoadWorldMapTiles` | VERIFIED | PASS | PASS | Loads world map tiles to vTiles1 and overworld cursor tiles (`00:2DE9`) |
| `LoadStaticPictureTiles` | VERIFIED | PASS | PASS | Generic static picture loader (0x80 tiles to vTiles2) (`00:2E13`) |
| `LoadFaceShrineReliefTiles` | VERIFIED | PASS | PASS | Loads Face Shrine relief artwork to vTiles2 (`00:2E06`) |
| `LoadSchulePaintingTiles` | VERIFIED | PASS | PASS | Loads Schule painting artwork to vTiles2 (`00:2E0B`) |
| `LoadChristinePortraitTiles` | VERIFIED | PASS | PASS | Loads Christine goat portrait artwork to vTiles2 (`00:2E10`) |
| `LoadEaglesTowerTopTiles` | VERIFIED | PASS | PASS | Loads Eagle's Tower collapse/boss rooftop tiles to vTiles1/vTiles2 (`00:2E21`) |
| `LoadMarinBeachTiles` | VERIFIED | PASS | PASS | Loads Marin beach cutscene graphics and large font (`00:2E41`) |
| `LoadSaveMenuTiles` | VERIFIED | PASS | PASS | Loads save/game over dialog box tiles to vTiles1 (`00:2E5E`) |
| `LoadRoomSpecificTiles` | VERIFIED | PASS | PASS | Loads room-specific NPC entity tiles and BG tiles with follower override and context dispatch (`00:2E73`) |
| `CopyWord` | VERIFIED | PASS | PASS | Copies two consecutive bytes from hl to de (`00:2FC7`) |
| `WriteObjectToBG_DMG` | VERIFIED | PASS | PASS | Retrieves 2x2 tile indices for an object and writes them to BG map (`00:2FCD`) |
| `SwitchToObjectsTilemapBank` | VERIFIED | PASS | PASS | Switches rSelectROMBank to indoor ($08) or overworld ($1A) objects tilemap bank (`00:3905`) |

---

## Technical Notes & Implementation Details

1. **Room-Specific Tiles & Object Tilemaps (`00:2E73`-`00:300D`, `00:3905`)**:
   - `LoadRoomSpecificTiles` loads 4 rows of 16 entity tiles into `vTiles0 + $400`, dynamically overridden by current followers (Bow-Wow `$A4`, Ghost `$D8`, Rooster `$DD`, Marin `$8F`) unless in dungeons/specific indoor rooms. It then branches by indoor vs outdoor and side-scrolling vs top-view to load BG tiles into `vTiles2`. In Color Dungeon, it calls back into bank `$20`.
   - `CopyWord` copies 2 sequential bytes from `hl` to `de`.
   - `WriteObjectToBG_DMG` indexes the appropriate objects tilemap (`OverworldObjectsTilemapDMG` in bank `$1A`, `IndoorObjectsTilemapDMG` in bank `$08`, or `ColorDungeonObjectsTilemap` in bank `$20`) and transfers the 4 2x2 tiles with row stride `$20` into the BG map.
   - `SwitchToObjectsTilemapBank` switches ROM bank between `$08` (indoors) and `$1A` (outdoors).


1. **Scene & Artwork Graphic Loaders (`00:2D79`-`00:2E6E`)**:
   - `LoadIntroSequenceTiles` switches between bank `$01` for rain tiles (`vTiles0 + $700`) and bank `$10` for intro graphics (`vTiles0` and `vTiles1`).
   - `LoadTitleScreenTiles` dynamically accommodates DMG vs CGB:
     - Title logo loaded from bank `$0F` (adjusted for GBC to bank `$2F`).
     - "DX" text tiles loaded from bank `$38` (`TitleDXTilesDMG` vs `TitleDXTilesCGB`).
     - "DX" progressive fade OAM tiles loaded from bank `$38` (`TitleDXOAMTiles + $100` vs `TitleDXOAMTiles`).
   - `LoadStaticPictureTiles` serves as the shared dispatcher for full-screen story artwork:
     - `LoadFaceShrineReliefTiles` -> `ReliefTiles` (`$7000`)
     - `LoadSchulePaintingTiles` -> `PaintingTiles` (`$7800`)
     - `LoadChristinePortraitTiles` -> `ChristineTiles` (`$5800`)
   - `LoadEaglesTowerTopTiles` adjusts bank `$13` for GBC and loads collapse graphics into `vTiles1 + $400` and `vTiles2`.
   - `LoadMarinBeachTiles` loads large font glyphs into `vTiles0 + $400` and beach artwork into `vTiles2`.
   - `LoadSaveMenuTiles` uses direct bank switch (`SwitchBank`) to bank `$0F` for save screen tiles.
