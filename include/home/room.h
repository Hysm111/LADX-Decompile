#ifndef LADX_HOME_ROOM_H
#define LADX_HOME_ROOM_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mark room trigger event as resolved:
 * If wRoomEventEffectExecuted != 0, returns immediately.
 * Otherwise, clears wC1CF, sets wRoomEventEffectExecuted and wC5A6 to 1,
 * and if wC19D == 0, triggers JINGLE_PUZZLE_SOLVED.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void MarkTriggerAsResolved(GBState *gb);

/**
 * Schedule the loading of object and OAM tilesets for the next room,
 * (either during a map transition or a room transition).
 * Actual loading will be done during the next vblank period.
 *
 * Switches ROM bank to 0x20 to look up Indoors/Overworld tileset tables,
 * evaluates special cases (Egg room, Color Dungeon, Camera Shop, Siren, Walrus),
 * schedules BG tiles updates (hNeedsUpdatingBGTiles),
 * schedules entity spritesheet updates (hNeedsUpdatingEntityTilesA, wNeedsUpdatingEntityTilesB),
 * and reloads the saved ROM bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void SelectRoomTilesets(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ROOM_H */
