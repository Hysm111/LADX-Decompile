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

/**
 * MakeListOfDoorPositions (00:373F)
 * Stores the position, Y position (pos & 0xF0), and X position ((pos & 0x0F) << 4)
 * of a door in wDoorPositions, wDoorYPositions, and wDoorXPositions indexed by door_type.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param door_type Door type index (DOOR_TYPE_*)
 * @param pos Door position byte
 */
void MakeListOfDoorPositions(GBState *gb, uint8_t door_type, uint8_t pos);

/**
 * UpdateIndoorRoomStatus (00:36C4)
 * Updates room status bitmask for the current room in wIndoorARoomStatus (or
 * wIndoorBRoomStatus for MAP_INDOORS_B, or wColorDungeonRoomStatus for MAP_COLOR_DUNGEON),
 * and updates hRoomStatus.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param new_status Status bits to OR with current room status
 */
void UpdateIndoorRoomStatus(GBState *gb, uint8_t new_status);

void LoadObject_KeyDoorTop(GBState *gb, uint8_t pos);
void LoadObject_KeyDoorBottom(GBState *gb, uint8_t pos);
void LoadObject_KeyDoorLeft(GBState *gb, uint8_t pos);
void LoadObject_KeyDoorRight(GBState *gb, uint8_t pos);
void LoadObject_ShutterDoorTop(GBState *gb, uint8_t pos);
void LoadObject_ShutterDoorBottom(GBState *gb, uint8_t pos);
void LoadObject_ShutterDoorLeft(GBState *gb, uint8_t pos);
void LoadObject_ShutterDoorRight(GBState *gb, uint8_t pos);
void LoadObject_OpenDoorTop(GBState *gb, uint8_t pos);
void LoadObject_OpenDoorBottom(GBState *gb, uint8_t pos);
void LoadObject_OpenDoorLeft(GBState *gb, uint8_t pos);
void LoadObject_OpenDoorRight(GBState *gb, uint8_t pos);
void LoadObject_BossDoor(GBState *gb, uint8_t pos);
void LoadObject_StairsDoor(GBState *gb, uint8_t pos);
void LoadObject_RevolvingDoor(GBState *gb, uint8_t pos);
void LoadObject_OneWayArrow(GBState *gb, uint8_t pos);
void LoadObject_DungeonEntrance(GBState *gb, uint8_t pos);
void LoadObject_IndoorEntrance(GBState *gb, uint8_t pos);

/**
 * DispatchIndoorDoorObject (00:32DF)
 * Dispatches an indoor door object ($EC-$FD) to its corresponding handler.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param obj_type Object type
 * @param pos Position offset
 * @return true if handled as a door object, false otherwise
 */
bool DispatchIndoorDoorObject(GBState *gb, uint8_t obj_type, uint8_t pos);

/**
 * ExpandOverworldObjectMacro (24:7578)
 * Dispatches and unpacks overworld macro object into individual room objects.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param obj_type Macro object type ($F5-$FD)
 * @param pos Position (YX)
 */
void ExpandOverworldObjectMacro(GBState *gb, uint8_t obj_type, uint8_t pos);

/**
 * LoadRoomObject (00:32A9)
 * Reads an individual room object from the room stream and writes it to wRoomObjects.
 * Handles 2-byte and 3-byte objects, overworld state overrides (bridges, vane, gates),
 * indoor interactive objects (switches, conveyors, chests, bombable walls, stairs),
 * and dispatches to door handlers and macro expanders.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param stream_addr ROM or WRAM address pointing to start of the object definition
 * @return Number of bytes consumed from stream (2 or 3)
 */
size_t LoadRoomObject(GBState *gb, uint16_t stream_addr);

/**
 * PadRoomObjectsArea (01:6CCE)
 * Surrounds the objects area defining a room in wRoomObjectsArea ($D700) with
 * ROOM_BORDER ($FF) values along the perimeter.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PadRoomObjectsArea(GBState *gb);

/**
 * Callbacks for LoadRoom cross-bank subroutines
 */
typedef struct LoadRoomCallbacks {
    void (*reset_room_variables)(GBState *gb);
    void (*load_room_palettes)(GBState *gb);
    void (*load_room_objects_attributes)(GBState *gb);
    uint8_t (*func_014_5897)(GBState *gb);
    void (*load_room_template)(GBState *gb, uint8_t template_id);
    void (*pad_room_objects_area)(GBState *gb);
    void (*func_036_6D4D)(GBState *gb);
    void (*func_021_53F3)(GBState *gb);
} LoadRoomCallbacks;

/**
 * LoadRoom (00:30F4)
 * Loads room objects:
 * - Disables interrupts except VBlank (rIE = IEF_VBLANK)
 * - Increments wD47F
 * - Resets room variables in bank $20
 * - On GBC, loads room palettes (bank $21) and object attributes (bank $20)
 * - If indoor, invokes func_014_5897 and resets wKillCount / wKillOrder
 * - Updates visited status flag in room status table and stores into hRoomStatus
 * - Resolves room header pointer (checks alternate overworld rooms, Goriya cave, Color Dungeon)
 * - Fills active room floor tile and loads room template (if indoor)
 * - Loops over room objects: stores warps in wWarpStructs, unpacks objects via LoadRoomObject
 * - Surrounds room with ROOM_BORDER values via PadRoomObjectsArea
 * - Invokes post-load hooks in bank $36 and bank $21
 * - Restores saved ROM bank via ReloadSavedBank
 *
 * @param gb Pointer to Game Boy hardware state
 * @param callbacks Optional callbacks structure (or NULL for default behavior)
 */
void LoadRoom(GBState *gb, const LoadRoomCallbacks *callbacks);


/**
 * Copies two horizontally-adjacent bytes from the object tilemap/attrmap to the BG tilemap (00:2214).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Source pointer in object tilemap (in VRAM/ROM)
 * @param bc Destination pointer in BG map
 * @param out_hl Updated hl pointer output
 * @param out_bc Updated bc pointer output
 */
void CopyObjectRowToBGMap(GBState *gb, uint16_t *hl, uint16_t *bc);

/**
 * Copies two vertically-adjacent bytes from the object tilemap/attrmap to the BG tilemap (00:2224).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Source pointer in object tilemap
 * @param bc Destination pointer in BG map
 * @param out_hl Updated hl pointer output
 * @param out_bc Updated bc pointer output
 */
void CopyObjectColumnToBGMap(GBState *gb, uint16_t *hl, uint16_t *bc);

/**
 * Updates a region (row or column) of the BG map with object tiles and attributes (00:2234).
 */
void DoUpdateBGRegion(GBState *gb,
                      void (*func_020_4a76)(GBState *),
                      void (*get_bg_attr_addr)(GBState *),
                      void (*switch_to_tilemap_bank)(GBState *),
                      void (*func_020_49d9)(GBState *),
                      void (*update_origin)(GBState *));

/**
 * UpdateBGRegion (00:2209)
 * Switches to Map Data bank ($08), calls DoUpdateBGRegion, and reloads saved bank.
 */
void UpdateBGRegion(GBState *gb,
                    void (*func_020_4a76)(GBState *),
                    void (*get_bg_attr_addr)(GBState *),
                    void (*switch_to_tilemap_bank)(GBState *),
                    void (*func_020_49d9)(GBState *),
                    void (*update_origin)(GBState *));

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_ROOM_H */
