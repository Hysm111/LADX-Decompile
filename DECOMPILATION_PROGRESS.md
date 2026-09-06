# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 16.50%
* **Number of Verified Functions**: 198
* **Number of Decompiled Functions**: 198
* **Number Remaining**: ~1002 functions
* **Current Subsystem**: Bank 0 - Entities (`code/home/entities.asm`, `00:3925`+)
* **Current Task**: Begin decompilation of `code/home/entities.asm` (`CanBowWowEatEntity` at `00:3925`)
* **Last Completed Task**: Decompiled and verified Bank 0 `LoadRoom` (`00:30F4`), `PadRoomObjectsArea` (`01:6CCE`), `LoadCreditsMarinPortraitTiles_trampoline` (`00:3915`), and `LoadThanksForPlayingTiles_trampoline` (`00:391D`)
* **Next Task**: Decompile and verify Bank 0 `CanBowWowEatEntity` (`00:3925`) in `code/home/entities.asm`
* **Last Update Timestamp**: 2026-09-06T17:25:00+03:00

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
| `WriteOverworldObjectToBG` | VERIFIED | PASS | PASS | Reads object attribute byte from WRAM2 and copies tiles/palettes to BG (`00:300E`) |
| `WriteIndoorObjectToBG` | VERIFIED | PASS | PASS | Reads indoor object byte and copies tiles/palettes to BG (`00:3018`) |
| `doCopyObjectToBG` | VERIFIED | PASS | PASS | Copies 2x2 tiles and attributes for an object to BG map with 32-tile row stride (`00:3019`) |
| `LoadRoomTilemap` | VERIFIED | PASS | PASS | Loops 128 times copying room objects to BG map with 10-object and 20-tile row strides, calls UpdateMinimapEntranceArrowAndReturn (`00:309B`) |
| `FillRoomWithConsecutiveObjects` | VERIFIED | PASS | PASS | Writes consecutive objects into wRoomObjects horizontally or vertically (+16 with hMultiPurpose0 bit 6) (`00:34EF`) |
| `SetupDestroyableObjectIfNeeded2` | VERIFIED | PASS | PASS | Evaluates overworld destroyable objects (grass/rocky ground/cave door) and backs up to WRAM2 with bank 9 or 1A (`00:3500`) |
| `CopyObjectToActiveRoomMap` | VERIFIED | PASS | PASS | Writes single room object to wRoomObjects + pos and calls SetupDestroyableObjectIfNeeded2 (`00:352D`) |
| `SetBankForRoom` | VERIFIED | PASS | PASS | Sets rSelectROMBank to BANK(OverworldRoomsFirstHalf) ($09) or OverworldRoomsSecondHalf ($1A) based on hMapRoom (`00:353B`) |
| `CopyIndoorsMacroObjectsToRoom` | VERIFIED | PASS | PASS | Unpacks indoor macro objects list, registers warp doors to wWarpPositions, and backs up to WRAM2 (`00:354B`) |
| `CopyOutdoorsMacroObjectsToRoom` | VERIFIED | PASS | PASS | Unpacks outdoor macro objects list, registers warp doors to wWarpPositions, and backs up to WRAM2 with bank $24 (`00:358B`) |
| `SetupDestroyableObjectIfNeeded` | VERIFIED | PASS | PASS | Evaluates macro destroyable objects and backs up to WRAM2 with fixed return bank $24 (`00:35CB`) |
| `ObjectPositionToRoomObjectAddress` | VERIFIED | PASS | PASS | Translates room object position byte to pointer within wRoomObjects (0xD711 + pos) (`00:35EE`) |
| `FillRoomMapWithObject` | VERIFIED | PASS | PASS | Fills active 10x8 room map in wRoomObjects with object type, skipping row padding/borders (`00:37E7`) |
| `LoadRoomTemplate_trampoline` | VERIFIED | PASS | PASS | Switches to bank $14, invokes LoadRoomTemplate, and restores bank from hRoomBank (`00:38EA`) |
| `LoadWorldMapBGMap_trampoline` | VERIFIED | PASS | PASS | Switches to bank $20 and invokes LoadWorldMapBGMap (`00:38FC`) |
| `MakeListOfDoorPositions` | VERIFIED | PASS | PASS | Records door position and extracted Y/X coordinates in wDoorPositions/Y/X (`00:373F`) |
| `UpdateIndoorRoomStatus` | VERIFIED | PASS | PASS | ORs status flags with wIndoorARoomStatus/wIndoorBRoomStatus/wColorDungeonRoomStatus and hRoomStatus (`00:36C4`) |
| `LoadObject_KeyDoorTop` | VERIFIED | PASS | PASS | Loads top key door tiles ($2D,$2E) or opens if already unlocked (`00:35FA`) |
| `LoadObject_KeyDoorBottom` | VERIFIED | PASS | PASS | Loads bottom key door tiles ($2F,$30) or opens if already unlocked (`00:3615`) |
| `LoadObject_KeyDoorLeft` | VERIFIED | PASS | PASS | Loads left key door tiles ($31,$32) or opens if already unlocked (`00:3630`) |
| `LoadObject_KeyDoorRight` | VERIFIED | PASS | PASS | Loads right key door tiles ($33,$34) or opens if already unlocked (`00:364B`) |
| `LoadObject_ShutterDoorTop` | VERIFIED | PASS | PASS | Sets shutter top bit in wShutterDoorsMask/2 and opens door (`00:3664`) |
| `LoadObject_ShutterDoorBottom` | VERIFIED | PASS | PASS | Sets shutter bottom bit in wShutterDoorsMask/2 and opens door (`00:3677`) |
| `LoadObject_ShutterDoorLeft` | VERIFIED | PASS | PASS | Sets shutter left bit in wShutterDoorsMask/2 and opens door (`00:368A`) |
| `LoadObject_ShutterDoorRight` | VERIFIED | PASS | PASS | Sets shutter right bit in wShutterDoorsMask/2 and opens door (`00:369D`) |
| `LoadObject_OpenDoorTop` | VERIFIED | PASS | PASS | Updates room status with open up and copies horizontal open door tiles ($43,$44) (`00:36B2`) |
| `LoadObject_OpenDoorBottom` | VERIFIED | PASS | PASS | Updates room status with open down and copies horizontal open door tiles ($8C,$08) (`00:36EA`) |
| `LoadObject_OpenDoorLeft` | VERIFIED | PASS | PASS | Updates room status with open left and copies vertical open door tiles ($09,$0A) (`00:36FE`) |
| `LoadObject_OpenDoorRight` | VERIFIED | PASS | PASS | Updates room status with open right and copies vertical open door tiles ($0B,$0C) (`00:3712`) |
| `LoadObject_BossDoor` | VERIFIED | PASS | PASS | Loads boss door tiles ($A4,$A5) or opens if room status visited/open (`00:3726`) |
| `LoadObject_StairsDoor` | VERIFIED | PASS | PASS | Copies vertical stairs door tiles ($AF,$B0) into wRoomObjects (`00:375E`) |
| `LoadObject_RevolvingDoor` | VERIFIED | PASS | PASS | Copies horizontal revolving wall tiles ($B1,$B2) into wRoomObjects (`00:376D`) |
| `LoadObject_OneWayArrow` | VERIFIED | PASS | PASS | Copies horizontal one-way arrow tiles ($45,$46) into wRoomObjects (`00:377C`) |
| `LoadObject_DungeonEntrance` | VERIFIED | PASS | PASS | Updates room status down and unpacks 4x3 entrance macro tiles ($B3-$BD) (`00:37A2`) |
| `LoadObject_IndoorEntrance` | VERIFIED | PASS | PASS | Loads indoor entrance ($C1,$C2) or shutter bottom if thief in Kanalet Castle ($D3) (`00:37B6`) |
| `DispatchIndoorDoorObject` | VERIFIED | PASS | PASS | Jump table dispatch for door objects $EC-$FD to respective handlers (`00:32DF`) |
| `ExpandOverworldObjectMacro` | VERIFIED | PASS | PASS | Bank $24 trampoline for expanding overworld macro objects ($F5-$FD) (`24:7578`) |
| `LoadRoomObject` | VERIFIED | PASS | PASS | Parses 2/3-byte room objects, applies overworld/indoor logic, registers stairs/warps, and writes to map (`00:32A9`) |
| `PadRoomObjectsArea` | VERIFIED | PASS | PASS | Surrounds room objects area ($D700) with ROOM_BORDER ($FF) along the 10x8 perimeter (`01:6CCE`) |
| `LoadRoom` | VERIFIED | PASS | PASS | Main room loading routine: parses headers, warps, templates, and dispatches objects (`00:30F4`) |
| `LoadCreditsMarinPortraitTiles_trampoline` | VERIFIED | PASS | PASS | Switches to bank $27 and jumps to LoadCreditsMarinPortraitTiles (`00:3915`) |
| `LoadThanksForPlayingTiles_trampoline` | VERIFIED | PASS | PASS | Switches to bank $20 and jumps to LoadThanksForPlayingTiles (`00:391D`) |

---

## Technical Notes & Implementation Details

1. **Room Loading & Perimeter Subroutines (`00:30F4`-`00:32A6`, `01:6CCE`, `00:3915`-`00:391D`)**:
   - `LoadRoom`:
     - Disables interrupts except VBlank (`rIE = IEF_VBLANK`).
     - Increments `wD47F` counter.
     - Calls `ResetRoomVariables` in Bank $20.
     - In CGB mode (`hIsGBC != 0`), invokes `LoadRoomPalettes` (Bank $21) and `LoadRoomObjectsAttributes` (Bank $20).
     - For indoor rooms, calls `func_014_5897` (Bank $14) and resets `wKillCount` (`$DBB5`) and `wKillOrder` array (17 bytes cleared to 0).
     - Marks room visited in `wOverworldRoomStatus`, `wIndoorARoomStatus`, `wIndoorBRoomStatus`, or `wColorDungeonRoomStatus` if top-down (`hIsSideScrolling == 0`). Preserves visited status if side-scrolling. Updates `hRoomStatus`.
     - Selects room pointer table:
       - Overworld alternate rooms checked if `OW_ROOM_STATUS_CHANGED` set: Eagle's Tower (`Overworld0EAlt`), Face Shrine (`Overworld8CAlt`), Kanalet Gate (`Overworld79Alt`), Unknown 06 (`Overworld06Alt`), Unknown 1B (`Overworld1BAlt`), Angler's Tunnel Entrance (`Overworld2BAlt`).
       - Indoor alternate room: Goriya cave (`MAP_CAVE_WATER`, `$F5`) with `wTradeSequenceItem == TRADING_ITEM_MAGNIFYING_LENS` loads `IndoorsAF5Alt`.
       - Color Dungeon uses `ColorDungeonRoomPointers` (`$7B77`).
       - Overworld rooms >= `$80` switch to Bank $1A (`BANK_OverworldRoomsSecondHalf`).
     - Parses header byte 0 (`hAnimatedTilesGroup`). If `ROOM_END` (`$FE`), jumps directly to `.endOfRoom`.
     - Parses header byte 1: fills room floor tile via `FillRoomMapWithObject`, and invokes `LoadRoomTemplate` (Bank $14) if indoors.
     - Loops parsing room objects: unpacks 5-byte warp descriptors into `wWarpStructs + hFreeWarpDataAddress`, or dispatches objects to `LoadRoomObject`.
     - At room end, invokes `PadRoomObjectsArea` (Bank $01), post-load hooks in Bank $36 (`func_036_6D4D`) and Bank $21 (`func_021_53F3`), and restores `wCurrentBank` via `ReloadSavedBank`.
   - `PadRoomObjectsArea`: fills 40 coordinates defining the 10x8 room border in `wRoomObjectsArea` (`$D700`) with `ROOM_BORDER` (`$FF`).
   - `LoadCreditsMarinPortraitTiles_trampoline`: switches to Bank $27 and invokes `LoadCreditsMarinPortraitTiles`.
   - `LoadThanksForPlayingTiles_trampoline`: switches to Bank $20 and invokes `LoadThanksForPlayingTiles`.

1. **Room Objects & Macro Processing Subroutines (`00:34EF`-`00:38FC`)**:
   - `FillRoomWithConsecutiveObjects`: loops `count` times writing `obj_type` to `hl`. Advances `hl` by 1 horizontally, or by 16 (`+ 0x0F` after `ldi`) if `hMultiPurpose0` bit 6 is set (vertical span).
   - `SetupDestroyableObjectIfNeeded2`: filters out `OBJECT_SHORT_GRASS` (`$04`). For `OBJECT_ROCKY_GROUND` (`$09`), only backs up if room is `ROOM_OW_GIANT_SKULL` (`$97`) to bank `$1A`. For `OBJECT_ROCKY_CAVE_DOOR` (`$E1`), ignores Eagle's Tower (`$0E`), room `$0C`, and room `$1B`. Otherwise, selects bank `$1A` (if `hMapRoom >= $80`) or bank `$09` (if `hMapRoom < $80`) and calls `BackupObjectInRAM2`.
   - `CopyObjectToActiveRoomMap`: writes object type to `wRoomObjects + pos` and triggers `SetupDestroyableObjectIfNeeded2`.
   - `SetBankForRoom`: switches `rSelectROMBank` to `BANK(OverworldRoomsFirstHalf)` (`$09`) if `hMapRoom < $80` or `BANK(OverworldRoomsSecondHalf)` (`$1A`) if `hMapRoom >= $80`. Returns selected bank.
   - `CopyIndoorsMacroObjectsToRoom` & `CopyOutdoorsMacroObjectsToRoom`: iterates over an `$FF`-terminated offset list, computing `target_hl = hl + offset` and reading each object ID. For door objects (`$E1`, `$E2`, `$E3`), records tile index `(target_hl & 0xFF) - 0x11` into `wWarpPositions` ring buffer indexed by `wC19C` (incremented mod 4). Calls `SetupDestroyableObjectIfNeeded2` (bank 9/1A) or `SetupDestroyableObjectIfNeeded` (fixed bank `$24`).
   - `SetupDestroyableObjectIfNeeded`: identical ignore filtering to `SetupDestroyableObjectIfNeeded2`, but invokes `BackupObjectInRAM2` with return bank `$24` (macro bank context).
   - `ObjectPositionToRoomObjectAddress`: returns `wRoomObjects + pos`.
   - `FillRoomMapWithObject`: stores object type in `hMultiPurposeH` and iterates 128 times starting at `wRoomObjects` (`$D711`). Only writes to active room columns (`1 <= (hl & 0x0F) <= 10`), preserving the 6 border/padding columns per row.
   - `LoadRoomTemplate_trampoline`: farcalls `LoadRoomTemplate` in bank `$14` and restores bank from `hRoomBank`.
   - `LoadWorldMapBGMap_trampoline`: switches to bank `$20` and calls `LoadWorldMapBGMap`.

1. **Room Tilemap Loading & CGB Object Copying (`00:300E`-`00:30F3`)**:
   - `WriteOverworldObjectToBG`: switches WRAM bank to 2 via `rSVBK` to read the object attribute value from `wRoomObjects`, restores WRAM bank to 1 (`rSVBK = 0`), and jumps to `doCopyObjectToBG`.
   - `WriteIndoorObjectToBG`: reads the object attribute value directly from current WRAM bank and falls through to `doCopyObjectToBG`.
   - `doCopyObjectToBG`: scales attribute value by 4, queries BG attributes via `GetBGAttributesAddressForObject` in bank `$1A`, selects the appropriate CGB objects tilemap (`OverworldObjectsTilemapCGB` in bank `$1A`, `ColorDungeonObjectsTilemap` in bank `$08`, or `IndoorObjectsTilemapCGB` in bank `$08`), copies 2 tile indices into `vBGMap0`, then switches `rVBK = 1` and copies 2 attribute bytes from the bank specified in `hMultiPurpose8`. It repeats this for the lower row at offset `+ 0x20` (32 tiles).
   - `LoadRoomTilemap`: sets `rSelectROMBank` and `wCurrentBank` using `SwitchToObjectsTilemapBank` and `SwitchBank`. Initializes `de = vBGMap0`, `hl = wRoomObjects`, `c = $80` (128 objects). In DMG mode, dispatches to `WriteObjectToBG_DMG`; in CGB mode, dispatches to `WriteIndoorObjectToBG` or `WriteOverworldObjectToBG`. Advances `hl` with row wrap at `OBJECTS_PER_ROW + 1` (`hl = (hl & $FFF0) + $11`), and advances `de` by 2 tiles with row wrap at `SCRN_X / TILE_WIDTH` (`de = (de & $FFE0) + $40`). At completion, jumps to `UpdateMinimapEntranceArrowAndReturn` in bank 1.

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

2. **Door Objects & Parsing Subroutines (`00:35FA`-`00:37E0`, `00:32DF`-`00:3303`, `00:32A9`-`00:34EE`)**:
   - `MakeListOfDoorPositions`: writes `pos` to `wDoorPositions[door_type]`, `pos & 0xF0` to `wDoorYPositions`, and `((pos & 0x0F) << 4)` to `wDoorXPositions`.
   - `UpdateIndoorRoomStatus`: resolves status RAM location for Map Indoors A (`$D900 + room`), Map Indoors B (`$DA00 + room`), or Color Dungeon (`$DDE0 + room`), ORs `new_status`, and writes back to both RAM and `hRoomStatus`.
   - `LoadObject_OpenDoor*`: marks room status bit (`ROOM_STATUS_DOOR_OPEN_*`) and copies corresponding open door tiles (top: `$43,$44`; bottom: `$8C,$08`; left: `$09,$0A`; right: `$0B,$0C`).
   - `LoadObject_KeyDoor*`: records door position; if already unlocked in `hRoomStatus`, delegates to `LoadObject_OpenDoor*`, otherwise copies closed key door tiles (`$2D-$34`).
   - `LoadObject_ShutterDoor*`: records door position, sets corresponding shutter bit in `wShutterDoorsMask` and `wShutterDoorsMask2`, and opens door.
   - `LoadObject_BossDoor`: records door position, delegates to open door if visited, otherwise loads boss door tiles (`$A4,$A5`).
   - `LoadObject_StairsDoor` / `LoadObject_RevolvingDoor` / `LoadObject_OneWayArrow`: copies 2-tile macro structures into `wRoomObjects`.
   - `LoadObject_DungeonEntrance`: sets open down status and copies 12-tile macro entrance block (`$B3-$BD`).
   - `LoadObject_IndoorEntrance`: checks for Kanalet Castle main entrance (`$D3`) with shoplifting flag set; if true, converts entrance to `LoadObject_ShutterDoorBottom`. Otherwise marks open status and copies entrance tiles (`$C1,$C2`).
   - `DispatchIndoorDoorObject`: parses object type range `$EC`-`$FD` and dispatches to door routines via jump table.
   - `LoadRoomObject`: parses 2-byte (single block) or 3-byte (horizontal/vertical span with length in nybble) objects. Evaluates dynamic overworld state (waterfall, weather vane, monkey bridge, gates, bombable cave doors, cut bushes over stairs) or indoor interactive objects (conveyors, unlit torches, switches, movable blocks, bombable walls, chests, hidden stairs) before placing tiles into `wRoomObjects`.
