#include "bank2/room_events.h"
#include "bank1/room_transition.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "home/bank.h"
#include "home/entities.h"
#include "home/gameplay.h"
#include "home/link.h"
#include "home/room.h"
#include "home/vfx.h"

bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return false;

    uint16_t slot;
    if (spawn_new_entity) {
        slot = spawn_new_entity(gb, ENTITY_CHEST_WITH_ITEM);
        if (slot == 0xFFFF) {
            return false;
        }
        /* Callback typically returns an initialized entity slot; original decrement if needed */
        uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot));
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), (uint8_t)(status - 1));
    } else {
        /* Default slot search: find free slot scanning backwards from 15 down to 0 */
        int found = -1;
        for (int i = MAX_ENTITIES - 1; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) == ENTITY_STATUS_DISABLED) {
                found = i;
                break;
            }
        }
        if (found < 0) {
            return false;
        }
        slot = (uint16_t)found;
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), ENTITY_STATUS_INIT);
        gb_write(gb, (uint16_t)(wEntitiesTypeTable + slot), ENTITY_CHEST_WITH_ITEM);
    }

    /* Position chest: X = (hIntersectedObjectLeft & 0xF0) + 8, Y = (hIntersectedObjectTop & 0xF0) + 0x10 */
    uint8_t obj_left = (uint8_t)((gb_read_hram(gb, hIntersectedObjectLeft) & 0xF0) + 0x08);
    uint8_t obj_top = (uint8_t)((gb_read_hram(gb, hIntersectedObjectTop) & 0xF0) + 0x10);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), obj_left);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), obj_top);

    /* Variant: wEntitiesSpriteVariantTable[slot] = hMultiPurpose8 */
    uint8_t variant = gb_read_hram(gb, hMultiPurpose8);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + slot), variant);

    return true;
}

uint16_t GetRoomStatusAddress(GBState *gb) {
    if (!gb) return 0;

    uint8_t room = gb_read_hram(gb, hMapRoom);
    if (gb_read(gb, wIsIndoor) == 0) {
        return (uint16_t)(wOverworldRoomStatus + room);
    }

    uint8_t map_id = gb_read_hram(gb, hMapId);
    if (map_id == MAP_COLOR_DUNGEON) {
        return (uint16_t)(wColorDungeonRoomStatus + room);
    }

    uint8_t d = gb_read(gb, wIsIndoor);
    if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
        d++;
    }

    return (uint16_t)(wOverworldRoomStatus + ((uint16_t)d << 8) + room);
}

void EnqueueDoorUnlockedSfx(GBState *gb) {
    if (!gb) return;
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_DOOR_UNLOCKED);
}

void label_002_5425(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    uint8_t map_id = gb_read_hram(gb, hMapId);
    uint8_t entity_type;
    if (map_id == MAP_COLOR_DUNGEON) {
        entity_type = ENTITY_KEY_DROP_POINT;
    } else if (map_id < MAP_CAVE_B) {
        entity_type = ENTITY_KEY_DROP_POINT;
    } else {
        entity_type = ENTITY_HIDING_SLIME_KEY;
    }

    uint16_t slot = SpawnNewEntity_trampoline(gb, entity_type, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    uint8_t pos_x = 0x28;
    if (map_id == MAP_COLOR_DUNGEON) {
        pos_x = 0x48;
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_MARIN_BRIDGE) {
            pos_x = 0x58;
        }
    }
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), pos_x);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), 0x3C);
    gb_write(gb, (uint16_t)(wEntitiesPosZTable + slot), 0x70);
}

void TryOpenKeyDoor(GBState *gb,
                    void (*sync_item_flags)(GBState *),
                    void (*reveal_object)(GBState *),
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    if (gb_read_hram(gb, hMultiPurposeG) == 0x40) {
        /* .spawnPushedBlock */
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_PUSHED_BLOCK, spawn_new_entity);
        if (slot == 0xFFFF) {
            return;
        }

        uint8_t status = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot)) - 1);
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), status);

        uint8_t pos_x = (uint8_t)((gb_read_hram(gb, hMultiPurpose4) & 0xF0) + 0x08);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), pos_x);

        uint8_t pos_y = (uint8_t)((gb_read_hram(gb, hMultiPurpose5) & 0xF0) + 0x10);
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), pos_y);
        return;
    }

    uint8_t keys = gb_read(gb, wSmallKeysCount);
    if (keys == 0) {
        return;
    }

    keys--;
    gb_write(gb, wSmallKeysCount, keys);
    SynchronizeDungeonsItemFlags_trampoline(gb, sync_item_flags ? sync_item_flags : SynchronizeDungeonsItemFlags);
    EnqueueDoorUnlockedSfx(gb);

    uint16_t room_status_addr = GetRoomStatusAddress(gb);
    uint8_t status = (uint8_t)(gb_read(gb, room_status_addr) | ROOM_STATUS_EVENT_3);
    gb_write(gb, room_status_addr, status);
    gb_write_hram(gb, hRoomStatus, status);

    uint8_t left = (uint8_t)(gb_read_hram(gb, hMultiPurpose4) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectLeft, left);

    uint8_t top = (uint8_t)(gb_read_hram(gb, hMultiPurpose5) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectTop, top);

    RevealObjectUnderObject_trampoline(gb, reveal_object);

    gb_write_hram(gb, hMultiPurpose0, (uint8_t)(left + 0x08));
    gb_write_hram(gb, hMultiPurpose1, (uint8_t)(top + 0x10));
    AddTranscientVfx(gb, TRANSCIENT_VFX_POOF);
}

/* ========================================================================= */
/* Bank 2: Room Events & Door Opening/Closing Subsystem (02:593B - 02:5D4E)  */
/* ========================================================================= */

const uint8_t ShutterDoorsMaskTable[4] = {
    (uint8_t)~DOOR_TYPE_SHUTTER_TOP_BIT,    /* 0xFE */
    (uint8_t)~DOOR_TYPE_SHUTTER_BOTTOM_BIT, /* 0xFD */
    (uint8_t)~DOOR_TYPE_SHUTTER_LEFT_BIT,   /* 0xFB */
    (uint8_t)~DOOR_TYPE_SHUTTER_RIGHT_BIT   /* 0xF7 */
};

const uint8_t OpeningDoorTileIds[72] = {
    /* Half-open door (36 bytes) */
    0x50, 0x51, 0x13, 0x12, /* DOOR_TYPE_KEY_TOP */
    0x11, 0x10, 0x42, 0x43, /* DOOR_TYPE_KEY_BOTTOM */
    0x45, 0x13, 0x55, 0x11, /* DOOR_TYPE_KEY_LEFT */
    0x12, 0x46, 0x10, 0x56, /* DOOR_TYPE_KEY_RIGHT */
    0x58, 0x59, 0x13, 0x12, /* DOOR_TYPE_SHUTTER_TOP */
    0x11, 0x10, 0x4A, 0x4B, /* DOOR_TYPE_SHUTTER_BOTTOM */
    0x4D, 0x13, 0x5D, 0x11, /* DOOR_TYPE_SHUTTER_LEFT */
    0x12, 0x4E, 0x10, 0x5E, /* DOOR_TYPE_SHUTTER_RIGHT */
    0x02, 0x03, 0x13, 0x12, /* DOOR_TYPE_BOSS_TOP */

    /* Fully open door (36 bytes) */
    0x11, 0x10, 0x13, 0x12, /* DOOR_TYPE_KEY_TOP */
    0x11, 0x10, 0x13, 0x12, /* DOOR_TYPE_KEY_BOTTOM */
    0x12, 0x13, 0x10, 0x11, /* DOOR_TYPE_KEY_LEFT */
    0x12, 0x13, 0x10, 0x11, /* DOOR_TYPE_KEY_RIGHT */
    0x11, 0x10, 0x13, 0x12, /* DOOR_TYPE_SHUTTER_TOP */
    0x11, 0x10, 0x13, 0x12, /* DOOR_TYPE_SHUTTER_BOTTOM */
    0x12, 0x13, 0x10, 0x11, /* DOOR_TYPE_SHUTTER_LEFT */
    0x12, 0x13, 0x10, 0x11, /* DOOR_TYPE_SHUTTER_RIGHT */
    0x11, 0x10, 0x13, 0x12  /* DOOR_TYPE_BOSS_TOP */
};

const uint8_t DoorXOffsets[18] = {
    0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08,
    0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08
};

const uint8_t DoorYOffsets[18] = {
    0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00,
    0x08, 0x08, 0x10, 0x10, 0x08, 0x08, 0x10, 0x10, 0x08
};

const uint8_t OpeningDoorTileOffsets[16] = {
    /* First object */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Second object */
    0x01, 0x01, 0x10, 0x10, 0x01, 0x01, 0x10, 0x10
};

const uint8_t OpenDoorObjectIdsTable[16] = {
    /* First object */
    0x43, 0x8C, 0x09, 0x0B, 0x43, 0x8C, 0x09, 0x0B,
    /* Second object */
    0x44, 0x08, 0x0A, 0x0C, 0x44, 0x08, 0x0A, 0x0C
};

const uint8_t DoorToOpenStatusFlagTable[9] = {
    ROOM_STATUS_DOOR_OPEN_UP,    /* DOOR_TYPE_KEY_TOP */
    ROOM_STATUS_DOOR_OPEN_DOWN,  /* DOOR_TYPE_KEY_BOTTOM */
    ROOM_STATUS_DOOR_OPEN_LEFT,  /* DOOR_TYPE_KEY_LEFT */
    ROOM_STATUS_DOOR_OPEN_RIGHT, /* DOOR_TYPE_KEY_RIGHT */
    ROOM_STATUS_DOOR_OPEN_UP,    /* DOOR_TYPE_SHUTTER_TOP */
    ROOM_STATUS_DOOR_OPEN_DOWN,  /* DOOR_TYPE_SHUTTER_BOTTOM */
    ROOM_STATUS_DOOR_OPEN_LEFT,  /* DOOR_TYPE_SHUTTER_LEFT */
    ROOM_STATUS_DOOR_OPEN_RIGHT, /* DOOR_TYPE_SHUTTER_RIGHT */
    ROOM_STATUS_DOOR_OPEN_UP     /* DOOR_TYPE_BOSS_TOP */
};

const int8_t DoorToAdjacentRoomTable[9] = {
    -8,  /* DOOR_TYPE_KEY_TOP */
     8,  /* DOOR_TYPE_KEY_BOTTOM */
    -1,  /* DOOR_TYPE_KEY_LEFT */
     1,  /* DOOR_TYPE_KEY_RIGHT */
    -8,  /* DOOR_TYPE_SHUTTER_TOP */
     8,  /* DOOR_TYPE_SHUTTER_BOTTOM */
    -1,  /* DOOR_TYPE_SHUTTER_LEFT */
     1,  /* DOOR_TYPE_SHUTTER_RIGHT */
    -8   /* DOOR_TYPE_BOSS_TOP */
};

const uint8_t DoorToAdjacentOpenStatusFlagTable[9] = {
    ROOM_STATUS_DOOR_OPEN_DOWN,  /* DOOR_TYPE_KEY_TOP */
    ROOM_STATUS_DOOR_OPEN_UP,    /* DOOR_TYPE_KEY_BOTTOM */
    ROOM_STATUS_DOOR_OPEN_RIGHT, /* DOOR_TYPE_KEY_LEFT */
    ROOM_STATUS_DOOR_OPEN_LEFT,  /* DOOR_TYPE_KEY_RIGHT */
    ROOM_STATUS_DOOR_OPEN_DOWN,  /* DOOR_TYPE_SHUTTER_TOP */
    ROOM_STATUS_DOOR_OPEN_UP,    /* DOOR_TYPE_SHUTTER_BOTTOM */
    ROOM_STATUS_DOOR_OPEN_RIGHT, /* DOOR_TYPE_SHUTTER_LEFT */
    ROOM_STATUS_DOOR_OPEN_LEFT,  /* DOOR_TYPE_SHUTTER_RIGHT */
    ROOM_STATUS_DOOR_OPEN_DOWN   /* DOOR_TYPE_BOSS_TOP */
};

const uint8_t ClosingDoorTileIds[32] = {
    /* Half-closed door */
    0x58, 0x59, 0x13, 0x12, /* DOOR_TYPE_SHUTTER_TOP */
    0x11, 0x10, 0x4A, 0x4B, /* DOOR_TYPE_SHUTTER_BOTTOM */
    0x4D, 0x13, 0x5D, 0x11, /* DOOR_TYPE_SHUTTER_LEFT */
    0x12, 0x4E, 0x10, 0x5E, /* DOOR_TYPE_SHUTTER_RIGHT */

    /* Fully closed door */
    0x40, 0x41, 0x58, 0x59, /* DOOR_TYPE_SHUTTER_TOP */
    0x4A, 0x4B, 0x52, 0x53, /* DOOR_TYPE_SHUTTER_BOTTOM */
    0x44, 0x4D, 0x54, 0x5D, /* DOOR_TYPE_SHUTTER_LEFT */
    0x4E, 0x47, 0x5E, 0x57  /* DOOR_TYPE_SHUTTER_RIGHT */
};

const uint8_t Data_002_5BE4[16] = {
    0x08, 0x08, 0x00, 0x00,
    0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10
};

const uint8_t ClosingDoorTileOffsets[8] = {
    0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x10, 0x10
};

const uint8_t ClosedShutterDoorObjectIdsTable[8] = {
    /* First object */
    0x35, 0x37, 0x39, 0x3B,
    /* Second object */
    0x36, 0x38, 0x3A, 0x3C
};

void DoorOpening(GBState *gb,
                 void (*func_01A_6710)(GBState *),
                 uint16_t (*get_adjacent_address)(GBState *, uint16_t))
{
    if (!gb) return;

    gb_write_hram(gb, hMultiPurposeC, 0x00);
    gb_write_hram(gb, hMultiPurposeE, 0x00);

    uint8_t door_event = gb_read(gb, wDoorEvent);
    uint16_t de_offset = (uint16_t)(door_event * 4);

    uint8_t left = (uint8_t)(DoorXOffsets[door_event] + gb_read(gb, (uint16_t)(wDoorXPositions + door_event)));
    gb_write_hram(gb, hIntersectedObjectLeft, left);

    while (1) {
        /* First tile pair (hMultiPurposeC == 0) */
        uint8_t top0 = (uint8_t)(DoorYOffsets[door_event] + gb_read(gb, (uint16_t)(wDoorYPositions + door_event)));
        gb_write_hram(gb, hIntersectedObjectTop, top0);
        GetIntersectedObjectBGAddress(gb);

        if (gb_read_hram(gb, hIsGBC) != 0) {
            uint16_t temp_de = de_offset;
            func_999(gb, &temp_de, 0x02, func_01A_6710);
        }

        uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
        uint16_t cmd_ptr = (uint16_t)(wDrawCommand + cmd_size);
        gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 5));

        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressHigh));
        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressLow));
        gb_write(gb, cmd_ptr++, 0x01);
        gb_write(gb, cmd_ptr++, OpeningDoorTileIds[de_offset]);
        gb_write(gb, cmd_ptr,   OpeningDoorTileIds[de_offset + 1]);

        /* Second tile pair (hMultiPurposeC == 9) */
        gb_write_hram(gb, hMultiPurposeC, 0x09);
        uint8_t top1 = (uint8_t)(DoorYOffsets[9 + door_event] + gb_read(gb, (uint16_t)(wDoorYPositions + door_event)));
        gb_write_hram(gb, hIntersectedObjectTop, top1);
        GetIntersectedObjectBGAddress(gb);

        de_offset += 2;
        if (gb_read_hram(gb, hIsGBC) != 0) {
            uint16_t temp_de = de_offset;
            func_999(gb, &temp_de, 0x02, func_01A_6710);
        }

        cmd_size = gb_read(gb, wDrawCommandsSize);
        cmd_ptr = (uint16_t)(wDrawCommand + cmd_size);
        gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 5));

        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressHigh));
        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressLow));
        gb_write(gb, cmd_ptr++, 0x01);
        gb_write(gb, cmd_ptr++, OpeningDoorTileIds[de_offset]);
        gb_write(gb, cmd_ptr++, OpeningDoorTileIds[de_offset + 1]);
        gb_write(gb, cmd_ptr,   0x00); /* terminator */

        uint8_t dbac = (uint8_t)(gb_read(gb, wDBAC) + 1);
        gb_write(gb, wDBAC, dbac);
        if (dbac < 0x08) {
            return;
        }

        if (gb_read_hram(gb, hMultiPurposeE) != 0) {
            break;
        }

        /* Transition to phase 2 (fully open door tiles) */
        gb_write_hram(gb, hMultiPurposeC, 0x00);
        de_offset = (uint16_t)(de_offset + 0x24 - 2);
        gb_write_hram(gb, hMultiPurposeE, (uint8_t)(de_offset + 2));
    }

    /* Finish door opening */
    gb_write(gb, wDoorsOpeningOrClosing, 0x00);
    gb_write(gb, wC1A8, 0x00);
    gb_write_hram(gb, hMultiPurposeE, 0x00);

    uint8_t pos = gb_read(gb, (uint16_t)(wDoorPositions + door_event));
    gb_write_hram(gb, hMultiPurpose0, pos);

    uint8_t c = (uint8_t)(door_event & 0x07);
    uint16_t obj_addr1 = (uint16_t)(wRoomObjects + pos + OpeningDoorTileOffsets[c]);
    gb_write(gb, obj_addr1, OpenDoorObjectIdsTable[c]);
    uint16_t obj_addr2 = (uint16_t)(wRoomObjects + pos + OpeningDoorTileOffsets[c + 8]);
    gb_write(gb, obj_addr2, OpenDoorObjectIdsTable[c + 8]);

    uint16_t status_addr = GetRoomStatusAddress(gb);
    uint8_t status_val = (uint8_t)(gb_read(gb, status_addr) | DoorToOpenStatusFlagTable[door_event]);
    gb_write(gb, status_addr, status_val);
    gb_write_hram(gb, hRoomStatus, status_val);

    uint8_t adj_room = (uint8_t)(gb_read(gb, wIndoorRoom) + (int8_t)DoorToAdjacentRoomTable[door_event]);
    uint16_t adj_addr = GetRoomStatusAddressForMapPosition_trampoline(gb, adj_room, get_adjacent_address);
    uint8_t adj_val = (uint8_t)(gb_read(gb, adj_addr) | DoorToAdjacentOpenStatusFlagTable[door_event]);
    gb_write(gb, adj_addr, adj_val);
}

void DoorClosing(GBState *gb, void (*func_01A_6710)(GBState *)) {
    if (!gb) return;

    gb_write_hram(gb, hMultiPurposeC, 0x00);
    gb_write_hram(gb, hMultiPurposeD, 0x00);
    gb_write_hram(gb, hMultiPurposeE, 0x00);

    uint8_t door_event = gb_read(gb, wDoorEvent);
    uint16_t de_offset = (uint16_t)((door_event - 4) * 4);

    uint8_t left = (uint8_t)(Data_002_5BE4[door_event - 4] + gb_read(gb, (uint16_t)(wDoorXPositions + door_event)));
    gb_write_hram(gb, hIntersectedObjectLeft, left);

    while (1) {
        /* First tile pair (hMultiPurposeC == 0) */
        uint8_t top0 = (uint8_t)(Data_002_5BE4[4 + door_event] + gb_read(gb, (uint16_t)(wDoorYPositions + door_event)));
        gb_write_hram(gb, hIntersectedObjectTop, top0);

        uint8_t diff_y0 = (uint8_t)((uint8_t)(gb_read(gb, wLinkMapEntryPositionY) - 0x10) - top0 + 0x10);
        if (diff_y0 < 0x20) {
            uint8_t diff_x0 = (uint8_t)((uint8_t)(gb_read(gb, wLinkMapEntryPositionX) - 0x08) - left + 0x10);
            if (diff_x0 < 0x20) {
                gb_write(gb, wLinkMapEntryPositionX, gb_read_hram(gb, hLinkPositionX));
                gb_write(gb, wLinkMapEntryPositionY, gb_read_hram(gb, hLinkPositionY));
            }
        }

        GetIntersectedObjectBGAddress(gb);

        if (gb_read_hram(gb, hIsGBC) != 0) {
            uint16_t temp_de = de_offset;
            func_999(gb, &temp_de, 0x02, func_01A_6710);
        }

        uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
        uint16_t cmd_ptr = (uint16_t)(wDrawCommand + cmd_size);
        gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 5));

        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressHigh));
        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressLow));
        gb_write(gb, cmd_ptr++, 0x01);
        gb_write(gb, cmd_ptr++, ClosingDoorTileIds[de_offset]);
        gb_write(gb, cmd_ptr,   ClosingDoorTileIds[de_offset + 1]);

        /* Second tile pair (hMultiPurposeC == 4) */
        gb_write_hram(gb, hMultiPurposeC, 0x04);
        uint8_t top1 = (uint8_t)(Data_002_5BE4[8 + door_event] + gb_read(gb, (uint16_t)(wDoorYPositions + door_event)));
        gb_write_hram(gb, hIntersectedObjectTop, top1);

        uint8_t diff_y1 = (uint8_t)((uint8_t)(gb_read(gb, wLinkMapEntryPositionY) - 0x10) - top1 + 0x10);
        if (diff_y1 < 0x20) {
            uint8_t diff_x1 = (uint8_t)((uint8_t)(gb_read(gb, wLinkMapEntryPositionX) - 0x08) - left + 0x10);
            if (diff_x1 < 0x20) {
                gb_write(gb, wLinkMapEntryPositionX, gb_read_hram(gb, hLinkPositionX));
                gb_write(gb, wLinkMapEntryPositionY, gb_read_hram(gb, hLinkPositionY));
            }
        }

        GetIntersectedObjectBGAddress(gb);

        de_offset += 2;
        if (gb_read_hram(gb, hIsGBC) != 0) {
            uint16_t temp_de = de_offset;
            func_999(gb, &temp_de, 0x02, func_01A_6710);
        }

        cmd_size = gb_read(gb, wDrawCommandsSize);
        cmd_ptr = (uint16_t)(wDrawCommand + cmd_size);
        gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 5));

        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressHigh));
        gb_write(gb, cmd_ptr++, gb_read_hram(gb, hIntersectedObjectBGAddressLow));
        gb_write(gb, cmd_ptr++, 0x01);
        gb_write(gb, cmd_ptr++, ClosingDoorTileIds[de_offset]);
        gb_write(gb, cmd_ptr++, ClosingDoorTileIds[de_offset + 1]);
        gb_write(gb, cmd_ptr,   0x00); /* terminator */

        uint8_t dbac = (uint8_t)(gb_read(gb, wDBAC) + 1);
        gb_write(gb, wDBAC, dbac);
        if (dbac < 0x08) {
            return;
        }

        if (gb_read_hram(gb, hMultiPurposeE) != 0) {
            break;
        }

        /* Transition to phase 2 (fully closed door tiles) */
        gb_write_hram(gb, hMultiPurposeC, 0x00);
        de_offset = (uint16_t)(de_offset + 0x10 - 2);
        gb_write_hram(gb, hMultiPurposeE, (uint8_t)(de_offset + 2));
    }

    /* Finish door closing */
    gb_write(gb, wDoorsOpeningOrClosing, 0x00);
    gb_write(gb, wC1A8, 0x00);
    gb_write_hram(gb, hMultiPurposeE, 0x00);

    uint8_t c = (uint8_t)(door_event - 4);
    uint8_t pos = gb_read(gb, (uint16_t)(wDoorPositions + door_event));
    gb_write_hram(gb, hMultiPurpose0, pos);

    uint16_t obj_addr1 = (uint16_t)(wRoomObjects + pos + ClosingDoorTileOffsets[c]);
    gb_write(gb, obj_addr1, ClosedShutterDoorObjectIdsTable[c]);
    uint16_t obj_addr2 = (uint16_t)(wRoomObjects + pos + ClosingDoorTileOffsets[c + 4]);
    gb_write(gb, obj_addr2, ClosedShutterDoorObjectIdsTable[c + 4]);

    uint16_t status_addr;
    if (gb_read(gb, wIsIndoor) == 0) {
        status_addr = (uint16_t)(wOverworldRoomStatus + gb_read_hram(gb, hMapRoom));
    } else if (gb_read_hram(gb, hMapId) == MAP_COLOR_DUNGEON) {
        status_addr = (uint16_t)(wColorDungeonRoomStatus + gb_read_hram(gb, hMapRoom));
    } else {
        status_addr = (uint16_t)(wIndoorARoomStatus + gb_read_hram(gb, hMapRoom));
    }

    uint8_t mask = (uint8_t)~DoorToOpenStatusFlagTable[door_event];
    uint8_t new_status = (uint8_t)(gb_read(gb, status_addr) & mask);
    gb_write(gb, status_addr, new_status);
    gb_write_hram(gb, hRoomStatus, new_status);
}

void ExecuteRoomEvents(GBState *gb,
                       void (*execute_room_triggers_and_effects)(GBState *),
                       void (*func_01A_6710)(GBState *),
                       uint16_t (*get_adjacent_address)(GBState *, uint16_t))
{
    if (!gb) return;

    if (gb_read(gb, wDialogState) != 0 ||
        gb_read(gb, wRoomTransitionState) != 0 ||
        gb_read(gb, wInventoryAppearing) != 0) {
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        return;
    }

    if (execute_room_triggers_and_effects) {
        execute_room_triggers_and_effects(gb);
    }

    uint8_t state = gb_read(gb, wDoorsOpeningOrClosing);
    if (state != 0) {
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
        if (state == 0x02) {
            DoorClosing(gb, func_01A_6710);
        } else {
            DoorOpening(gb, func_01A_6710, get_adjacent_address);
        }
        return;
    }

    if (gb_read(gb, wEnqueueDoorsOpening) != 0) {
        uint8_t mask = gb_read(gb, wShutterDoorsMask);
        uint8_t c = mask;
        for (uint8_t e = 4; e < 8; e++) {
            uint8_t bit = (uint8_t)(c & 0x01);
            c >>= 1;
            if (bit != 0) {
                mask &= ShutterDoorsMaskTable[e - 4];
                gb_write(gb, wShutterDoorsMask, mask);
                gb_write(gb, wDoorEvent, e);
                gb_write(gb, wDBAC, 0x00);
                gb_write(gb, wDoorsOpeningOrClosing, 0x01);
                return;
            }
        }
        gb_write(gb, wEnqueueDoorsOpening, 0x00);
        return;
    }

    if (gb_read(gb, wEnqueueDoorsClosing) != 0) {
        uint8_t mask = gb_read(gb, wShutterDoorsMask2);
        uint8_t c = mask;
        for (uint8_t e = 4; e < 8; e++) {
            uint8_t bit = (uint8_t)(c & 0x01);
            c >>= 1;
            if (bit != 0) {
                mask &= ShutterDoorsMaskTable[e - 4];
                gb_write(gb, wShutterDoorsMask2, mask);
                gb_write(gb, wDoorEvent, e);
                gb_write(gb, wDoorsOpeningOrClosing, 0x02);
                gb_write(gb, wDBAC, 0x00);
                return;
            }
        }
        gb_write(gb, wEnqueueDoorsClosing, 0x00);
        return;
    }
}
