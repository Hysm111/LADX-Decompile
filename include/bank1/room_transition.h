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

#ifdef __cplusplus
}
#endif

#endif /* LADX_BANK1_ROOM_TRANSITION_H */
