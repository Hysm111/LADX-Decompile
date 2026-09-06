# Link's Awakening DX Decompilation Progress

## Overall Status

* **Project Name**: Zelda: Link's Awakening DX C/C++ Decompilation
* **Current Overall Progress**: 10.08%
* **Number of Verified Functions**: 121
* **Number of Decompiled Functions**: 121
* **Number Remaining**: ~1130+ functions
* **Current Subsystem**: Bank 0 - Audio Controls, Hardware Interfaces & Gameplay Dispatch
* **Current Task**: Completed and verified SetWorldMusicTrack, EnableSRAM, SelectMusicTrackAfterTransition_trampoline, ResetMusicFadeTimer, func_27F2, SynchronizeDungeonsItemFlags_trampoline, GetRandomByte, ReadJoypadState, GetIntersectedObjectBGAddress, TableJump
* **Last Completed Task**: Decompiled and verified `SetWorldMusicTrack`, `EnableSRAM`, `SelectMusicTrackAfterTransition_trampoline`, `ResetMusicFadeTimer`, `func_27F2`, `SynchronizeDungeonsItemFlags_trampoline`, `GetRandomByte`, `ReadJoypadState`, `GetIntersectedObjectBGAddress`, `TableJump`
* **Next Task**: Continue Bank 0 gameplay loops and handlers (`WorldHandler`, `WorldInteractiveHandler`, `ApplyGotItem`)
* **Last Update Timestamp**: 2026-09-06T07:30:00+03:00

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
| `FillBGMapWhite` | VERIFIED | PASS | PASS | Fills 0x800 bytes of vBGMap0 with $7F (`00:28F7`) |\n| `FillBGMap` | VERIFIED | PASS | PASS | Fills BC bytes of vBGMap0 with value A (`00:28FC`) |
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
| `func_020_6AC1_trampoline` | VERIFIED | PASS | PASS | Calls target in bank $20 and restores saved bank (`00:08E6`) |\n| `UpdateIntroSeaBGPalettes_trampoline` | VERIFIED | PASS | PASS | Calls UpdateIntroSeaBGPalettes in bank $20 and restores saved bank (`00:08F0`) |
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
| `func_020_6D0E_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls func_020_6D0E, restores stacked bank (`00:0978`) |
| `CheckPushedTombStone_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls CheckPushedTombStone, restores stacked bank (`00:09C8`) |
| `GetEntityInitHandler_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls GetEntityInitHandler, restores stacked bank (`00:09D3`) |
| `func_020_4874_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls func_020_4874, restores stacked bank (`00:09DE`) |
| `func_020_4954_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls func_020_4954, restores stacked bank (`00:09E9`) |
| `ReplaceObjects56and57_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls ReplaceObjects56and57, restores stacked bank (`00:09F5`) |
| `func_036_505F_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls func_036_505F, restores stacked bank (`00:0A47`) |
| `func_036_4F9B_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls func_036_4F9B, restores stacked bank (`00:0A53`) |
| `func_003_5A2E_trampoline` | VERIFIED | PASS | PASS | Selects bank $03, calls func_003_5A2E, restores stacked bank (`00:0A6B`) |
| `func_036_4F68_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls func_036_4F68, restores stacked bank (`00:0A77`) |
| `func_020_6D52_trampoline` | VERIFIED | PASS | PASS | Selects bank $20, calls func_020_6D52, restores stacked bank (`00:0A83`) |
| `func_036_4BE8_trampoline` | VERIFIED | PASS | PASS | Selects bank $36, calls func_036_4BE8, restores stacked bank (`00:0A8F`) |
| `func_91D` | VERIFIED | PASS | PASS | Appends 2-column BG attribute draw command for room object to `wDrawCommandVRAM1` (`00:091D`) |
| `func_91D_jp_92E` | VERIFIED | PASS | PASS | Fetches object BG attributes from bank $1A and queues 2-column draw command (`00:092E`) |
| `func_983` | VERIFIED | PASS | PASS | Reads single object attribute byte from target ROM bank via helper `01A:6710` (`00:0983`) |
| `func_999` | VERIFIED | PASS | PASS | Appends 2-byte row BG attribute draw command to `wDrawCommandVRAM1` (`00:0999`) |
| `CreateTradingItemEntity` | VERIFIED | PASS | PASS | Spawns `ENTITY_TRADING_ITEM` at Link's current coordinates (`00:0C0C`) |
| `SpawnNewEntity_trampoline` | VERIFIED | PASS | PASS | Switches to bank $03, calls `SpawnNewEntity`, reloads saved bank (`00:3B86`) |
| `SpawnNewEntityInRange_trampoline` | VERIFIED | PASS | PASS | Switches to bank $03, calls `SpawnNewEntityInRange`, reloads saved bank (`00:3B98`) |
| `SelectRoomTilesets` | VERIFIED | PASS | PASS | Schedules loading of BG objects and OAM tilesets in bank $20 (`00:0D1E`) |
| `CheckPresentSaveScreen` | VERIFIED | PASS | PASS | Evaluates A+B+Start+Select combo and sets gameplay to FILE_SAVE (`00:0E46`) |
| `AnimateEntitiesAndRestoreBank` | VERIFIED | PASS | PASS | Calls AnimateEntities and restores requested bank (`00:0EF4`) |
| `AnimateEntitiesAndRestoreBank17` | VERIFIED | PASS | PASS | Switches to bank 3, calls AnimateEntities, restores bank $17 (`00:0EED`) |
| `AnimateEntitiesAndRestoreBank01` | VERIFIED | PASS | PASS | Switches to bank 3, calls AnimateEntities, restores bank $01 (`00:0EFC`) |
| `AnimateEntitiesAndRestoreBank02` | VERIFIED | PASS | PASS | Switches to bank 3, calls AnimateEntities, restores bank $02 (`00:0F05`) |
| `returnFromGameplayHandler` | VERIFIED | PASS | PASS | Common return path presenting dialog (bank $0F) and loading CGB BG palettes (bank $24) (`00:101A`) |
| `SetWorldMusicTrack` | VERIFIED | PASS | PASS | Sets music track, resets fade timers (`00:27C3`) |
| `EnableSRAM` | VERIFIED | PASS | PASS | Sets RAM bank 0 and writes CART_SRAM_ENABLE to rRAMG (`00:27D0`) |
| `SelectMusicTrackAfterTransition_trampoline` | VERIFIED | PASS | PASS | Switches to bank $02, calls SelectMusicTrackAfterTransition, restores bank (`00:27DD`) |
| `ResetMusicFadeTimer` | VERIFIED | PASS | PASS | Sets fade-out timer to MUSIC_FADE_OUT_TIMER_MAX ($38) and zeroes fade-in timer (`00:27EA`) |
| `func_27F2` | VERIFIED | PASS | PASS | If hContinueMusicAfterWarp is 0, calls func_01F_4003 in bank $1F, restores bank (`00:27F2`) |
| `SynchronizeDungeonsItemFlags_trampoline` | VERIFIED | PASS | PASS | Calls SynchronizeDungeonsItemFlags in bank $01, restores bank (`00:2802`) |
| `GetRandomByte` | VERIFIED | PASS | PASS | Advances seed with rrca(hFrameCounter + wRandomSeed + rLY) and returns random byte (`00:280D`) |
| `ReadJoypadState` | VERIFIED | PASS | PASS | Reads P1 joypad matrix, handles interactivity and pass-out motion (`00:281E`) |
| `GetIntersectedObjectBGAddress` | VERIFIED | PASS | PASS | Calculates BG tilemap address for intersected object from scroll and coords (`00:2887`) |
| `TableJump` | VERIFIED | PASS | PASS | Jump table dispatcher for `rst 0` (`00:28C0`) |

---

## Technical Discoveries

- **Room Tileset Selection (`code/bank0.asm:00:0D1E`)**:
  - Switches to ROM bank `$20`.
  - For indoors: reads from `ColorDungeonTilesetsTable` (`$70B3`) if `hMapId == 0xFF`, `IndoorsTilesetsTable + 0x100` (`$6FB3`) if map is in `indoors_b` range (`0x06 <= hMapId < 0x1A`), else `IndoorsTilesetsTable` (`$6EB3`).
  - For overworld: applies Room 7 egg hack (`ROOM_OW_RIGHT_OF_EGG` increments room to 8), maps 16x16 grid to 2x2 room block table at `OverworldTilesetsTable` (`$6E73`), respects `W_TILESET_KEEP` (`0x0F`) and camera shop room exclusivity (`W_TILESET_CAMERA_SHOP` at `0x1A`).
  - Spritesheet Groups: index into `RoomSpritesheetGroupsTable` (`$70D3`). Overworld checks Siren room owl-talked bit (group `$23` -> `$24`) and Walrus status (group `$21` -> `$22`).
  - Iterates 4 spritesheet slots, updating `wLoadedEntitySpritesheets` and triggering updates via `hNeedsUpdatingEntityTilesA` (slot A) and `wNeedsUpdatingEntityTilesB` (slot B).
  - Restores previous ROM bank via `ReloadSavedBank`.
- **Save Screen Trigger (`code/bank0.asm:00:0E46`)**:
  - Triggered only during interactive gameplay when A + B + Start + Select buttons are simultaneously held (`hPressedButtonsMask == 0xF0`).
  - Inhibited if indoor/outdoor transition sequence is active (`wTransitionSequenceCounter != 4`), dialog is open (`wDialogState != 0`), room scrolling is active (`wRoomTransitionState != 0`), `wC167 != 0`, inventory/photo viewer is active (`wGameplayType >= GAMEPLAY_INVENTORY`), or either `wD474` / `wD464` is non-zero.
  - On activation, resets sequence counter, clears `wGameplaySubtype` and dialog/screen state, and sets `wGameplayType` to `GAMEPLAY_FILE_SAVE` (`0x06`).
- **Gameplay Handler Return Path (`code/bank0.asm:00:101A`)**:
  - Switches to `BANK(FontTiles)` (`$0F`) to execute pending dialogs via `ExecuteDialog`.
  - If running on Game Boy Color (`hIsGBC != 0`), switches to `BANK(LoadBGPalettes)` (`$24`) and executes `LoadBGPalettes`.

- **Hardware Joypad Reading (`code/bank0.asm:00:281E`)**:
  - Checks if room transition is active (`wRoomTransitionState != 0`); if so, returns early without modifying joypad states.
  - Checks gameplay interactivity: if in world gameplay (`wGameplayType == GAMEPLAY_WORLD`), requires `wGameplaySubtype == GAMEPLAY_WORLD_INTERACTIVE`.
  - An exception exists for Link passing out (`wLinkMotionState == LINK_MOTION_PASS_OUT` and `hLinkPhysicsModifier == 4`), which allows input reading even before transition finishes.
  - Otherwise, requires `wTransitionSequenceCounter == 4` and `wPaletteUnknownE == 0`. If conditions are not met, clears `hPressedButtonsMask` and `hJoypadState`.
  - Reads buttons by strobing `rP1` with `J_BUTTONS` (`$20`), then d-pad with `J_DPAD` (`$10`), inverts the active-low signals, shifts d-pad to high nibble, combines both, computes newly pressed buttons using `(prev ^ curr) & curr`, updates `hJoypadState` and `hPressedButtonsMask`, and resets `rP1` to `$30`.
- **Random Number Generation (`code/bank0.asm:00:280D`)**:
  - The RNG advances by summing `hFrameCounter`, the current `wRandomSeed`, and the LCD scanline register `rLY`.
  - The sum is rotated right 1 bit circularly (`rrca`), stored back to `wRandomSeed`, and returned in register A.
- **Cartridge SRAM Enabling (`code/bank0.asm:00:27D0`)**:
  - Maps external cartridge RAM bank 0 (`rRAMB = 0`), then writes `CART_SRAM_ENABLE` (`$0A`) to `rRAMG` (`$0000`) to enable battery-backed SRAM.
- **Table Jump Dispatcher (`code/bank0.asm:00:28C0`)**:
  - Invoked via `rst 0` (`JP_TABLE` macro).
  - Pops the return address off the stack (pointing to the table immediately following `rst 0`), multiplies the table index by 2, loads the target address word, and jumps to it.
