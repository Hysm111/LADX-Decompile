#ifndef LADX_BANK1_ROOM_TRANSITION_H
#define LADX_BANK1_ROOM_TRANSITION_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PrepareEntityPositionForRoomTransition (01:5EAB)
 * Configures the position and load order of a newly created entity before a room transition.
 * Adjusts entity X/Y position and sign tables based on wRoomTransitionDirection.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param entity_index Entity slot index (0..15)
 */
void PrepareEntityPositionForRoomTransition(GBState *gb, uint8_t entity_index);

/**
 * UpdateRecentRoomsList (01:5F02)
 * Adds the current room (hMapRoom) to the 6-slot circular recent rooms list (wRecentRooms),
 * advancing wRecentRoomsIndex and clearing the cleared-entities flag for the evicted room.
 * If the room is already in the list, does nothing.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void UpdateRecentRoomsList(GBState *gb);

/**
 * HideAllSprites (01:5F2E)
 * Disables cartridge SRAM and sets all 40 OAM entry Y positions to 0xF4.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void HideAllSprites(GBState *gb);

/**
 * HideSpritesUnderDialog (01:5F68)
 * Checks dialog state and hides non-Link sprites obscured by the dialog box.
 * Preserves Piece-of-Heart graphic if displaying Dialog04F.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void HideSpritesUnderDialog(GBState *gb);

/**
 * HideSprites (01:5F4B)
 * If inventory is opening, hides sprites overlapping the window;
 * otherwise calls HideSpritesUnderDialog.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void HideSprites(GBState *gb);


/**
 * SynchronizeDungeonsItemFlags (01:5E67)
 * Copies 5 bytes from wCurrentDungeonItemFlags to wColorDungeonItemFlags (if color dungeon)
 * or wDungeonItemFlags + (hMapId * 5) (if standard dungeon 0..9). Overworld is skipped.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void SynchronizeDungeonsItemFlags(GBState *gb);

/**
 * CreateFollowingNpcEntity (01:5FB3)
 * Checks and creates following entities (Rooster, Ghost, Marin, Bow-Wow) depending on quest state.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param spawn_new_entity Callback for SpawnNewEntity (bank $03)
 */
void CreateFollowingNpcEntity(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t entity_type));

/**
 * func_001_6162 (01:6162)
 * Clears audio, gameplay state, palettes, scroll registers, and switch block state.
 * Sets hButtonsInactiveDelay to 0x18.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_01F_4003 Callback for audio func_01F_4003 in bank $1F
 */
void func_001_6162(GBState *gb, void (*func_01F_4003)(GBState *));

/**
 * LoadCounterAnimatedTiles (01:61AA)
 * Loads animated counter tile graphics from bank $0F to $96D0/$96C0 based on wTextDebuggerDialogId,
 * and sets tiles $6C and $6D at $9909 and $990A.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCounterAnimatedTiles(GBState *gb);

/**
 * OpenDungeonNameDialog (01:61EE)
 * Opens dialog 0x56 + hMapId if Link is in default motion state and free movement mode is disabled.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void OpenDungeonNameDialog(GBState *gb);


/**
 * LoadTileset0F (01:6CE3)
 * Fills the visible 20 columns of vBGMap0 ($9800) with a checkerboard tile pattern ($AE/$AF).
 * If running on Game Boy Color (hIsGBC != 0), calls func_001_6D11 to initialize BG attributes.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadTileset0F(GBState *gb);

/**
 * func_001_6D11 (01:6D11)
 * Fills vBGMap0 attributes in VRAM Bank 1 with 0x05 (if GAMEPLAY_WORLD) or 0x06 (otherwise).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_001_6D11(GBState *gb);

/**
 * WriteDMACodeToHRAM (01:6D32)
 * Copies the 10-byte OAM DMA routine machine code from ROM to HRAM at hDMARoutine ($FFC0).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void WriteDMACodeToHRAM(GBState *gb);

/**
 * UpdateMinimapEntranceArrowAndReturn (01:6DEA)
 * Draws dungeon entrance indicator arrow tile ($A3) or hides it ($7F) on minimap vBGMap1.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void UpdateMinimapEntranceArrowAndReturn(GBState *gb);

/**
 * IncrementGameplaySubtype (01:44D6)
 * Increments wGameplaySubtype by 1.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void IncrementGameplaySubtype(GBState *gb);

/**
 * IncrementGameplaySubtypeAndReturn (01:44D6)
 * Alias to IncrementGameplaySubtype.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void IncrementGameplaySubtypeAndReturn(GBState *gb);


/**
 * func_001_5888 (01:5888)
 * Clears 12 bytes at wRoomTransitionState ($C124-$C12F).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_001_5888(GBState *gb);

/**
 * InitializeInventoryBar (01:5895)
 * Configures the window and subscreen registers to display the inventory bar at the bottom.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void InitializeInventoryBar(GBState *gb);

/**
 * func_001_58A8 (01:58A8)
 * Updates shallow water ripple animation sprite in OAM buffer at wDynamicOAMBuffer + 0x6C.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_001_58A8(GBState *gb);

/**
 * PeachPictureState2Handler (01:6856)
 * Selects the appropriate tileset to load for full-screen pictures (Eagle's Tower collapse, Schule painting, Christine).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState2Handler(GBState *gb);

/**
 * PeachPictureState3Handler (01:6873)
 * Selects the tilemap to load and resets window, scroll, transition counters and palette for picture display.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState3Handler(GBState *gb);


/**
 * func_001_695B (01:695B)
 * Vertical screen shake updater based on wD215 counter.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_001_695B(GBState *gb);

/**
 * func_6A7C (01:6A7C)
 * Sprite renderer for Eagle's Tower collapse sequence.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_6A7C(GBState *gb);

/**
 * PeachPictureState4Handler (01:68AA)
 * Fade-in handler for picture cutscenes.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState4Handler(GBState *gb);

/**
 * PeachPictureState5Handler (01:68C0)
 * Wait for button press / Eagle's tower jump handler for picture cutscenes.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState5Handler(GBState *gb);

/**
 * func_001_68D9 (01:68D9)
 * Advances gameplay subtype and clears transition sequence counters.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_001_68D9(GBState *gb);

/**
 * PeachPictureState7Handler (01:68E4)
 * Eagle's Tower collapse rumble timer and screen shake updater.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState7Handler(GBState *gb);


/**
 * PeachPictureState8Handler (01:6908)
 * Collapse sequence explosion and tower sinking animation step.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState8Handler(GBState *gb);

/**
 * PeachPictureState9Handler (01:6945)
 * Final collapse timer before fading out picture cutscene.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState9Handler(GBState *gb);

/**
 * FileSaveFadeOut (01:5825)
 * Handles screen fade-out and returns to world gameplay.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void FileSaveFadeOut(GBState *gb);

/**
 * PeachPictureStateAHandler (01:5822)
 * Final cutscene fade-out handler, calls func_6A7C and FileSaveFadeOut.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureStateAHandler(GBState *gb);

/**
 * PeachPictureState0Handler (01:6808)
 * Initial cutscene setup: advances subtype and copies palettes to WRAM bank 3 on CGB.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState0Handler(GBState *gb);

/**
 * PeachPictureState1Handler (01:6829)
 * Fades out current screen and loads checkerboard tileset 0F.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureState1Handler(GBState *gb);

/**
 * PeachPictureEntryPoint (01:67EE)
 * Main state dispatcher for full-screen picture cutscenes.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PeachPictureEntryPoint(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_BANK1_ROOM_TRANSITION_H */
