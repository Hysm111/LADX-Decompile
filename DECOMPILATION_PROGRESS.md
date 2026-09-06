# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 22.67%
* **Number of Verified Functions**: 272
* **Number of Decompiled Functions**: 272
* **Number Remaining**: ~928 functions
* **Current Subsystem**: Bank 0 - Dialog Subsystem (`code/home/dialog.asm`, `00:2321`+)
* **Current Task**: Decompile and verify Bank 0 dialog routines
* **Last Completed Task**: Decompiled and verified 12 Bank 0 Dialog state machine, opening/closing, and input handlers (`OpenDialogInTable0`, `OpenDialogInTable1`, `OpenDialogInTable2`, `DialogOpenAnimationStartHandler`, `DialogOpenAnimationHandler`, `DialogClosingEndHandler`, `DialogOpenAnimationEndHandler`, `IncrementDialogState` / `IncrementDialogStateAndReturn`, `UpdateDialogState`, `DialogFinishedHandler`, `DialogClosingBeginHandler`, `DialogLetterAnimationStartHandler`) (`00:236B` - `00:24CA`)
* **Next Task**: Decompile and verify next batch in `code/home/dialog.asm` (e.g. `ExecuteDialog` dispatcher, dialog character rendering and scrolling routines)
* **Last Update Timestamp**: 2026-09-06T19:10:00+03:00

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
| `CanBowWowEatEntity` | VERIFIED | PASS | PASS | Checks if entity can be eaten by Bow-Wow via BowWowEatableEntitiesTable in Bank $14 (`00:3925`) |
| `label_3935` | VERIFIED | PASS | PASS | Switches to Bank $19, calls func_019_7c50, and switches to Bank $03 (`00:3935`) |
| `LiftableRockStartSmashingAnimation_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls LiftableRockStartSmashingAnimation, and restores saved bank (`00:3942`) |
| `label_394D` | VERIFIED | PASS | PASS | Switches to Bank $14, calls func_014_54ac, and restores saved bank (`00:394D`) |
| `CreateFollowingNpcEntity_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $01, calls CreateFollowingNpcEntity, and switches to Bank $02 (`00:3958`) |
| `ConfigureNewEntity_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls ConfigureNewEntity, and restores saved bank (`00:3965`) |
| `GetEntityDirectionToLink_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls GetEntityDirectionToLink_03, and restores saved bank (`00:3970`) |
| `label_397B` | VERIFIED | PASS | PASS | Switches to Bank $14, calls func_014_5347, and sets bank to $03 (`00:397B`) |
| `ConfigureEntityHitbox` | VERIFIED | PASS | PASS | Copies 4 bytes from HitboxPositions table ($3AAA) into wEntitiesHitboxPositionTable based on hitbox flags (`00:3AEA`) |
| `SetEntitySpriteVariant` | VERIFIED | PASS | PASS | Writes sprite variant index into wEntitiesSpriteVariantTable for entity (`00:3B0C`) |
| `IncrementEntityState` | VERIFIED | PASS | PASS | Increments state byte in wEntitiesStateTable for entity (`00:3B12`) |
| `HurtBySpikes_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $02, calls HurtBySpikes, and restores saved bank (`00:3B18`) |
| `ApplyEntityInteractionWithBackground_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls ApplyEntityInteractionWithBackground, and restores saved bank (`00:3B23`) |
| `label_3B2E` | VERIFIED | PASS | PASS | Switches to Bank $03, calls ApplySwordIntersectionWithObjects, and restores saved bank (`00:3B2E`) |
| `DefaultEnemyDamageCollisionHandler_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls DefaultEnemyDamageCollisionHandler, and restores saved bank (`00:3B39`) |
| `label_3B44` | VERIFIED | PASS | PASS | Switches to Bank $03, calls func_003_6c6b, and restores saved bank (`00:3B44`) |
| `CheckLinkCollisionWithProjectile_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls CheckLinkCollisionWithProjectile, and restores saved bank (`00:3B4F`) |
| `CheckLinkCollisionWithEnemy_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls CheckLinkCollisionWithEnemy, and restores saved bank (`00:3B5A`) |
| `label_3B65` | VERIFIED | PASS | PASS | Switches to Bank $03, calls func_003_73eb, and restores saved bank (`00:3B65`) |
| `label_3B70` | VERIFIED | PASS | PASS | Switches to Bank $03, calls func_003_6e2b, and restores saved bank (`00:3B70`) |
| `label_3B7B` | VERIFIED | PASS | PASS | Switches to Bank $03, calls func_003_75a2, and restores saved bank (`00:3B7B`) |
| `ApplyVectorTowardsLink_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls ApplyVectorTowardsLink, and restores saved bank (`00:3BAA`) |
| `GetVectorTowardsLink_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls GetVectorTowardsLink, and restores saved bank (`00:3BB5`) |
| `AnimateEntities` | VERIFIED | PASS | PASS | Main entity loop: handles boss agony cry, dialog state, OAM slot cycling, bank $20 routines, and iterates slots 15..0 (`00:398D`) |
| `ResetEntity_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $15, calls ResetEntity, and restores Bank $03 in rSelectROMBank (`00:3A0A`) |
| `AnimateEntity` | VERIFIED | PASS | PASS | Prepares active entity HRAM, handles lifted/raft transition updates, updates timers in bank $14, and dispatches via status jump table in bank $03 (`00:3A18`) |
| `ExecuteActiveEntityHandler_trampoline` | VERIFIED | PASS | PASS | Calls ExecuteActiveEntityHandler and restores Bank $03 (`00:3A81`) |
| `ExecuteActiveEntityHandler` | VERIFIED | PASS | PASS | Reads handler address and bank from EntityHandlersTable ($20:$4000) and jumps to entity code (`00:3A8D`) |
| `ClearEntitySpeed` | VERIFIED | PASS | PASS | Clears X and Y speeds in wEntitiesSpeedXTable and wEntitiesSpeedYTable for entity slot (`00:3D7F`) |
| `CopyEntityPositionToActivePosition` | VERIFIED | PASS | PASS | Copies posX and posY to HRAM, and computes visualPosY = posY - posZ (`00:3D8A`) |
| `RenderActiveEntitySpritesPair` | VERIFIED | PASS | PASS | Renders an 8x16 sprite pair into wDynamicOAMBuffer, applying screen shake, X/Y flip, GBC palette override, and calling Bank $15 helpers (`00:3BC0`) |
| `label_3C71` | VERIFIED | PASS | PASS | Calls func_015_7995 in Bank $15 and restores saved bank via ReloadSavedBank (`00:3C71`) |
| `RenderActiveEntitySprite` | VERIFIED | PASS | PASS | Renders a single 8x16 sprite into wDynamicOAMBuffer, adjusting visual Y for side-scrolling rooms and calling Bank $15 helpers (`00:3C77`) |
| `label_3CD9` | VERIFIED | PASS | PASS | Sets rSelectROMBank to $15 and jumps to label_3C71 (`00:3CD9`) |
| `RenderActiveEntitySpritesRectUsingAllOAM` | VERIFIED | PASS | PASS | Renders a rectangular group of sprites starting at wOAMBuffer ($C000) (`00:3CE0`) |
| `RenderActiveEntitySpritesRect` | VERIFIED | PASS | PASS | Renders a rectangular group of sprites starting at wDynamicOAMBuffer + wOAMNextAvailableSlot (`00:3CE6`) |
| `SkipDisabledEntityDuringRoomTransition` | VERIFIED | PASS | PASS | Checks room transition status, X/Y screen bounds, and sign tables to determine if entity rendering should skip (`00:3D57`) |
| `func_015_7964_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $15, calls func_015_7964, and restores saved bank via ReloadSavedBank (`00:3DA0`) |
| `EntityInitMiniMoldorm_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $04, calls EntityInitMiniMoldorm, and restores saved bank (`00:3DAB`) |
| `EntityInitMoldorm_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $04, calls EntityInitMoldorm, and restores saved bank (`00:3DB6`) |
| `EntityInitFacade_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $04, calls EntityInitFacade, and restores saved bank (`00:3DC1`) |
| `EntityInitSlimeEye_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $04, calls EntityInitSlimeEye, and restores saved bank (`00:3DCC`) |
| `EntityInitGenie_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $36, calls EntityInitGenie, and restores saved bank (`00:3DD7`) |
| `EntityInitSlimeEel_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $05, calls EntityInitSlimeEel, and restores saved bank (`00:3DE2`) |
| `EntityInitDodongoSnake_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $05, calls EntityInitDodongoSnake, and restores saved bank (`00:3DED`) |
| `EntityInitHotHead_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $05, calls EntityInitHotHead, and restores saved bank (`00:3DF8`) |
| `EntityInitEvilEagle_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $05, calls EntityInitEvilEagle, and restores saved bank (`00:3E03`) |
| `Entity67Handler_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $05, calls Entity67Handler, and restores saved bank (`00:3E0E`) |
| `CheckPositionForMapTransition_trampoline` | VERIFIED | PASS | PASS | Preserves wCurrentBank, calls CheckPositionForMapTransition in Bank $02, and restores saved bank (`00:3E19`) |
| `GhiniMovement_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $04, calls GhiniVisibleHandler.move, and restores saved bank (`00:3E29`) |
| `SmashRock_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls SmashRock, and restores saved bank (`00:3E34`) |
| `LoadHeartsAndRupeesCount` | VERIFIED | PASS | PASS | Switches to Bank $02, calls LoadRupeesDigits and LoadHeartsCount, and restores saved bank (`00:3E3F`) |
| `SpawnChestWithItemAndRestoreBank3` | VERIFIED | PASS | PASS | Switches to Bank $02, calls SpawnChestWithItem, and restores Bank $03 (`00:3E4D`) |
| `DrawABButtonSlots` | VERIFIED | PASS | PASS | Switches to Bank $20, calls DrawInventorySlots with parameters for A/B slots, and restores saved bank (`00:3E5A`) |
| `GiveInventoryItem_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $03, calls GiveInventoryItem with player item, and restores saved bank (`00:3E6B`) |
| `func_006_783C_trampoline` | VERIFIED | PASS | PASS | Switches to Bank $06, calls func_006_783C, and restores Bank $03 (`00:3E76`) |
| `UnloadAllEntities` | VERIFIED | PASS | PASS | Clears all 16 entity status entries in wEntitiesStatusTable to 0 (`00:3E83`) |
| `label_3E8E` | VERIFIED | PASS | PASS | Spawns smoke VFX at active entity position when power recoil is active and frame counter matches (`00:3E8E`) |
| `StopEntityRecoilOnCollision` | VERIFIED | PASS | PASS | Detects horizontal/vertical collision during recoil and clears ignore hits countdown (`00:3EAF`) |
| `BossIntro` | VERIFIED | PASS | PASS | Triggers boss/miniboss music and plays intro monologue dialog based on map ID and active entity (`00:3EE8`) |
| `DidKillEnemy` | VERIFIED | PASS | PASS | Drops item via SpawnEnemyDrop, increments kill count, records kill order, updates room cleared flags, and unloads entity (`00:3F50`) |
| `UnloadEntity` | VERIFIED | PASS | PASS | Disables entity by writing ENTITY_STATUS_DISABLED to wEntitiesStatusTable slot (`00:3F8D`) |
| `OpenDialogInTable0` | VERIFIED | PASS | PASS | Resets dialog variables and configures top/bottom state based on Link position (`00:2385`) |
| `OpenDialogInTable1` | VERIFIED | PASS | PASS | Sets up dialog in table 1 ($100-$1FF) via OpenDialogInTable0 (`00:2373`) |
| `OpenDialogInTable2` | VERIFIED | PASS | PASS | Sets up dialog in table 2 ($200-$2FF) via OpenDialogInTable0 (`00:237C`) |
| `DialogOpenAnimationStartHandler` | VERIFIED | PASS | PASS | Dispatches to DialogOpenAnimationStart in Bank $14 (`00:236B`) |
| `DialogOpenAnimationHandler` | VERIFIED | PASS | PASS | Empty animation frame handler returning immediately (`00:23B0`) |
| `DialogClosingEndHandler` | VERIFIED | PASS | PASS | Resets dialog state, applies cooldown, and updates GBC palettes if needed (`00:23B1`) |
| `DialogOpenAnimationEndHandler` | VERIFIED | PASS | PASS | Dispatches to DialogOpenAnimationEnd in Bank $1C (`00:247D`) |
| `IncrementDialogState` | VERIFIED | PASS | PASS | Increments wDialogState (`00:2485`) |
| `UpdateDialogState` | VERIFIED | PASS | PASS | Sets dialog closing state (or exits dialog in photo album mode) and clears frame (`00:2496`) |
| `DialogFinishedHandler` | VERIFIED | PASS | PASS | Advances dialog state to closing when user presses A or B (`00:248A`) |
| `DialogClosingBeginHandler` | VERIFIED | PASS | PASS | Dispatches to AnimateDialogClosing in Bank $1C (`00:24AF`) |
| `DialogLetterAnimationStartHandler` | VERIFIED | PASS | PASS | Decrements scroll delay or invokes ClearLetterPixels in Bank $1C and advances state (`00:24B7`) |

---

## Technical Notes & Implementation Details

1. **Dialog State Machine & Initialization (`00:236B`-`00:24CA`)**:
   - `OpenDialogInTable0`: tests `hLinkPositionY < 0x48`. If true, sets `DIALOG_BOX_BOTTOM_FLAG | DIALOG_OPENING_1` (`0x81`), placing the box at bottom; otherwise `DIALOG_OPENING_1` (`0x01`). Resets ask selection, character index, name index, and sets `wDialogSFX` to `$0F`.
   - `OpenDialogInTable1` & `OpenDialogInTable2`: delegates to `OpenDialogInTable0` and sets `wDialogIndexHi` to `1` or `2`.
   - `DialogFinishedHandler`: checks `wDialogInteractionLocked`. If unlocked and A or B button is pressed, calls `UpdateDialogState`.
   - `UpdateDialogState`: clears `wDialogOpenCloseAnimationFrame`. In `GAMEPLAY_PHOTO_ALBUM` mode, immediately closes dialog (`wDialogState = 0`); otherwise preserves bottom flag in high nibble and sets state to `DIALOG_CLOSING_1` (`0x0E`).
   - `DialogClosingEndHandler`: resets `wDialogState = 0`, sets `wDialogCooldown = DIALOG_COOLDOWN` ($18). On CGB in `GAMEPLAY_WORLD` with `wBGPaletteEffectAddress >= 8`, switches to Bank `$21` to execute `func_021_53CF`.
   - `DialogLetterAnimationStartHandler`: counts down `wDialogScrollDelay`; once zero, invokes `ClearLetterPixels` in Bank `$1C` and advances dialog state.

1. **Entity Recoil, Boss Intro, and Kill Enemy Routines (`00:3E8E`-`00:3F92`)**:
   - `label_3E8E`: triggers transient smoke VFX (`TRANSCIENT_VFX_SMOKE`, 8) at entity's X and visual Y coordinates every 4 frames during power recoiling (`wEntitiesPowerRecoilingTable`).
   - `StopEntityRecoilOnCollision`: calculates absolute velocities for X and Y, selects directional collision mask (`0x03` for horizontal dominant, `0x0C` for vertical dominant), and resets `wEntitiesIgnoreHitsCountdownTable` if a collision occurs.
   - `BossIntro`: handles delay countdown, ensures single-fire execution via `wDidBossIntro`, assigns `MUSIC_MINIBOSS` ($50) or `MUSIC_BOSS` ($19), and displays corresponding intro dialogs (`Dialog0DA` for Desert Lanmola, `Dialog026` for Grim Creeper, and index into `BossIntroDialogTable` for standard bosses).
   - `DidKillEnemy`: invokes `SpawnEnemyDrop` in Bank `$03`, increments `wKillCount`, logs `wKillOrder`, marks bit in `wEntitiesClearedRooms` for load order < 8, and unloads the entity.
   - `UnloadEntity` / `UnloadEntityAndReturn`: zeros `wEntitiesStatusTable[bc]` to disable the entity slot. This completes all functions in `code/home/entities.asm`.

1. **Boss & Entity Init Trampolines (`00:3DAB`-`00:3E8D`)**:
   - Boss init trampolines switch into specific ROM banks: Mini-Moldorm, Moldorm, Facade, Slime Eye, and Ghini movement use Bank `$04`; Genie uses Bank `$36`; Slime Eel, Dodongo Snake, Hot Head, Evil Eagle, and Entity 67 use Bank `$05`.
   - `CheckPositionForMapTransition_trampoline` explicitly saves `wCurrentBank` on the stack before switching to Bank `$02` and restores it via `SwitchBank`.
   - `LoadHeartsAndRupeesCount` executes both `LoadRupeesDigits` and `LoadHeartsCount` in Bank `$02` before reloading saved bank.
   - `SpawnChestWithItemAndRestoreBank3` and `func_006_783C_trampoline` ensure Bank `$03` is actively restored upon completion.
   - `DrawABButtonSlots` sets register `c = 1, b = 0, e = 0xFF` and calls `DrawInventorySlots` in Bank `$20`.
   - `UnloadAllEntities` iterates all `MAX_ENTITIES` (16) slots in `wEntitiesStatusTable`, zeroing each slot to disable all entities.

1. **Entity Sprite Rendering Routines (`00:3BC0`-`00:3D57`, `00:3DA0`)**:
   - `RenderActiveEntitySpritesPair`: iterates a 4-byte display list entry per variant (`tile0`, `attr0`, `tile1`, `attr1`), computing horizontal flip adjustments (`+8` or `0`), screen shake offset subtraction, and off-screen Y-coordinate relocation (`$F0`) if tile lower nibble is `$0F`. Also handles GBC palette override (`OAM_GBC_PAL_4`). Calls `func_015_795D` and `func_015_7995` in Bank $15 before restoring saved bank.
   - `RenderActiveEntitySprite`: renders single 8x16 sprite variant, adjusting `hActiveEntityVisualPosY` by `-4` in side-scrolling rooms (`hIsSideScrolling != 0`), applying GBC palette override outside of credits mode, and delegating to Bank $15 helpers.
   - `RenderActiveEntitySpritesRect` / `RenderActiveEntitySpritesRectUsingAllOAM`: processes arbitrary rectangular sprite definitions (`relY`, `relX`, `tile`, `attr`), replacing `$FF` tile index with 0.
   - `SkipDisabledEntityDuringRoomTransition`: skips rendering when transition is active and entity coordinates exceed screen bounds (`[posX - 1] >= 0xC0` or `[visualPosY - 1] >= 0x88`) or have non-zero sign table entries.
   - `func_015_7964_trampoline`: switches to Bank $15, calls `func_015_7964`, and reloads saved bank.

1. **Entity Animation & Dispatch Pipeline (`00:398D`-`00:3A8D`, `00:3D7F`-`00:3D8A`)**:
   - `AnimateEntities`: iterates entity slots in reverse order (`15` down to `0`), setting `wActiveEntityIndex` and checking `wEntitiesStatusTable`. Also decrements `wBossAgonySFXCountdown` triggering `WAVE_SFX_BOSS_DEATH_CRY`, updates `wC111`/`wC1A8` when no dialog is active, computes `wOAMNextAvailableSlot`, and calls Bank $20 helper routines.
   - `AnimateEntity`: loads type, state, and sprite variant into HRAM registers, checks lifted / raft owner conditions to determine whether `UpdateEntityPositionForRoomTransition` runs before or after `CopyEntityPositionToActivePosition`, updates timers in Bank $14, and switches to Bank $03 to dispatch via status jump table.
   - `ExecuteActiveEntityHandler`: reads a 3-byte `far_pointer` (`addr_low`, `addr_high`, `bank`) from `EntityHandlersTable` (`$20:$4000`), sets `wCurrentBank` and `rSelectROMBank`, and executes the entity handler.
   - `ClearEntitySpeed`: sets both speed coordinates to 0 in WRAM.
   - `CopyEntityPositionToActivePosition`: loads `wEntitiesPosXTable` into `hActiveEntityPosX`, `wEntitiesPosYTable` into `hActiveEntityPosY`, and subtracts `wEntitiesPosZTable` from `posY` to store in `hActiveEntityVisualPosY`.

1. **Entities Hitbox & Collision Trampolines (`00:3AEA`-`00:3BBF`)**:
   - `ConfigureEntityHitbox`: masks `wEntitiesHitboxFlagsTable` with `$7C` to select one of 16 4-byte entries from `HitboxPositions` (`$3AAA`), and writes the 4 hitbox coordinate/extent bytes to `wEntitiesHitboxPositionTable + (bc * 4)`.
   - `SetEntitySpriteVariant`: stores variant byte into `wEntitiesSpriteVariantTable + bc`.
   - `IncrementEntityState`: increments `wEntitiesStateTable + bc`.
   - Bank $02 trampoline: `HurtBySpikes_trampoline`.
   - Bank $03 trampolines: `ApplyEntityInteractionWithBackground_trampoline`, `label_3B2E`, `DefaultEnemyDamageCollisionHandler_trampoline`, `label_3B44`, `CheckLinkCollisionWithProjectile_trampoline`, `CheckLinkCollisionWithEnemy_trampoline`, `label_3B65`, `label_3B70`, `label_3B7B`, `ApplyVectorTowardsLink_trampoline`, `GetVectorTowardsLink_trampoline`.

1. **Entities Trampolines & Helpers (`00:3925`-`00:3988`)**:
   - `CanBowWowEatEntity`: selects Bank $14 in `rSelectROMBank`, reads byte at `BowWowEatableEntitiesTable` (`$5218 + entity_type`), resets `rSelectROMBank` to `$05`, and returns the byte.
   - `label_3935`: switches to Bank $19 via `SwitchBank`, calls `func_019_7c50`, and switches to Bank $03 via `SwitchBank`.
   - `LiftableRockStartSmashingAnimation_trampoline`: switches `rSelectROMBank` to `$03`, calls `LiftableRockStartSmashingAnimation`, and restores `wCurrentBank` via `ReloadSavedBank`.
   - `label_394D`: switches `rSelectROMBank` to `$14`, calls `func_014_54ac`, and restores `wCurrentBank` via `ReloadSavedBank`.
   - `CreateFollowingNpcEntity_trampoline`: switches to Bank $01 via `SwitchBank`, calls `CreateFollowingNpcEntity`, and switches to Bank $02 via `SwitchBank`.
   - `ConfigureNewEntity_trampoline`: switches `rSelectROMBank` to `$03`, calls `ConfigureNewEntity`, and restores `wCurrentBank` via `ReloadSavedBank`.
   - `GetEntityDirectionToLink_trampoline`: switches `rSelectROMBank` to `$03`, calls `GetEntityDirectionToLink_03`, and restores `wCurrentBank` via `ReloadSavedBank`.
   - `label_397B`: switches `rSelectROMBank` to `$14`, calls `func_014_5347`, and sets `rSelectROMBank` to `$03`.

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
