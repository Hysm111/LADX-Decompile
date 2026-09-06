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

#ifdef __cplusplus
}
#endif

#endif /* LADX_BANK1_ROOM_TRANSITION_H */
