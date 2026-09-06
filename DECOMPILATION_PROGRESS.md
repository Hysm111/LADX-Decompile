# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 34.58%
* **Number of Verified Functions**: 415
* **Number of Decompiled Functions**: 415
* **Number Remaining**: ~785 functions
* **Current Subsystem**: Bank 1 - Room Transition & World Initialization (`code/bank1.asm`, `01:5E67`-`01:6202`)
* **Current Task**: Bank 1 Room Transition & World Initialization block completed
* **Last Completed Task**: Decompiled and verified 5 Bank 1 room transition, NPC following & initialization routines (`SynchronizeDungeonsItemFlags`, `CreateFollowingNpcEntity`, `func_001_6162`, `LoadCounterAnimatedTiles`, `OpenDungeonNameDialog`) (`01:5E67`-`01:6202`)
* **Next Task**: Decompile and verify Bank 1 Marin Beach sequence handlers (`code/marin_beach.asm`)
* **Last Update Timestamp**: 2026-09-07T01:05:00+03:00

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
| `ExecuteDialog` | VERIFIED | PASS | PASS | Dialog state machine dispatcher: sets BG tile, wraps next char position, and dispatches to handler (`00:2321`) |
| `func_23E4` | VERIFIED | PASS | PASS | Backs up 18x2 BG map tiles under dialog box to wD500 buffer during dialog opening on DMG & CGB (`00:23E4`) |
| `DialogScrollingStartHandler` | VERIFIED | PASS | PASS | Scrolling start handler stub returning immediately (`00:2714`) |
| `DialogScrollingEndHandler` | VERIFIED | PASS | PASS | Scrolling end handler stub returning immediately (`00:2768`) |
| `SkipDialog` | VERIFIED | PASS | PASS | Sets wDialogAskSelectionIndex to 2 and transitions dialog to closing state (`00:278B`) |
| `DialogChoiceHandler` | VERIFIED | PASS | PASS | Handles choice prompt input: toggles selection on Left/Right, draws cursor marker, advances on A (`00:2793`) |
| `DrawDialogArrowTrampoline` | VERIFIED | PASS | PASS | Switches to Bank $17 and invokes DrawDialogArrow ($7D7C) (`00:27BB`) |
| `DialogBeginScrolling` | VERIFIED | PASS | PASS | Scrolls dialog text up 1 line, clears new line with BG tile, pauses 8 frames, advances state (`00:2719`) |
| `DialogFinishScrolling` | VERIFIED | PASS | PASS | Finalizes dialog scrolling, clears first line, and resets state to DIALOG_LETTER_IN_1 (`00:276D`) |
| `DialogBreakHandler` | VERIFIED | PASS | PASS | Handles pause between lines; draws prompt arrow, checks A/B skip, builds fill draw command (`00:2695`) |
| `DialogLetterAnimationEndHandler` | VERIFIED | PASS | PASS | Prepares character tile placement draw command and dispatches to DialogDrawNextCharacterHandler (`00:24CD`) |
| `DialogDrawNextCharacterHandler` | VERIFIED | PASS | PASS | Reads dialog character, handles @/<ask>/# player name replacement, copies font tile to draw command (`00:2529`) |
| `ExecuteGameplayHandler` | VERIFIED | PASS | PASS | Master gameplay handler evaluating save screen trigger and dispatching to active mode (`00:0E34`) |
| `jumpToGameplayHandler` | VERIFIED | PASS | PASS | Jump table dispatcher for all 27 gameplay states (intro, credits, files, maps, pictures, world) (`00:0E85`) |
| `IntroHandler` | VERIFIED | PASS | PASS | Dispatches to opening intro sequence entry point (`00:0EDF`) |
| `EndCreditsHandler` | VERIFIED | PASS | PASS | Switches to Bank $17 and invokes EndCreditsEntryPoint with common gameplay return (`00:0EE2`) |
| `FileSelectionHandler` | VERIFIED | PASS | PASS | Dispatches to file selection menu entry point (`00:0F0E`) |
| `FileCreationHandler` | VERIFIED | PASS | PASS | Dispatches to file creation / player naming entry point (`00:0F11`) |
| `FileDeletionHandler` | VERIFIED | PASS | PASS | Dispatches to file deletion menu entry point (`00:0F14`) |
| `FileCopyHandler` | VERIFIED | PASS | PASS | Dispatches to file copy menu entry point (`00:0F17`) |
| `FileSaveHandler` | VERIFIED | PASS | PASS | Switches to Bank $01 and dispatches to save dialog screen entry point (`00:0ED7`) |
| `WorldMapHandler` | VERIFIED | PASS | PASS | Dispatches to full Koholint world map overview entry point with common gameplay return (`00:0ED1`) |
| `PeachPictureHandler` | VERIFIED | PASS | PASS | Dispatches to Peach letter photo viewer entry point with common gameplay return (`00:0EC5`) |
| `MarinBeachHandler` | VERIFIED | PASS | PASS | Dispatches to Marin beach cutscene entry point with common gameplay return (`00:0ECB`) |
| `FaceShrineMuralHandler` | VERIFIED | PASS | PASS | Dispatches to Face Shrine mural relief entry point with common gameplay return (`00:0EBF`) |
| `WorldHandler` | VERIFIED | PASS | PASS | Updates interactive palette effects in Bank $14, performs overworld audio, and invokes WorldHandlerEntryPoint in Bank $01 (`00:0F1A`) |
| `InventoryHandler` | VERIFIED | PASS | PASS | Switches to Bank $20 and dispatches to subscreen inventory entry point (`00:0F2D`) |
| `PhotoAlbumHandler` | VERIFIED | PASS | PASS | Switches to Bank $28 and dispatches to photo album entry point with common gameplay return (`00:0F35`) |
| `PhotoPictureHandler` | VERIFIED | PASS | PASS | Switches to Bank $37 and dispatches to photo fullscreen display entry point (`00:0F40`) |
| `LinkMotionTeleportUpHandler` | VERIFIED | PASS | PASS | Switches to Bank $19 and dispatches to teleport up animation (`00:1155`) |
| `LinkMotionPassOutHandler` | VERIFIED | PASS | PASS | Switches to Bank $01 and dispatches to LinkPassOut sequence (`00:115D`) |
| `LinkMotionDefaultHandler` | VERIFIED | PASS | PASS | Checks IsInteractiveMotionAllowed in Bank $36, switches to Bank $02, and invokes LinkMotionDefault (`00:1165`) |
| `CheckItemsToUse` | VERIFIED | PASS | PASS | Master inventory item trigger evaluator: checks blocks, boots running, shield, items A/B, sword hold, and color dungeon (`00:1177`) |
| `UseItem` | VERIFIED | PASS | PASS | Inventory item action dispatcher for sword, shield, bombs, bracelet, bow, boomerang, hookshot, feather, ocarina, powder, shovel, magic rod (`00:129C`) |
| `UseMagicRod` | VERIFIED | PASS | PASS | Validates attack/sword state and projectile count, starts magic rod attack animation countdown (`00:12D8`) |
| `UseShield` | VERIFIED | PASS | PASS | Checks pushing state and plays shield draw noise SFX (`00:12EE`) |
| `UseShovel` | VERIFIED | PASS | PASS | Checks air/shovel state, tests surface collision for sword poking or shovel dig SFX, and initiates shovel timer (`00:12F8`) |
| `UseHookshot` | VERIFIED | PASS | PASS | Checks active hookshot state and dispatches to FireHookshot (`00:1319`) |
| `HoldSwordIfNeeded` | VERIFIED | PASS | PASS | Sets sword holding animation state and enables sword collision when button is held outside NPC/text dialog (`00:1321`) |
| `SetShieldVals` | VERIFIED | PASS | PASS | Sets wIsUsingShield and wHasMirrorShield, and synchronizes shield attributes via Bank $20 trampoline (`00:1340`) |
| `PlaceBomb` | VERIFIED | PASS | PASS | Checks placed bomb count, decrements bomb BCD count, spawns bomb projectile, and checks bomb-arrow conversion (`00:135A`) |
| `UsePowerBracelet` | VERIFIED | PASS | PASS | Power bracelet item handler stub (`00:1382`) |
| `UseBoomerang` | VERIFIED | PASS | PASS | Validates projectile limits, spawns boomerang projectile, and syncs via Bank $20 helper (`00:1383`) |
| `ShootArrow` | VERIFIED | PASS | PASS | Arrow shooting action: BCD ammo decrement, directional projectile spawn, bomb-arrow combo conversion, and speed setup (`00:13BD`) |
| `SpawnPlayerProjectile` | VERIFIED | PASS | PASS | Spawns projectile entity, initializes position, Z height, speed tables, direction, and variant from player state (`00:142F`) |
| `UseMagicPowder` | VERIFIED | PASS | PASS | Handles toadstool trade dialog check, powder count validation, and magic powder sprinkle spawning (`00:148D`) |
| `UseRocsFeather` | VERIFIED | PASS | PASS | Jump mechanics: initializes Z velocity, plays jump jingle, applies lateral boost when running with Pegasus boots, handles side-scrolling jump physics (`00:14CB`) |
| `UseSword` | VERIFIED | PASS | PASS | Sword attack action: initializes swing animation, selects random swing SFX, clears spin attack, and spawns full-health sword beam on L2 sword (`00:1528`) |
| `UpdateLinkDirectionFromJoypad` | VERIFIED | PASS | PASS | Converts dpad bitmask from hPressedButtonsMask into cardinal link direction via JoypadToLinkDirection table (`00:157C`) |
| `CheckStaticSwordCollision_trampoline` | VERIFIED | PASS | PASS | Evaluates static sword collision with environment and floor items, switching to Bank $02 (`00:15A7`) |
| `CheckStaticSwordCollision` | VERIFIED | PASS | PASS | Maps sword tip coordinates to room grid, evaluates object physics flags, cuts bushes/grass, reveals hidden objects, triggers rock smashing VFX, and spawns heart/rupee drops (`00:15AF`) |
| `CheckItemsSwordCollision` | VERIFIED | PASS | PASS | Evaluates sword impact with solid surfaces or ground items, sets Moblin alerting counter, triggers clink or sword poke sound effects (`00:16C2`) |
| `UsePegasusBoots` | VERIFIED | PASS | PASS | Pegasus boots charge state machine: checks airborne/side-scrolling constraints, steps counter increment, dust VFX, and full-speed sprint initialization (`00:1705`) |
| `DisplayTransientVfxForLinkRunning` | VERIFIED | PASS | PASS | Periodically emits ground dust or shallow water splash transient VFX while running on 8-frame cycles (`00:1756`) |
| `ClearLinkPositionIncrement` | VERIFIED | PASS | PASS | Resets Link's horizontal and vertical speed increments to zero (`00:178E`) |
| `ApplyLinkMotionState` | VERIFIED | PASS | PASS | Coordinates Link attack step / sword charging flash, and fires magic rod fireball projectile when cooldown matches (`00:1794`) |
| `func_1819` | VERIFIED | PASS | PASS | Farcalls bank $20 helper $4AB3 with direct bank select and reloads saved bank (`00:1819`) |
| `func_1828` | VERIFIED | PASS | PASS | Farcalls bank $20 helper $49BA with direct bank select and reloads saved bank (`00:1828`) |
| `LinkMotionMapFadeOutHandler` | VERIFIED | PASS | PASS | Link motion handler for map fade-out: clears active entities, handles shop theft flag, matches outdoor/indoor warp targets, sets room parameters and records spawn points (`00:1837`) |
| `SetSpawnLocation` | VERIFIED | PASS | PASS | Records 5 bytes of warp coordinate and target data plus indoor room index into wSpawnLocationData (`00:19C2`) |
| `label_19DA` | VERIFIED | PASS | PASS | Resets Link's facing direction to DIRECTION_RIGHT (0) (`00:19DA`) |
| `LinkMotionMapFadeInHandler` | VERIFIED | PASS | PASS | Link motion handler for map fade-in: manages Manbo warp transition, resets motion state, and triggers shopkeeper confrontation dialog if items were stolen (`00:19DE`) |
| `func_1A22` | VERIFIED | PASS | PASS | Farcalls bank $20 helper $6C4F and FadeOutMusic with direct bank select, restoring saved ROM bank (`00:1A22`) |
| `func_1A39` | VERIFIED | PASS | PASS | Farcalls bank $20 helper $6C7A and helper $563B with direct bank select, restoring saved ROM bank (`00:1A39`) |
| `UpdateLinkWalkingAnimation` | VERIFIED | PASS | PASS | Updates hLinkAnimationState from direction, step cadence, equipped shield tier, active shield use, side-scrolling, pushing, lifting, and swimming states (`00:1A50`) |
| `AnimateMarinBeachTiles` | VERIFIED | PASS | PASS | Copies Marin beach dialogue wave animation tiles to vTiles2+$500 on 8-frame cadence when no draw command is active (`00:1ACC`) |
| `AnimateCounterTilesGroup` | VERIFIED | PASS | PASS | Calls bank $01 LoadCounterAnimatedTiles on 8-frame cadence and reloads bank $0C (`00:1BF9`) |
| `LoadAnimatedTilesFrameAtOffset` | VERIFIED | PASS | PASS | Loads 64-byte tile data from specified group high byte and offset into vTiles2+$6C0 (`00:1C13`) |
| `AnimateTideTilesGroup` | VERIFIED | PASS | PASS | Updates tide animated tiles (group $6B) on slow 16-frame cadence (`00:1C16`) |
| `AnimateVillageTilesGroup` | VERIFIED | PASS | PASS | Updates village animated tiles (group $6C) on slow 16-frame cadence (`00:1C1A`) |
| `AnimateWaterDungeonTilesGroup` | VERIFIED | PASS | PASS | Updates water dungeon animated tiles (group $73) on slow 16-frame cadence (`00:1C1E`) |
| `AnimateSlowWaterfallTilesGroup` | VERIFIED | PASS | PASS | Updates slow waterfall animated tiles (group $6A) on slow 16-frame cadence (`00:1C22`) |
| `AnimateTilesSlowSpeed` | VERIFIED | PASS | PASS | Executes slow animated tile updates on (hAnimatedTilesFrameCount & 0x0F) == 0, advancing offset by $40 (`00:1C24`) |
| `AnimateDungeon1TilesGroup` | VERIFIED | PASS | PASS | Updates Dungeon 1 water/torch animation on 8-frame cadence using AnimatedTilesDataOffsets table (`00:1C39`) |
| `LoadAnimatedTilesFrame` | VERIFIED | PASS | PASS | Copies 64 bytes of animated tile data to VRAM with special dual-copy handling for Color Dungeon ($FF) (`00:1C51`) |
| `AnimateUndergroundTilesGroup` | VERIFIED | PASS | PASS | Updates underground/cave animated tiles (group $6E) on slow 16-frame cadence (`00:1C8A`) |
| `AnimateLavaTilesGroup` | VERIFIED | PASS | PASS | Updates lava animated tiles (group $6F) on 8-frame cadence using AnimatedTilesDataOffsets table (`00:1C8E`) |
| `AnimateDungeon2TilesGroup` | VERIFIED | PASS | PASS | Updates Dungeon 2 animated scrolling tiles from wAnimatedScrollingTilesStorage, branching for Color Dungeon ($8400) (`00:1CA9`) |
| `label_1CB8` | VERIFIED | PASS | PASS | Updates scrolling tiles to $90C0 on 4-frame cadence or falls back to AnimateDungeon1TilesGroup (`00:1CB8`) |
| `AnimateWarpTilesGroup` | VERIFIED | PASS | PASS | Updates warp tile animated graphics (group $70) on medium 8-frame cadence (`00:1CC6`) |
| `AnimateTilesMediumSpeed` | VERIFIED | PASS | PASS | Executes medium-speed tile updates on (hAnimatedTilesFrameCount & 0x07) == 0, advancing offset by $40 (`00:1CC8`) |
| `AnimateWaterCurrentsTilesGroup` | VERIFIED | PASS | PASS | Updates water current tiles (group $71) on fast 4-frame cadence (`00:1CD3`) |
| `AnimateTilesFastSpeed` | VERIFIED | PASS | PASS | Executes fast-speed tile updates on (hAnimatedTilesFrameCount & 0x03) == 0, advancing offset by $40 (`00:1CD6`) |
| `AnimateWaterfallTilesGroup` | VERIFIED | PASS | PASS | Updates fast waterfall tiles (group $72) on fast 4-frame cadence (`00:1CE1`) |
| `IncrementAnimatedTilesDataOffset` | VERIFIED | PASS | PASS | Adds $40 to hAnimatedTilesDataOffset register and returns the updated offset (`00:1CEE`) |
| `AnimateLightBeamTilesGroup` | VERIFIED | PASS | PASS | Updates light beam tiles (group $75) on fast 4-frame cadence (`00:1CF7`) |
| `AnimateBubblesTilesGroup` | VERIFIED | PASS | PASS | Updates bubbles animated tiles (group $74) on medium 8-frame cadence (`00:1CFB`) |
| `AnimateWeatherVaneTilesGroup` | VERIFIED | PASS | PASS | Updates weather vane animated tiles (group $77) on medium 8-frame cadence (`00:1CFF`) |
| `AnimateCrystalBlockTilesGroup` | VERIFIED | PASS | PASS | Updates crystal block animated tiles (group $76) on medium 8-frame cadence (`00:1D03`) |
| `AnimatePhotoTilesGroup` | VERIFIED | PASS | PASS | Dispatches photo album animated graphics handler in bank $38 (`00:1D07`) |
| `CopyLinkTilesPair` | VERIFIED | PASS | PASS | Copies D bytes of Link sprite graphics from BC to HL in bank $0C, then switches to bank $20 (`00:1D0A`) |
| `SkipTilesGroupAnimation` | VERIFIED | PASS | PASS | Bypasses tile group update, calling bank $20 helper $54F5 and setting ROM bank $0C (`00:1D1E`) |
| `DrawLinkSprite` | VERIFIED | PASS | PASS | Generates Link dual-sprite OAM entries during V-Blank with GBC tunic palette and swimming modifiers (`00:1D2E`) |
| `CopyDataAndDrawLinkSprite` | VERIFIED | PASS | PASS | Copies data, resets hReplaceTiles, switches to ROM bank $0C, and draws Link sprite (`00:1F3B`) |
| `Copy4TilesAndDrawLinkSprite` | VERIFIED | PASS | PASS | Copies 64 bytes (4 tiles) to VRAM and invokes Link sprite drawing (`00:1F38`) |
| `ReplaceTilesPairAndDrawLinkSprite` | VERIFIED | PASS | PASS | Copies 32 bytes (2 tiles) in bank $0C to VRAM and draws Link sprite (`00:1EA7`) |
| `ReplaceMarinTiles` | VERIFIED | PASS | PASS | Swaps Marin character tiles between standing (bank $0E) and sitting (bank $12) (`00:1DF4`) |
| `ReplaceTradingItemTiles` | VERIFIED | PASS | PASS | Swaps trading sequence item tile in VRAM (vTiles1+$1A0) for ribbon through magnifier items (`00:1E0C`) |
| `ReplaceMagicPowderTilesByToadstool` | VERIFIED | PASS | PASS | Replaces magic powder tile pair by toadstool in VRAM (`00:1E2B`) |
| `ReplaceSlimeKeyTilesByGoldenLeaf` | VERIFIED | PASS | PASS | Replaces slime key tile pair by golden leaf in VRAM (`00:1EA1`) |
| `ReplaceToadstoolTilesByMagicPowder` | VERIFIED | PASS | PASS | Replaces toadstool tile pair by magic powder in VRAM (`00:1E8D`) |
| `ReplaceTilesButtonPressed` | VERIFIED | PASS | PASS | Updates dungeon button-pressed tiles in VRAM (`00:1EB5`) |
| `ReplaceTiles_04` | VERIFIED | PASS | PASS | Replaces 4 dungeon tiles in vTiles2+$140 from bank $0D (`00:1EBC`) |
| `ReplaceEndCreditsTiles` | VERIFIED | PASS | PASS | Copies 64 bytes of credits graphic tiles to VRAM and restores bank $0C (`00:1E55`) |
| `ReplaceDialogTilesByInstruments` | VERIFIED | PASS | PASS | Copies Siren instrument dialog tiles from bank $11 according to wCreditsScratch0 (`00:1E33`) |
| `ReplaceTiles_08` | VERIFIED | PASS | PASS | Copies ending sequence graphics tiles from bank $13 according to wCreditsScratch0 (`00:1E69`) |
| `UpdateSwitchBlockTiles` | VERIFIED | PASS | PASS | Multi-stage switch block raise/lower tile animator updating blocks A and B in VRAM (`00:1ED7`) |
| `AnimateTiles` | VERIFIED | PASS | PASS | Core V-Blank graphics dispatcher routing gameplay modes, replacement requests, and animated tile tables (`00:1B0D`) |
| `label_1F69_trampoline` | VERIFIED | PASS | PASS | Trampoline calling label_1F69 and restoring ROM bank $02 (`00:1F61`) |
| `label_1F69` | VERIFIED | PASS | PASS | Evaluates room object in front of Link, text interaction (signs/chests/weather vane), and Power Bracelet pull/lift cadence (`00:1F69`) |
| `func_2165` | VERIFIED | PASS | PASS | Sets hObjectUnderEntity, calls RevealObjectUnderObject trampoline, saves direction, and spawns lifted rock projectile (`00:2165`) |
| `RevealObjectUnderObject_trampoline` | VERIFIED | PASS | PASS | Direct bank switch to $14 to reveal underlying ground tile and restore bank (`00:2178`) |
| `label_2183` | VERIFIED | PASS | PASS | Spawns ENTITY_LIFTABLE_ROCK projectile, plays WAVE_SFX_LIFT_UP, and configures status & variant tables (`00:2183`) |
| `UpdateFinalLinkPosition` | VERIFIED | PASS | PASS | Integrates Link horizontal and vertical velocities into position unless inventory is appearing (`00:21A8`) |
| `ComputeLinkPosition` | VERIFIED | PASS | PASS | Fixed-point 4.4 speed integrator with subpixel accumulator updating Link coordinate along specified axis (`00:21B6`) |
| `func_21E1` | VERIFIED | PASS | PASS | Fixed-point 4.4 Z-axis velocity integrator with subpixel accumulator updating hLinkPositionZ (`00:21E1`) |
| `UpdateBGRegion` | VERIFIED | PASS | PASS | Selects Map Data ROM bank ($08), invokes DoUpdateBGRegion, and restores previous bank (`00:2209`) |
| `CopyObjectRowToBGMap` | VERIFIED | PASS | PASS | Copies two horizontally-adjacent bytes from 4-byte object tilemap/attrmap to destination BG map (`00:2214`) |
| `CopyObjectColumnToBGMap` | VERIFIED | PASS | PASS | Copies two vertically-adjacent bytes from 4-byte object tilemap/attrmap to destination BG map (`00:2224`) |
| `DoUpdateBGRegion` | VERIFIED | PASS | PASS | Core loop updating a region (row/column) of BG tilemap and GBC attributes during room transitions (`00:2234`) |
| `SelectMusicTrackAfterTransition_trampoline` | VERIFIED | PASS | PASS | Trampoline switching to bank $02 to select new world music track and reloading saved bank (`00:27DD`) |
| `ResetMusicFadeTimer` | VERIFIED | PASS | PASS | Resets audio fade-out timer to max ($38) and clears fade-in timer (`00:27EA`) |
| `func_27F2` | VERIFIED | PASS | PASS | If music does not continue after warp, invokes bank $1F helper $4003 and reloads saved bank (`00:27F2`) |

---

## Technical Notes & Implementation Details

1. **Room Transition BG Region Updates (`00:2209`-`00:2319`)**:
   - `UpdateBGRegion` orchestrates progressive streaming of the destination room's background tiles during camera pan scrolling transitions.
   - For vertical scrolling (`DIRECTION_UP` / `DIRECTION_DOWN`), tiles are copied as 1x2 horizontal slices (`CopyObjectRowToBGMap`) selecting upper or lower pair based on `wBGUpdateRegionOriginLow & 0x20`.
   - For horizontal scrolling (`DIRECTION_LEFT` / `DIRECTION_RIGHT`), tiles are copied as 2x1 vertical slices (`CopyObjectColumnToBGMap`) selecting left or right column based on `wBGUpdateRegionOriginLow & 0x01`.
   - On Game Boy Color, the routine alternates tilemap copying with attribute copying using bank switching to `hMultiPurpose8`.
   - On GBC Overworld, dynamic room modifications are preserved by reading object values from WRAM Bank 2 (`rSVBK = 2`).

1. **Link Interactive Motion & Position Physics (`00:1F61`-`00:2204`)**:
   - `ComputeLinkPosition` and `func_21E1` treat speeds as signed 4.4 fixed-point numbers:
     - The upper 4 bits are the integer pixel offset (`int8_t speed >> 4`).
     - The lower 4 bits are swapped and accumulated into subpixel registers `wC11A` (X), `wC11B` (Y), or `wC149` (Z).
     - When subpixel addition overflows, carry (+1) is added to the integer step.
   - `label_1F69` handles dual interactions:
     - Text reading: when facing `DIRECTION_UP` and pressing A or B, checks for Weather Vane base ($5E), Owl statues ($6F), Signposts ($D4), chests, or Marin dialogue.
     - Object lifting: checks Power Bracelet equipped in A or B slot. When pulling away from the object (opposite direction button held), increments `wPullCounter` up to threshold (3 with Piece of Power, 8 normally), then triggers object reveal and spawns `ENTITY_LIFTABLE_ROCK`.

1. **V-Blank Link Sprite Rendering & Dynamic Tile Replacement (`00:1B0D`, `00:1D2E`-`00:1F48`)**:
   - `DrawLinkSprite` formats two 8x16 sprite records at `wLinkOAMBuffer+8`:
     - Evaluates invincibility flicker (bit 2 on GBC, shifted on DMG).
     - Clamps vertical boundary at $88 (`cp $88; ret nc`).
     - Horizontal offset adds 8 for sprite 1.
     - Sets GBC palette bits based on equipped tunic (`wTunicType`: 0=green, 1=blue, 2=red) or pit-falling state ($50..$55), and forces flags $03 / $23 for swimming animation states $4E / $4F.
   - `UpdateSwitchBlockTiles` is a state-machine driven animator:
     - Stage 3: toggles `wSwitchBlocksState ^= 0x02`.
     - Stage 4: copies half-raised transition tiles to Switch Block A ($9040).
     - Stage 6: copies half-raised transition tiles to Switch Block B ($9080).
     - Stage 8: copies final state tiles to Switch Block A.
     - Stage 10: resets animation stage to 0 and copies final state tiles to Switch Block B.

1. **Animated Background Tiles Subsystem (`00:1ACC`-`00:1D2D`)**:
   - Animated tiles run on varying cadences checked during V-Blank:
     - Slow speed (every 16 frames: `hAnimatedTilesFrameCount & 0x0F == 0`): Tide ($6B), Village ($6C), Water Dungeon ($73), Slow Waterfall ($6A), Underground ($6E).
     - Medium speed (every 8 frames: `hAnimatedTilesFrameCount & 0x07 == 0`): Warp ($70), Bubbles ($74), Weather Vane ($77), Crystal Blocks ($76).
     - Fast speed (every 4 frames: `hAnimatedTilesFrameCount & 0x03 == 0`): Currents ($71), Waterfall ($72), Light Beam ($75).
     - Custom frame tables: Dungeon 1 ($6D) and Lava ($6F) index into `AnimatedTilesDataOffsets` (`{0, $40, $80, $C0, $C0, $C0, $80, $40}`) using `(frameCount >> 3) & 7`.
     - Dungeon 2 scrolling tiles: read from `wAnimatedScrollingTilesStorage` ($DCC0) into $8400 (Color Dungeon) or $90C0 (non-Color Dungeon).
   - In all cadence checks, when the frame count does not match the cadence mask, `SkipTilesGroupAnimation` is called to dispatch bank $20 auxiliary helper $54F5 and ensure ROM bank $0C is selected.

1. **Link Motion Handlers & Map Transition Subsystem (`00:1794`-`00:1ACB`)**:
   - `ApplyLinkMotionState`: ignores swimming; if sword active (`wC16A != 0`), configures OAM multipurpose positions, applies 4-frame flash during full charge (`wSwordCharge >= MAX_SWORD_CHARGE`), and dispatches to Bank $20; if magic rod attack step (`countdown & 0x80`), spawns `ENTITY_MAGIC_ROD_FIREBALL` on frame 0x0C if no dialog or transition is pending, plays `NOISE_SFX_MAGIC_ROD`, and restores countdown.
   - `LinkMotionMapFadeOutHandler`: clears `wEntitiesStatusTable`, records shop theft (`wIsThief`, `wHasStolenFromShop`, `wPhotos1` |= 0x40, `wDidStealItem` = 1), maps player position to warp tile in `wWarpPositions` (outdoor) or reads `wWarpStructs` (indoor), applies side-scrolling physics if category 2, updates dungeon room and minimap in Bank $14, and records spawn location via `SetSpawnLocation`.
   - `LinkMotionMapFadeInHandler`: manages Manbo warp-in sequence (`TRANSITION_GFX_MANBO_OUT`), calls Bank $20 fade-in routines, sets `wLinkMotionState` (retaining special state 1 if `wD463 == 1`), and opens shop confrontation `Dialog036` if `wDidStealItem` is set.
   - `UpdateLinkWalkingAnimation`: selects animation table based on swimming modifier, side-scrolling, lifting (`wIsCarryingLiftedObject`), pushing (`wIsLinkPushing`), or shield status (`wHasMirrorShield` and `wIsUsingShield`), and reads animation state indexed by `((hLinkDirection << 1) | ((wConsecutiveStepsCount >> 3) & 1))`.
   - `callsb` direct bank selection: macros `callsb` in LADX bypass `SwitchBank` by writing directly to `rSelectROMBank` without altering `wCurrentBank`, followed by restoring `rSelectROMBank` from `wCurrentBank`.

1. **Sword Collisions & Pegasus Boots Subsystem (`00:15A7`-`00:1793`)**:
   - `CheckStaticSwordCollision`: computes sword hitpoint using `SwordCollisionMapX` and `SwordCollisionMapY` (directional offsets for regular swings 0..3 or spin attacks 4..11). Locates intersecting object in `wRoomObjects` at `($D700 | (y | c))`; tests object physics flags via `GetObjectPhysicsFlags_trampoline`. If object is a bush or grass (`0xD3`, `0x5C`, `0x0A` outdoor, `0xDD` indoor), reveals hidden ground tile, spawns `ENTITY_LIFTABLE_ROCK` with smashing animation, and rolls 1/8 chance to drop `ENTITY_DROPPABLE_HEART` or `ENTITY_DROPPABLE_RUPEE` with bounce physics (`wEntitiesSpeedZTable = $10`).
   - `CheckItemsSwordCollision`: checks `wC16D != 0`, sets `hMultiPurpose0`/`hMultiPurpose1` using `LinkDirectionToSwordCollisionRangeX`/`LinkDirectionToSwordCollisionRangeY`, alerts nearby sword moblins (`wSwordMoblinAlertingSoundCounter = 4`), sets collision recoil timer `wC1C4 = $10`, and plays `NOISE_SFX_CLINK` ($17) on walls (`flags & $F0 == $90`) or `JINGLE_SWORD_POKING` ($07) on pokeable obstacles.
   - `UsePegasusBoots`: verifies Link is grounded and not moving vertically in side-scrolling areas; advances `wConsecutiveStepsCount` (+2); displays transient VFX; charges `wPegasusBootsChargeMeter` up to `MAX_PEGASUS_BOOTS_CHARGE` ($20); when full, engages sprint (`wIsRunningWithPegasusBoots = $20`), cancels spin attack and sword charge, and sets Link velocity vectors from `XPositionIncrementPegasusRunning` / `YPositionIncrementPegasusRunning` (32 / -32 px/s).
   - `DisplayTransientVfxForLinkRunning`: fires on 8-frame boundary (`hFrameCounter & 7 == 0`) when Link is grounded and unblocked; chooses between shallow water splash (`TRANSCIENT_VFX_PEGASUS_SPLASH` and `JINGLE_WATER_SPLASH`) or dry ground dust (`TRANSCIENT_VFX_PEGASUS_DUST` and `NOISE_SFX_FOOTSTEP`).

1. **Player Item Actions & Projectile Subsystem (`00:1382`-`00:158E`)**:
   - `SpawnPlayerProjectile`: dynamically allocates projectile in active entity table `wEntitiesStatusTable` (slots 0..15), computes directional X/Y offsets, sets initial Z velocity (`hLinkPositionZ + 1`), sets speed vectors from `PlayerProjectileSpeedXPerDirection` / `PlayerProjectileSpeedYPerDirection`, synchronizes sprite variant, direction, and thrown direction, and starts attack step countdown (`$0C`).
   - `ShootArrow`: checks active arrow limit (`ARROW_MAX_ACTIVE_COUNT = 2`), decrements arrow count in BCD, spawns arrow entity, checks for bomb-arrow combo (`wBombArrowCooldown != 0`), clears dropped bomb entity slot and converts arrow state to 1; otherwise plays whoosh SFX and sets cooldown. Applies `data_13AD` and `data_13B5` speed tables (adjusted by +4 if Piece of Power is active).
   - `UseBoomerang`: validates single active projectile, spawns `ENTITY_BOOMERANG` (0x01), and calls Bank `$20` trampoline.
   - `UseMagicPowder`: checks for active toadstool to initiate trade dialog (`DIALOG_GOT_TOADSTOOL` = $02); validates powder count; spawns `ENTITY_MAGIC_POWDER_SPRINKLE` (0x08) and invokes `SprinkleMagicPowder` in Bank `$20`.
   - `UseRocsFeather`: triggers jump state (`wIsLinkInTheAir = 1`), resets `wC152`/`wC153`, plays `JINGLE_FEATHER_JUMP` ($0D), applies directional horizontal/vertical boost vectors from `PegasusBootsJumpBoostXTable` / `PegasusBootsJumpBoostYTable` if Pegasus Boots sprint is active; in side-scrolling areas, computes vertical speed based on lateral directional inputs (`0xE8` or `0xEA`) and updates final link position.
   - `UseSword`: checks spin attack state; sets `SWORD_ANIMATION_STATE_SWING_START` ($01) and enables collision; selects random swing sound effect from `SwordRandomSfxTable`; updates facing direction from joypad; resets spin attack when grounded; checks for full health (`wFullHearts != 0`) and Level 2 Sword (`wSwordLevel == 2`) to fire `ENTITY_SWORD_BEAM` ($DF).

1. **Item Usage & Inventory Actions Subsystem (`00:1177`-`00:1381`) - check_items_to_use.asm 100% Complete**:
   - `CheckItemsToUse`: verifies `wBlockItemUsage | wC167 | wIsUsingHookshot == 0`. Handles Pegasus Boots charge running sword hold / shield raise; suppresses actions when gel is clinging or carrying objects; handles Pegasus Boots A/B button charges and meter reset; processes shield A/B button holds; checks Joypad state for newly pressed A/B buttons to invoke `UseItem`; evaluates sword hold via `HoldSwordIfNeeded`; and invokes Color Dungeon callback in Bank `$20`.
   - `UseItem`: dispatches item actions based on item ID (1..13).
   - `PlaceBomb`: prevents duplicate bombs (`wHasPlacedBomb >= 1`); plays error buzzer if `wBombCount == 0`; performs BCD decrement (`sub 1; daa`); spawns player projectile (`ENTITY_BOMB`); and invokes `ConvertToBombArrowIfNeeded` in Bank `$20`.
   - `UseShovel`: verifies Link is grounded and not currently shoveling; queries surface poking collision to select between `JINGLE_SWORD_POKING` ($07) or `NOISE_SFX_SHOVEL_DIG` ($0E); sets shovel active flag and timer.
   - `HoldSwordIfNeeded`: checks NPC/text context; enters `SWORD_ANIMATION_STATE_HOLDING` ($05) and sets `wSwordCollisionEnabled`.

1. **Gameplay Dispatchers & Link Motion Subsystem (`00:0E34`-`00:1176`)**:
   - `ExecuteGameplayHandler`: checks `wGameplayType >= GAMEPLAY_WORLD_MAP`; if in non-interactive or menu state, or interactive world (`wGameplaySubtype == GAMEPLAY_WORLD_INTERACTIVE`), evaluates `CheckPresentSaveScreen`. If save screen combo (A+B+Start+Select) is pressed without active transition/dialog, transitions `wGameplayType` to `GAMEPLAY_FILE_SAVE`; otherwise dispatches via `jumpToGameplayHandler`.
   - `jumpToGameplayHandler`: jump table mapping 27 gameplay types (0x00..0x1A) to respective mode handlers (`IntroHandler`, `EndCreditsHandler`, `FileSelectionHandler`, `FileCreationHandler`, `FileDeletionHandler`, `FileCopyHandler`, `FileSaveHandler`, `WorldMapHandler`, `PeachPictureHandler`, `MarinBeachHandler`, `FaceShrineMuralHandler`, `WorldHandler`, `InventoryHandler`, `PhotoAlbumHandler`, `PhotoPictureHandler`).
   - `WorldHandler`: performs palette updates for interactive objects in Bank `$14`, executes overworld audio tasks, switches to Bank `$01`, and transfers control to `WorldHandlerEntryPoint`.
   - `LinkMotionTeleportUpHandler`, `LinkMotionPassOutHandler`, `LinkMotionDefaultHandler`: manage bank switches and execution dispatch for player teleportation (Bank `$19`), player death/pass-out (Bank `$01`), and default interactive motion (verifies interactive motion in Bank `$36`, then switches to Bank `$02`).

1. **Dialog Text Rendering, Line Scrolling, & Break Handling (`00:24CD`-`00:278A`) - Dialog Subsystem 100% Complete**:
   - `DialogBeginScrolling` & `DialogFinishScrolling`: shifts BG tile map rows up with 32-tile row wrapping; clears lower row with `hDialogBackgroundTile`; pauses 8 frames and transitions to `DIALOG_LETTER_IN_1`.
   - `DialogBreakHandler`: checks character index column (`wDialogCharacterIndex & 0x1F`); if 0 and next char is `@` (0xFF) or `<ask>` (0xFE), sets `DIALOG_END` or `DIALOG_CHOICE`; sets `wDialogIsWaitingForButtonPress` and plays `JINGLE_DIALOG_BREAK`; draws arrow marker via trampoline; advances on A button or skips on B button in world map; builds fill row draw command (`DC_FILL_ROW | 0x0F`) and advances state.
   - `DialogLetterAnimationEndHandler`: looks up screen destination high/low from Bank `$1C` tables (`Data_01C_45C1`, `Data_01C_4601`), writes tile command to `wDrawCommand`, increments state, and dispatches to `DialogDrawNextCharacterHandler`.
   - `DialogDrawNextCharacterHandler`: looks up dialog text pointer and bank from `DialogPointerTable` ($4001) and `DialogBankTable` ($4741) in Bank `$1C`; handles `#` player name expansion (reads Link's name or `"THIEF"` if `wIsThief != 0`); looks up codepoint in `CodepointToTileMap` ($4641) in Bank `$1C`; copies 16 bytes of font tile bitmap from `FontTiles` ($5000 in saved bank); handles SFX triggering on non-space characters (`WAVE_SFX_TEXT_PRINT`, `WAVE_SFX_OWL_HOOT`); detects box full (`wDialogNextCharPosition == 0x1F`) or resets to `DIALOG_LETTER_IN_1`.

1. **Dialog Dispatcher, BG Tile Backup, & Choice Selection (`00:2321`-`00:27BB`)**:
   - `ExecuteDialog`: sets `hDialogBackgroundTile` to `DIALOG_BG_TILE_LIGHT` ($7F) during `GAMEPLAY_CREDITS` and `DIALOG_BG_TILE_DARK` ($7E) otherwise; wraps `wDialogNextCharPosition` past 32 chars (`(char_lo & 0x0F) | 0x10`); dispatches according to `(wDialogState & ~0x80) - 1`.
   - `func_23E4`: computes source VRAM and destination `wD500` offsets from overlapping lookup tables (`s_dialog_save_table`), copies 2 rows of 18 tiles with 32-column row wrapping on DMG, and copies both tile indices (VRAM 0 -> WRAM 1) and color attributes (VRAM 1 -> WRAM 2) on CGB.
   - `DialogChoiceHandler`: checks joypad state; Right/Left toggles selection (`wDialogAskSelectionIndex = (sel + 1) & 1`) and plays `JINGLE_MOVE_SELECTION` ($0A); checks frame counter bit 4 to invoke `DrawDialogChoiceMarker` in Bank `$17`; advances to closing state on A button.
   - `SkipDialog`: sets `wDialogAskSelectionIndex = 2` and transitions dialog to closing state.
   - `DrawDialogArrowTrampoline`: switches to Bank `$17` and dispatches to `DrawDialogArrow`.

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
