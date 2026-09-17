#ifndef LADX_BANK2_ROOM_EVENTS_H
#define LADX_BANK2_ROOM_EVENTS_H

#include "gb.h"

/**
 * Spawns a chest containing an item at the coordinates of the intersected object. (02:41D0)
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn an entity. If NULL, standard slot search is used.
 * @return true if chest was spawned, false if no free entity slot.
 */
bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * Retrieves the address of the room status byte for the current room (02:5B9F).
 *
 * @param gb Pointer to Game Boy system state.
 * @return 16-bit address in WRAM of room status flags.
 */
uint16_t GetRoomStatusAddress(GBState *gb);

/**
 * Triggers the door unlocked noise SFX (02:5420).
 *
 * @param gb Pointer to Game Boy system state.
 */
void EnqueueDoorUnlockedSfx(GBState *gb);

/**
 * Spawns a key drop entity or slime key entity depending on dungeon map ID (02:5425).
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional entity spawn callback (00:3B86).
 */
void label_002_5425(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * Tries to open a key door with a small key or spawn pushed block (02:53B0).
 *
 * @param gb Pointer to Game Boy system state.
 * @param sync_item_flags Optional callback for SynchronizeDungeonsItemFlags (01:5E67).
 * @param reveal_object Optional callback for RevealObjectUnderObject (bank $14).
 * @param spawn_new_entity Optional callback for SpawnNewEntity (00:3B86).
 */
void TryOpenKeyDoor(GBState *gb,
                    void (*sync_item_flags)(GBState *),
                    void (*reveal_object)(GBState *),
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/* ========================================================================= */
/* Bank 2: Room Events & Door Opening/Closing Subsystem (02:593B - 02:5D4E)  */
/* ========================================================================= */

extern const uint8_t ShutterDoorsMaskTable[4];
extern const uint8_t OpeningDoorTileIds[72];
extern const uint8_t DoorXOffsets[18];
extern const uint8_t DoorYOffsets[18];
extern const uint8_t OpeningDoorTileOffsets[16];
extern const uint8_t OpenDoorObjectIdsTable[16];
extern const uint8_t DoorToOpenStatusFlagTable[9];
extern const int8_t DoorToAdjacentRoomTable[9];
extern const uint8_t DoorToAdjacentOpenStatusFlagTable[9];
extern const uint8_t ClosingDoorTileIds[32];
extern const uint8_t Data_002_5BE4[16];
extern const uint8_t ClosingDoorTileOffsets[8];
extern const uint8_t ClosedShutterDoorObjectIdsTable[8];

/**
 * Handles door opening animation, draw commands, object replacement, and status updates (02:5A7B).
 *
 * @param gb Pointer to Game Boy system state.
 * @param func_01A_6710 Optional callback for palette loading in bank $1A.
 * @param get_adjacent_address Optional callback for GetRoomStatusAddressForMapPosition.
 */
void DoorOpening(GBState *gb,
                 void (*func_01A_6710)(GBState *),
                 uint16_t (*get_adjacent_address)(GBState *, uint16_t));

/**
 * Handles door closing animation, draw commands, object replacement, and status updates (02:5C04).
 *
 * @param gb Pointer to Game Boy system state.
 * @param func_01A_6710 Optional callback for palette loading in bank $1A.
 */
void DoorClosing(GBState *gb, void (*func_01A_6710)(GBState *));

/**
 * Executes active room triggers, effects, and door opening/closing state handlers (02:593B).
 *
 * @param gb Pointer to Game Boy system state.
 * @param execute_room_triggers_and_effects Optional callback for room triggers and effects (02:5D4F).
 * @param func_01A_6710 Optional callback for palette loading in bank $1A.
 * @param get_adjacent_address Optional callback for GetRoomStatusAddressForMapPosition.
 */
void ExecuteRoomEvents(GBState *gb,
                       void (*execute_room_triggers_and_effects)(GBState *),
                       void (*func_01A_6710)(GBState *),
                       uint16_t (*get_adjacent_address)(GBState *, uint16_t));

#endif /* LADX_BANK2_ROOM_EVENTS_H */
