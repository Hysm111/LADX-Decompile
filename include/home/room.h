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

/**
 * FillRoomWithConsecutiveObjects (00:34EF)
 * Writes consecutive room objects into wRoomObjects either horizontally (advance 1)
 * or vertically (advance 16, if hMultiPurpose0 bit 6 is set).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Target destination address in wRoomObjects
 * @param obj_type Object type ID to write
 * @param count Number of consecutive objects to write
 */
void FillRoomWithConsecutiveObjects(GBState *gb, uint16_t hl, uint8_t obj_type, uint8_t count);

/**
 * SetupDestroyableObjectIfNeeded2 (00:3500)
 * Evaluates whether an overworld object is destroyable/interactive on CGB,
 * and calls BackupObjectInRAM2 with return bank $09 or $1A.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Target object address in wRoomObjects
 * @param obj_id Object ID
 */
void SetupDestroyableObjectIfNeeded2(GBState *gb, uint16_t hl, uint8_t obj_id);

/**
 * CopyObjectToActiveRoomMap (00:352D)
 * Writes an object value to wRoomObjects at (wRoomObjects + pos)
 * and invokes SetupDestroyableObjectIfNeeded2.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param pos Position offset within the room objects area
 * @param obj_val Object type ID
 */
void CopyObjectToActiveRoomMap(GBState *gb, uint8_t pos, uint8_t obj_val);

/**
 * SetBankForRoom (00:353B)
 * Selects BANK(OverworldRoomsFirstHalf) ($09) or BANK(OverworldRoomsSecondHalf) ($1A)
 * into rSelectROMBank depending on hMapRoom (< $80 or >= $80).
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Selected ROM bank number
 */
uint8_t SetBankForRoom(GBState *gb);

/**
 * SetupDestroyableObjectIfNeeded (00:35CB)
 * Evaluates whether an overworld object is destroyable/interactive on CGB,
 * and calls BackupObjectInRAM2 with fixed return bank $24.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Target object address in wRoomObjects
 * @param obj_id Object ID
 */
void SetupDestroyableObjectIfNeeded(GBState *gb, uint16_t hl, uint8_t obj_id);

/**
 * FillRoomMapWithObject (00:37E7)
 * Fills the active room map (10x8 tile area in wRoomObjects) with the given object type,
 * skipping row padding/borders. Saves object type in hMultiPurposeH.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param obj_type Object type to fill the room with
 */
void FillRoomMapWithObject(GBState *gb, uint8_t obj_type);

/**
 * LoadRoomTemplate_trampoline (00:38EA)
 * Switches to BANK(LoadRoomTemplate) ($14), invokes load_room_template callback with template_id,
 * and restores ROM bank from hRoomBank into rSelectROMBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param template_id Room template index
 * @param load_room_template Callback to LoadRoomTemplate in Bank $14
 */
void LoadRoomTemplate_trampoline(GBState *gb, uint8_t template_id,
                                void (*load_room_template)(GBState *, uint8_t));

/**
 * LoadWorldMapBGMap_trampoline (00:38FC)
 * Switches to bank $20 via callsb and invokes load_world_map_bg_map callback.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param load_world_map_bg_map Callback to LoadWorldMapBGMap in Bank $20
 */
void LoadWorldMapBGMap_trampoline(GBState *gb, void (*load_world_map_bg_map)(GBState *));

/**
 * ObjectPositionToRoomObjectAddress (00:35EE)
 * Returns the address in wRoomObjects for a given position byte.
 *
 * @param pos Position offset
 * @return Address within wRoomObjects (0xD711 + pos)
 */
uint16_t ObjectPositionToRoomObjectAddress(uint8_t pos);

/**
 * CopyIndoorsMacroObjectsToRoom (00:354B)
 * Copies objects provided by an indoors macro into wRoomObjects,
 * updating warp positions for door objects and calling SetupDestroyableObjectIfNeeded2.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Start location of objects (in wRoomObjects)
 * @param bc Object offsets list address (0xFF-terminated)
 * @param de Object IDs list address
 */
void CopyIndoorsMacroObjectsToRoom(GBState *gb, uint16_t hl, uint16_t bc, uint16_t de);

/**
 * CopyOutdoorsMacroObjectsToRoom (00:358B)
 * Copies objects provided by an outdoors macro into wRoomObjects,
 * updating warp positions for door objects and calling SetupDestroyableObjectIfNeeded.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Start location of objects (in wRoomObjects)
 * @param bc Object offsets list address (0xFF-terminated)
 * @param de Object IDs list address
 */
void CopyOutdoorsMacroObjectsToRoom(GBState *gb, uint16_t hl, uint16_t bc, uint16_t de);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ROOM_H */
