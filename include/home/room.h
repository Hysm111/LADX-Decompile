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

/**
 * LoadTileset0F_trampoline (00:28E8)
 * Switches to BANK(LoadTileset0F) ($01) via SwitchBank and jumps to LoadTileset0F.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param load_tileset Callback to LoadTileset0F
 */
void LoadTileset0F_trampoline(GBState *gb, void (*load_tileset)(GBState *));

/**
 * GetChestsStatusForRoom_trampoline (00:29ED)
 * Switches to bank $14, calls GetChestsStatusForRoom callback,
 * and reloads saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param get_chests_status Callback to GetChestsStatusForRoom
 */
void GetChestsStatusForRoom_trampoline(GBState *gb, void (*get_chests_status)(GBState *));

/**
 * func_2A07 (00:2A07)
 * Switches to bank $01, calls func_001_5A59 callback,
 * and reloads saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_001_5A59 Callback to routine at 001:5A59
 */
void func_2A07(GBState *gb, void (*func_001_5A59)(GBState *));

/**
 * GetObjectPhysicsFlags (00:2A12)
 * Reads the physics flags for a given static object.
 * Sets rSelectROMBank to BANK(OverworldObjectPhysicFlags) ($08).
 * If hMapId == MAP_COLOR_DUNGEON ($FF), reads from Indoors1ObjectPhysicFlags + de.
 * Otherwise, reads from OverworldObjectPhysicFlags + de.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Room object offset
 * @return Physics flags byte
 */
uint8_t GetObjectPhysicsFlags(GBState *gb, uint16_t de);

/**
 * GetObjectPhysicsFlags_trampoline (00:2A26)
 * Calls GetObjectPhysicsFlags, then restores previous ROM bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Room object offset
 * @return Physics flags byte
 */
uint8_t GetObjectPhysicsFlags_trampoline(GBState *gb, uint16_t de);

/**
 * GetObjectPhysicsFlagsAndRestoreBank3 (00:2A2C)
 * Calls GetObjectPhysicsFlags, sets rSelectROMBank to $03, and returns physics flags.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Room object offset
 * @return Physics flags byte
 */
uint8_t GetObjectPhysicsFlagsAndRestoreBank3(GBState *gb, uint16_t de);

/**
 * GetRoomStatusAddressForMapPosition_trampoline (00:2BC1)
 * Farcalls GetRoomStatusAddressForMapPosition in bank $14 with room position in DE,
 * then restores saved ROM bank via ReloadSavedBank and returns address in HL.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Room position
 * @param get_address Callback to GetRoomStatusAddressForMapPosition
 * @return Room status address in WRAM
 */
uint16_t GetRoomStatusAddressForMapPosition_trampoline(GBState *gb, uint16_t de, uint16_t (*get_address)(GBState *, uint16_t));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ROOM_H */
