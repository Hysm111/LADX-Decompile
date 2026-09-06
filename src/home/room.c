#include "home/room.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/maps.h"
#include "constants/rooms.h"
#include "constants/gfx.h"
#include "constants/tilesets.h"
#include "constants/sfx.h"

void MarkTriggerAsResolved(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wRoomEventEffectExecuted) != 0) {
        return;
    }

    gb_write(gb, wC1CF, 0);
    gb_write(gb, wRoomEventEffectExecuted, 1);
    gb_write(gb, wC5A6, 1);

    if (gb_read(gb, wC19D) == 0) {
        gb_write(gb, hJingle, JINGLE_PUZZLE_SOLVED);
    }
}

void SelectRoomTilesets(GBState *gb) {
    if (!gb) return;

    /* Switch ROM bank to BANK(TilesetTables) ($20) */
    gb_write(gb, rSelectROMBank, 0x20);

    /* ------------------------------------------------------------
     * Select the new BG objects tileset
     * ------------------------------------------------------------ */
    uint8_t is_indoor = gb_read(gb, wIsIndoor);
    uint8_t map_room = gb_read(gb, hMapRoom);
    uint8_t map_id = gb_read(gb, hMapId);
    uint8_t current_world_tileset = gb_read(gb, hWorldTileset);

    if (is_indoor != 0) {
        /* Indoor BG tileset */
        uint16_t hl = IndoorsTilesetsTable_Addr;
        if (map_id == MAP_COLOR_DUNGEON) {
            hl = ColorDungeonTilesetsTable_Addr;
        } else if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
            hl += 0x100;
        }
        hl += map_room;

        uint8_t new_tileset = gb_read(gb, hl);
        if (new_tileset != current_world_tileset) {
            gb_write(gb, hWorldTileset, new_tileset);
            if (new_tileset != W_TILESET_NO_UPDATE) {
                gb_write(gb, hNeedsUpdatingBGTiles, TILESET_LOAD_WORLD);
            }
        }
    } else {
        /* Overworld BG tileset */
        uint8_t room = map_room;
        if (room == ROOM_OW_RIGHT_OF_EGG) {
            room++;
        }

        /* 2x2 room block index: ((room >> 2) & 0xF8) | ((room >> 1) & 0x07) */
        uint8_t de_low = ((room >> 2) & 0xF8) | ((room >> 1) & 0x07);
        uint8_t new_tileset = gb_read(gb, OverworldTilesetsTable_Addr + de_low);

        if (new_tileset != current_world_tileset && new_tileset != W_TILESET_KEEP) {
            if (new_tileset == W_TILESET_CAMERA_SHOP && map_room != ROOM_OW_CAMERA_SHOP) {
                /* Not on camera shop room, treat as TILESET_KEEP */
            } else {
                gb_write(gb, hWorldTileset, new_tileset);
                gb_write(gb, hNeedsUpdatingBGTiles, TILESET_LOAD_WORLD);
            }
        }
    }

    /* ------------------------------------------------------------
     * Select the new OAM tileset
     * ------------------------------------------------------------ */
    gb_write(gb, hMultiPurpose0, 0);

    uint8_t d = is_indoor;
    if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
        d++;
    }
    uint16_t de = ((uint16_t)d << 8) | map_room;
    uint8_t e = gb_read(gb, RoomSpritesheetGroupsTable_Addr + de);

    if (d != 0) {
        if (map_id == MAP_HOUSE && map_room == ROOM_INDOOR_B_CAMERA_SHOP) {
            e = 0x3D;
        }
    } else {
        if (e == 0x23) {
            if (gb_read(gb, wOverworldRoomStatus + ROOM_OW_SIREN) & OW_ROOM_STATUS_OWL_TALKED) {
                e++;
            }
        }
        if (e == 0x21) {
            if (gb_read(gb, wOverworldRoomStatus + ROOM_OW_WALRUS) & 0x20) {
                e++;
            }
        }
    }

    /* Color Dungeon handles spritesheets differently */
    if (map_id == MAP_COLOR_DUNGEON) {
        gb_write(gb, hNeedsUpdatingEntityTilesA, 1);
        ReloadSavedBank(gb);
        return;
    }

    uint16_t table = (is_indoor == 0) ? OverworldEntitySpritesheetsTable_Addr : IndoorEntitySpritesheetsTable_Addr;
    uint16_t spritesheets_addr = table + ((uint16_t)e * 4);

    for (uint8_t slot = 0; slot < 4; slot++) {
        uint8_t sheet = gb_read(gb, spritesheets_addr + slot);
        uint8_t loaded = gb_read(gb, wLoadedEntitySpritesheets + slot);

        if (sheet == loaded || sheet == 0xFF) {
            continue;
        }

        gb_write(gb, wLoadedEntitySpritesheets + slot, sheet);
        uint8_t mp0 = gb_read(gb, hMultiPurpose0);
        if (mp0 == 0) {
            gb_write(gb, hMultiPurpose0, mp0 + 1);
            gb_write(gb, wEntityTilesSpriteslotIndexA, slot);
            gb_write(gb, hNeedsUpdatingEntityTilesA, 1);
        } else {
            gb_write(gb, wEntityTilesSpriteslotIndexB, slot);
            gb_write(gb, wNeedsUpdatingEntityTilesB, 1);
        }
    }

    ReloadSavedBank(gb);
}

void LoadTileset0F_trampoline(GBState *gb, void (*load_tileset)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, 0x01);
    if (load_tileset) {
        load_tileset(gb);
    }
}

void GetChestsStatusForRoom_trampoline(GBState *gb, void (*get_chests_status)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x14);
    if (get_chests_status) {
        get_chests_status(gb);
    }
    ReloadSavedBank(gb);
}

void func_2A07(GBState *gb, void (*func_001_5A59)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x01);
    if (func_001_5A59) {
        func_001_5A59(gb);
    }
    ReloadSavedBank(gb);
}

uint8_t GetObjectPhysicsFlags(GBState *gb, uint16_t de) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, BANK_ObjectPhysicFlags);
    uint16_t hl = (gb_read(gb, hMapId) == MAP_COLOR_DUNGEON) ? Indoors1ObjectPhysicFlags : OverworldObjectPhysicFlags;
    return gb_read(gb, hl + de);
}

uint8_t GetObjectPhysicsFlags_trampoline(GBState *gb, uint16_t de) {
    if (!gb) return 0;

    uint8_t flags = GetObjectPhysicsFlags(gb, de);
    ReloadSavedBank(gb);
    return flags;
}

uint8_t GetObjectPhysicsFlagsAndRestoreBank3(GBState *gb, uint16_t de) {
    if (!gb) return 0;

    uint8_t flags = GetObjectPhysicsFlags(gb, de);
    gb_write(gb, rSelectROMBank, 0x03);
    return flags;
}

uint16_t GetRoomStatusAddressForMapPosition_trampoline(GBState *gb, uint16_t de, uint16_t (*get_address)(GBState *, uint16_t)) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, 0x14);
    uint16_t result = 0;
    if (get_address) {
        result = get_address(gb, de);
    }
    ReloadSavedBank(gb);
    return result;
}

void FillRoomWithConsecutiveObjects(GBState *gb, uint16_t hl, uint8_t obj_type, uint8_t count) {
    if (!gb) return;

    while (count > 0) {
        gb_write(gb, hl, obj_type);
        hl++;
        if (gb_read(gb, hMultiPurpose0) & 0x40) {
            hl += 0x0F;
        }
        count--;
    }
}

void SetupDestroyableObjectIfNeeded2(GBState *gb, uint16_t hl, uint8_t obj_id) {
    if (!gb) return;

    if (obj_id == OBJECT_SHORT_GRASS) {
        return;
    }

    uint8_t room = gb_read(gb, hMapRoom);

    if (obj_id == OBJECT_ROCKY_GROUND) {
        if (room != ROOM_OW_GIANT_SKULL) {
            return;
        }
        BackupObjectInRAM2(gb, hl, 0x1A);
        return;
    }

    if (obj_id == OBJECT_ROCKY_CAVE_DOOR) {
        if (room == ROOM_OW_EAGLES_TOWER || room == UNKNOWN_ROOM_0C || room == UNKNOWN_ROOM_1B) {
            return;
        }
    }

    uint8_t bank = (room >= ROOM_SECTION_OW_SECOND_HALF) ? 0x1A : 0x09;
    BackupObjectInRAM2(gb, hl, bank);
}

void CopyObjectToActiveRoomMap(GBState *gb, uint8_t pos, uint8_t obj_val) {
    if (!gb) return;

    uint16_t hl = wRoomObjects + pos;
    gb_write(gb, hl, obj_val);
    SetupDestroyableObjectIfNeeded2(gb, hl, obj_val);
}

uint8_t SetBankForRoom(GBState *gb) {
    if (!gb) return 0;

    uint8_t bank = (gb_read(gb, hMapRoom) >= ROOM_SECTION_OW_SECOND_HALF) ?
                   BANK_OverworldRoomsSecondHalf : BANK_OverworldRoomsFirstHalf;
    gb_write(gb, rSelectROMBank, bank);
    return bank;
}

void SetupDestroyableObjectIfNeeded(GBState *gb, uint16_t hl, uint8_t obj_id) {
    if (!gb) return;

    if (obj_id == OBJECT_SHORT_GRASS) {
        return;
    }

    uint8_t room = gb_read(gb, hMapRoom);

    if (obj_id == OBJECT_ROCKY_GROUND) {
        if (room != ROOM_OW_GIANT_SKULL) {
            return;
        }
        BackupObjectInRAM2(gb, hl, 0x24);
        return;
    }

    if (obj_id == OBJECT_ROCKY_CAVE_DOOR) {
        if (room == ROOM_OW_EAGLES_TOWER || room == UNKNOWN_ROOM_0C || room == UNKNOWN_ROOM_1B) {
            return;
        }
    }

    BackupObjectInRAM2(gb, hl, 0x24);
}

void FillRoomMapWithObject(GBState *gb, uint8_t obj_type) {
    if (!gb) return;

    gb_write(gb, hMultiPurposeH, obj_type);

    uint16_t hl = wRoomObjects;
    uint8_t d = TILES_PER_MAP;

    while (d > 0) {
        uint8_t col = (uint8_t)(hl & 0x0F);
        if (col != 0 && col < (OBJECTS_PER_ROW + 1)) {
            gb_write(gb, hl, obj_type);
        }
        hl++;
        d--;
    }
}

void LoadRoomTemplate_trampoline(GBState *gb, uint8_t template_id,
                                void (*load_room_template)(GBState *, uint8_t)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, BANK_LoadRoomTemplate);
    if (load_room_template) {
        load_room_template(gb, template_id);
    }
    gb_write(gb, rSelectROMBank, gb_read(gb, hRoomBank));
}

void LoadWorldMapBGMap_trampoline(GBState *gb, void (*load_world_map_bg_map)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, BANK_LoadWorldMapBGMap);
    if (load_world_map_bg_map) {
        load_world_map_bg_map(gb);
    }
}


static uint8_t ReadMacroROMByte(GBState *gb, uint16_t addr) {
    if (addr >= 0x4000) {
        return gb_read(gb, addr);
    }
    if (gb && gb->rom && gb->rom[addr] != 0) {
        return gb->rom[addr];
    }
    switch (addr) {
        /* HorizontalObjectOffsets ($37E1) */
        case HorizontalObjectOffsets_Addr: return 0x00;
        case HorizontalObjectOffsets_Addr + 1: return 0x01;
        case HorizontalObjectOffsets_Addr + 2: return 0xFF;

        /* VerticalObjectOffsets ($37E4) */
        case VerticalObjectOffsets_Addr: return 0x00;
        case VerticalObjectOffsets_Addr + 1: return 0x10;
        case VerticalObjectOffsets_Addr + 2: return 0xFF;

        /* KeyDoorTopObjectIds ($35F8) */
        case KeyDoorTopObjectIds_Addr: return 0x2D;
        case KeyDoorTopObjectIds_Addr + 1: return 0x2E;

        /* KeyDoorBottomObjectIds ($3613) */
        case KeyDoorBottomObjectIds_Addr: return 0x2F;
        case KeyDoorBottomObjectIds_Addr + 1: return 0x30;

        /* KeyDoorLeftObjectIds ($362E) */
        case KeyDoorLeftObjectIds_Addr: return 0x31;
        case KeyDoorLeftObjectIds_Addr + 1: return 0x32;

        /* KeyDoorRightObjectIds ($3649) */
        case KeyDoorRightObjectIds_Addr: return 0x33;
        case KeyDoorRightObjectIds_Addr + 1: return 0x34;

        /* OpenDoorTopObjectIds ($36B0) */
        case OpenDoorTopObjectIds_Addr: return 0x43;
        case OpenDoorTopObjectIds_Addr + 1: return 0x44;

        /* OpenDoorBottomObjectIds ($36E8) */
        case OpenDoorBottomObjectIds_Addr: return 0x8C;
        case OpenDoorBottomObjectIds_Addr + 1: return 0x08;

        /* OpenDoorLeftObjectIds ($36FC) */
        case OpenDoorLeftObjectIds_Addr: return 0x09;
        case OpenDoorLeftObjectIds_Addr + 1: return 0x0A;

        /* OpenDoorRightObjectIds ($3710) */
        case OpenDoorRightObjectIds_Addr: return 0x0B;
        case OpenDoorRightObjectIds_Addr + 1: return 0x0C;

        /* BossDoorObjectIds ($3724) */
        case BossDoorObjectIds_Addr: return 0xA4;
        case BossDoorObjectIds_Addr + 1: return 0xA5;

        /* StairsDoorObjectIds ($375C) */
        case StairsDoorObjectIds_Addr: return 0xAF;
        case StairsDoorObjectIds_Addr + 1: return 0xB0;

        /* RevolvingDoorObjectIds ($376B) */
        case RevolvingDoorObjectIds_Addr: return 0xB1;
        case RevolvingDoorObjectIds_Addr + 1: return 0xB2;

        /* OneWayArrowObjectIds ($377A) */
        case OneWayArrowObjectIds_Addr: return 0x45;
        case OneWayArrowObjectIds_Addr + 1: return 0x46;

        /* DungeonEntranceObjectOffsets ($3789) */
        case DungeonEntranceObjectOffsets_Addr: return 0x00;
        case DungeonEntranceObjectOffsets_Addr + 1: return 0x01;
        case DungeonEntranceObjectOffsets_Addr + 2: return 0x02;
        case DungeonEntranceObjectOffsets_Addr + 3: return 0x03;
        case DungeonEntranceObjectOffsets_Addr + 4: return 0x10;
        case DungeonEntranceObjectOffsets_Addr + 5: return 0x11;
        case DungeonEntranceObjectOffsets_Addr + 6: return 0x12;
        case DungeonEntranceObjectOffsets_Addr + 7: return 0x13;
        case DungeonEntranceObjectOffsets_Addr + 8: return 0x20;
        case DungeonEntranceObjectOffsets_Addr + 9: return 0x21;
        case DungeonEntranceObjectOffsets_Addr + 10: return 0x22;
        case DungeonEntranceObjectOffsets_Addr + 11: return 0x23;
        case DungeonEntranceObjectOffsets_Addr + 12: return 0xFF;

        /* DungeonEntranceObjectIds ($3796) */
        case DungeonEntranceObjectIds_Addr: return 0xB3;
        case DungeonEntranceObjectIds_Addr + 1: return 0xB4;
        case DungeonEntranceObjectIds_Addr + 2: return 0xB4;
        case DungeonEntranceObjectIds_Addr + 3: return 0xB5;
        case DungeonEntranceObjectIds_Addr + 4: return 0xB6;
        case DungeonEntranceObjectIds_Addr + 5: return 0xB7;
        case DungeonEntranceObjectIds_Addr + 6: return 0xB8;
        case DungeonEntranceObjectIds_Addr + 7: return 0xB9;
        case DungeonEntranceObjectIds_Addr + 8: return 0xBA;
        case DungeonEntranceObjectIds_Addr + 9: return 0xBB;
        case DungeonEntranceObjectIds_Addr + 10: return 0xBC;
        case DungeonEntranceObjectIds_Addr + 11: return 0xBD;

        /* EntranceObjectIds ($37B4) */
        case EntranceObjectIds_Addr: return 0xC1;
        case EntranceObjectIds_Addr + 1: return 0xC2;

        default:
            return gb_read(gb, addr);
    }
}

uint16_t ObjectPositionToRoomObjectAddress(uint8_t pos) {
    return wRoomObjects + pos;
}

void CopyIndoorsMacroObjectsToRoom(GBState *gb, uint16_t hl, uint16_t bc, uint16_t de) {
    if (!gb) return;

    while (1) {
        uint8_t offset = ReadMacroROMByte(gb, bc);
        if (offset == 0xFF) {
            break;
        }

        uint16_t target_hl = hl + offset;
        uint8_t obj_id = ReadMacroROMByte(gb, de);

        if (obj_id == OBJECT_ROCKY_CAVE_DOOR || obj_id == 0xE2 || obj_id == OBJECT_CAVE_DOOR) {
            uint8_t pos = (uint8_t)((target_hl & 0xFF) - 0x11);
            uint8_t c19c = gb_read(gb, wC19C);
            gb_write(gb, wC19C, (c19c + 1) & 0x03);
            gb_write(gb, wWarpPositions + c19c, pos);
        }

        gb_write(gb, target_hl, obj_id);
        SetupDestroyableObjectIfNeeded2(gb, target_hl, obj_id);

        de++;
        bc++;
    }
}

void CopyOutdoorsMacroObjectsToRoom(GBState *gb, uint16_t hl, uint16_t bc, uint16_t de) {
    if (!gb) return;

    while (1) {
        uint8_t offset = ReadMacroROMByte(gb, bc);
        if (offset == 0xFF) {
            break;
        }

        uint16_t target_hl = hl + offset;
        uint8_t obj_id = ReadMacroROMByte(gb, de);

        if (obj_id == OBJECT_ROCKY_CAVE_DOOR || obj_id == 0xE2 || obj_id == OBJECT_CAVE_DOOR) {
            uint8_t pos = (uint8_t)((target_hl & 0xFF) - 0x11);
            uint8_t c19c = gb_read(gb, wC19C);
            gb_write(gb, wC19C, (c19c + 1) & 0x03);
            gb_write(gb, wWarpPositions + c19c, pos);
        }

        gb_write(gb, target_hl, obj_id);
        SetupDestroyableObjectIfNeeded(gb, target_hl, obj_id);

        de++;
        bc++;
    }
}

void MakeListOfDoorPositions(GBState *gb, uint8_t door_type, uint8_t pos) {
    if (!gb) return;

    gb_write(gb, wDoorPositions + door_type, pos);
    gb_write(gb, wDoorYPositions + door_type, pos & 0xF0);
    uint8_t x = ((pos << 4) | (pos >> 4)) & 0xF0;
    gb_write(gb, wDoorXPositions + door_type, x);
}

void UpdateIndoorRoomStatus(GBState *gb, uint8_t new_status) {
    if (!gb) return;

    uint16_t hl;
    uint8_t room = gb_read(gb, hMapRoom);
    uint8_t map_id = gb_read(gb, hMapId);

    if (map_id == MAP_COLOR_DUNGEON) {
        hl = wColorDungeonRoomStatus + room;
    } else {
        uint16_t offset = room;
        if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
            offset += 0x100;
        }
        hl = wIndoorARoomStatus + offset;
    }

    uint8_t status = gb_read(gb, hl) | new_status;
    gb_write(gb, hl, status);
    gb_write(gb, hRoomStatus, status);
}

void LoadObject_OpenDoorTop(GBState *gb, uint8_t pos) {
    if (!gb) return;

    UpdateIndoorRoomStatus(gb, ROOM_STATUS_DOOR_OPEN_UP);
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, OpenDoorTopObjectIds_Addr);
}

void LoadObject_OpenDoorBottom(GBState *gb, uint8_t pos) {
    if (!gb) return;

    UpdateIndoorRoomStatus(gb, ROOM_STATUS_DOOR_OPEN_DOWN);
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, OpenDoorBottomObjectIds_Addr);
}

void LoadObject_OpenDoorLeft(GBState *gb, uint8_t pos) {
    if (!gb) return;

    UpdateIndoorRoomStatus(gb, ROOM_STATUS_DOOR_OPEN_LEFT);
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, VerticalObjectOffsets_Addr, OpenDoorLeftObjectIds_Addr);
}

void LoadObject_OpenDoorRight(GBState *gb, uint8_t pos) {
    if (!gb) return;

    UpdateIndoorRoomStatus(gb, ROOM_STATUS_DOOR_OPEN_RIGHT);
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, VerticalObjectOffsets_Addr, OpenDoorRightObjectIds_Addr);
}

void LoadObject_KeyDoorTop(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_KEY_TOP, pos);
    if (gb_read(gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_UP) {
        LoadObject_OpenDoorTop(gb, pos);
        return;
    }

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, KeyDoorTopObjectIds_Addr);
}

void LoadObject_KeyDoorBottom(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_KEY_BOTTOM, pos);
    if (gb_read(gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_DOWN) {
        LoadObject_OpenDoorBottom(gb, pos);
        return;
    }

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, KeyDoorBottomObjectIds_Addr);
}

void LoadObject_KeyDoorLeft(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_KEY_LEFT, pos);
    if (gb_read(gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_LEFT) {
        LoadObject_OpenDoorLeft(gb, pos);
        return;
    }

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, VerticalObjectOffsets_Addr, KeyDoorLeftObjectIds_Addr);
}

void LoadObject_KeyDoorRight(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_KEY_RIGHT, pos);
    if (gb_read(gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_RIGHT) {
        LoadObject_OpenDoorRight(gb, pos);
        return;
    }

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, VerticalObjectOffsets_Addr, KeyDoorRightObjectIds_Addr);
}

void LoadObject_ShutterDoorTop(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_SHUTTER_TOP, pos);
    uint8_t mask = gb_read(gb, wShutterDoorsMask) | DOOR_TYPE_SHUTTER_TOP_BIT;
    gb_write(gb, wShutterDoorsMask, mask);
    gb_write(gb, wShutterDoorsMask2, mask);
    LoadObject_OpenDoorTop(gb, pos);
}

void LoadObject_ShutterDoorBottom(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_SHUTTER_BOTTOM, pos);
    uint8_t mask = gb_read(gb, wShutterDoorsMask) | DOOR_TYPE_SHUTTER_BOTTOM_BIT;
    gb_write(gb, wShutterDoorsMask, mask);
    gb_write(gb, wShutterDoorsMask2, mask);
    LoadObject_OpenDoorBottom(gb, pos);
}

void LoadObject_ShutterDoorLeft(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_SHUTTER_LEFT, pos);
    uint8_t mask = gb_read(gb, wShutterDoorsMask) | DOOR_TYPE_SHUTTER_LEFT_BIT;
    gb_write(gb, wShutterDoorsMask, mask);
    gb_write(gb, wShutterDoorsMask2, mask);
    LoadObject_OpenDoorLeft(gb, pos);
}

void LoadObject_ShutterDoorRight(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_SHUTTER_RIGHT, pos);
    uint8_t mask = gb_read(gb, wShutterDoorsMask) | DOOR_TYPE_SHUTTER_RIGHT_BIT;
    gb_write(gb, wShutterDoorsMask, mask);
    gb_write(gb, wShutterDoorsMask2, mask);
    LoadObject_OpenDoorRight(gb, pos);
}

void LoadObject_BossDoor(GBState *gb, uint8_t pos) {
    if (!gb) return;

    MakeListOfDoorPositions(gb, DOOR_TYPE_BOSS_TOP, pos);
    if (gb_read(gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_UP) {
        LoadObject_OpenDoorTop(gb, pos);
        return;
    }

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, BossDoorObjectIds_Addr);
}

void LoadObject_StairsDoor(GBState *gb, uint8_t pos) {
    if (!gb) return;

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, VerticalObjectOffsets_Addr, StairsDoorObjectIds_Addr);
}

void LoadObject_RevolvingDoor(GBState *gb, uint8_t pos) {
    if (!gb) return;

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, RevolvingDoorObjectIds_Addr);
}

void LoadObject_OneWayArrow(GBState *gb, uint8_t pos) {
    if (!gb) return;

    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, OneWayArrowObjectIds_Addr);
}

void LoadObject_DungeonEntrance(GBState *gb, uint8_t pos) {
    if (!gb) return;

    UpdateIndoorRoomStatus(gb, ROOM_STATUS_DOOR_OPEN_DOWN);
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, DungeonEntranceObjectOffsets_Addr, DungeonEntranceObjectIds_Addr);
}

void LoadObject_IndoorEntrance(GBState *gb, uint8_t pos) {
    if (!gb) return;

    uint8_t map_id = gb_read(gb, hMapId);
    if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
        if (gb_read(gb, hMapRoom) == ROOM_INDOOR_B_KANALET_MAIN_ENTRANCE) {
            if (gb_read(gb, wHasStolenFromShop) != 0) {
                LoadObject_ShutterDoorBottom(gb, pos);
                return;
            }
        }
    }

    UpdateIndoorRoomStatus(gb, 0x01); /* ROOM_STATUS_DOOR_OPEN_RIGHT */
    uint16_t hl = ObjectPositionToRoomObjectAddress(pos);
    CopyIndoorsMacroObjectsToRoom(gb, hl, HorizontalObjectOffsets_Addr, EntranceObjectIds_Addr);
}

bool DispatchIndoorDoorObject(GBState *gb, uint8_t obj_type, uint8_t pos) {
    if (!gb) return false;

    switch (obj_type) {
        case OBJECT_DOOR_TYPE_KEY_TOP:
            LoadObject_KeyDoorTop(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_KEY_BOTTOM:
            LoadObject_KeyDoorBottom(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_KEY_LEFT:
            LoadObject_KeyDoorLeft(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_KEY_RIGHT:
            LoadObject_KeyDoorRight(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_SHUTTER_TOP:
            LoadObject_ShutterDoorTop(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_SHUTTER_BOTTOM:
            LoadObject_ShutterDoorBottom(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_SHUTTER_LEFT:
            LoadObject_ShutterDoorLeft(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_SHUTTER_RIGHT:
            LoadObject_ShutterDoorRight(gb, pos);
            return true;
        case OBJECT_OPEN_DOOR_TOP:
            LoadObject_OpenDoorTop(gb, pos);
            return true;
        case OBJECT_OPEN_DOOR_BOTTOM:
            LoadObject_OpenDoorBottom(gb, pos);
            return true;
        case OBJECT_OPEN_DOOR_LEFT:
            LoadObject_OpenDoorLeft(gb, pos);
            return true;
        case OBJECT_OPEN_DOOR_RIGHT:
            LoadObject_OpenDoorRight(gb, pos);
            return true;
        case OBJECT_DOOR_TYPE_BOSS_TOP:
            LoadObject_BossDoor(gb, pos);
            return true;
        case OBJECT_STAIRS_DOOR:
            LoadObject_StairsDoor(gb, pos);
            return true;
        case OBJECT_FLIP_WALL:
            LoadObject_RevolvingDoor(gb, pos);
            return true;
        case OBJECT_ONE_WAY_ARROW:
            LoadObject_OneWayArrow(gb, pos);
            return true;
        case OBJECT_DUNGEON_ENTRANCE:
            LoadObject_DungeonEntrance(gb, pos);
            return true;
        case OBJECT_INDOOR_ENTRANCE:
            LoadObject_IndoorEntrance(gb, pos);
            return true;
        default:
            return false;
    }
}


void ExpandOverworldObjectMacro(GBState *gb, uint8_t obj_type, uint8_t pos) {
    if (!gb) return;
    /* Hook / trampoline for Bank $24:7578 */
}

size_t LoadRoomObject(GBState *gb, uint16_t stream_addr) {
    if (!gb) return 0;

    gb_write(gb, hMultiPurpose0, 0);

    uint8_t byte0 = gb_read(gb, stream_addr);
    uint8_t pos;
    uint8_t obj_type;
    size_t bytes_consumed;

    if ((byte0 & 0x80) != 0 && (byte0 & 0x10) == 0) {
        /* Three-bytes object: byte0 encodes direction and length */
        gb_write(gb, hMultiPurpose0, byte0);
        pos = gb_read(gb, stream_addr + 1);
        obj_type = gb_read(gb, stream_addr + 2);
        bytes_consumed = 3;
    } else {
        /* Two-bytes object: byte0 is position (YX) */
        pos = byte0;
        obj_type = gb_read(gb, stream_addr + 1);
        bytes_consumed = 2;
    }

    uint8_t room_status = gb_read(gb, hRoomStatus);
    uint8_t is_indoor = gb_read(gb, wIsIndoor);

    if (!is_indoor) {
        /* Overworld */
        if (obj_type >= OBJECT_MACROS_SECTION) {
            ExpandOverworldObjectMacro(gb, obj_type, pos);
            SetBankForRoom(gb);
            return bytes_consumed;
        }

        /* Non-macro Overworld object */
        if (obj_type == OBJECT_WATERFALL) {
            gb_write(gb, wC50E, OBJECT_WATERFALL);
        }

        /* Weather vane */
        if (obj_type == OBJECT_WEATHER_VANE_BASE) {
            if ((pos & 0x20) != 0) {
                obj_type = OBJECT_GROUND_STAIRS;
            }
        } else if (obj_type == OBJECT_WEATHER_VANE_TOP) {
            if ((pos & 0x20) != 0) {
                obj_type = OBJECT_WEATHER_VANE_BASE;
            }
        } else if (obj_type == OBJECT_WEATHER_VANE_ABOVE) {
            if ((pos & 0x20) != 0) {
                obj_type = OBJECT_WEATHER_VANE_TOP;
            }
        }

        /* Monkey bridge */
        if (obj_type == OBJECT_MONKEY_BRIDGE_TOP ||
            obj_type == OBJECT_MONKEY_BRIDGE_MIDDLE ||
            obj_type == OBJECT_MONKEY_BRIDGE_BOTTOM) {
            if ((room_status & 0x10) != 0) {
                obj_type = OBJECT_MONKEY_BRIDGE_BUILT;
            }
        }

        /* Closed gate */
        if (obj_type == OBJECT_CLOSED_GATE) {
            if ((room_status & 0x10) != 0) {
                obj_type = OBJECT_CAVE_DOOR;
            }
        }

        /* Bombable cave door */
        if (obj_type == OBJECT_BOMBABLE_CAVE_DOOR) {
            if ((room_status & 0x04) != 0) {
                obj_type = OBJECT_ROCKY_CAVE_DOOR;
            }
        }

        /* Bush masking cave entrance */
        if (obj_type == OBJECT_BUSH_GROUND_STAIRS) {
            if ((room_status & 0x10) != 0) {
                uint8_t map_room = gb_read(gb, hMapRoom);
                if (map_room == UNKNOWN_ROOM_75 ||
                    map_room == ROOM_OW_RIGHT_OF_EGG ||
                    map_room == UNKNOWN_ROOM_AA ||
                    map_room == UNKNOWN_ROOM_4A) {
                    obj_type = OBJECT_GROUND_STAIRS;
                }
            }
        }

        gb_write(gb, hMultiPurpose9, obj_type);

        /* Door warp data configuration */
        if (obj_type == OBJECT_CLOSED_GATE ||
            obj_type == OBJECT_ROCKY_CAVE_DOOR ||
            obj_type == 0xCB ||
            obj_type == OBJECT_BOMBABLE_CAVE_DOOR ||
            obj_type == 0x61 ||
            obj_type == OBJECT_GROUND_STAIRS ||
            obj_type == 0xC5 ||
            obj_type == 0xE2 ||
            obj_type == OBJECT_CAVE_DOOR) {
            uint8_t c19c = gb_read(gb, wC19C);
            gb_write(gb, wC19C, (c19c + 1) & 0x03);
            gb_write(gb, wWarpPositions + c19c, pos);
        }

        /* Configure stairs if 0xC5 or OBJECT_GROUND_STAIRS */
        if (obj_type == 0xC5 || obj_type == OBJECT_GROUND_STAIRS) {
            gb_write(gb, hStaircase, STAIRCASE_INACTIVE);
            gb_write(gb, hStaircasePosY, (pos & 0xF0) + 0x10);
            gb_write(gb, hStaircasePosX, ((pos & 0x0F) << 4) + 0x08);
        }
    } else {
        /* Indoor */
        if (obj_type >= OBJECT_DOOR_TYPE_KEY_TOP) {
            DispatchIndoorDoorObject(gb, obj_type, pos);
            return bytes_consumed;
        }

        gb_write(gb, hMultiPurpose9, obj_type);

        /* Conveyor belt */
        if (obj_type >= OBJECT_CONVEYOR_BOTTOM && obj_type < OBJECT_TRENDY_GAME_BORDER) {
            uint8_t count = gb_read(gb, wConveyorBeltsCount);
            gb_write(gb, wConveyorBeltsCount, count + 1);
        }

        /* Unlit torch */
        if (obj_type == OBJECT_TORCH_UNLIT) {
            gb_write(gb, wObjectAffectingBGPalette, 0);
            if (gb_read(gb, hMapRoom) == UNKNOWN_ROOM_C4 && obj_type != 0) {
                uint8_t torches = gb_read(gb, wTorchesCount);
                gb_write(gb, wTorchesCount, torches + 1);
                gb_write(gb, wObjectAffectingBGPalette, obj_type);
                uint8_t c3cd = gb_read(gb, wC3CD);
                gb_write(gb, wC3CD, c3cd + 4);
                if (gb_read(gb, hIsGBC) == 0) {
                    gb_write(gb, wTransitionSequenceCounter, 4);
                }
            }
        }

        /* Switches and movable blocks */
        if (obj_type == OBJECT_POT_WITH_SWITCH || obj_type == OBJECT_SWITCH_BUTTON) {
            gb_write(gb, wRoomSwitchableObject, ROOM_SWITCHABLE_OBJECT_SWITCH_BUTTON);
        } else if (obj_type == OBJECT_RAISED_BLOCK || obj_type == OBJECT_LOWERED_BLOCK) {
            gb_write(gb, wRoomSwitchableObject, ROOM_SWITCHABLE_OBJECT_MOBILE_BLOCK);
        }

        /* Bombable wall top */
        if (obj_type == OBJECT_BOMBABLE_WALL_TOP || obj_type == OBJECT_HIDDEN_BOMBABLE_WALL_TOP) {
            if ((room_status & 0x04) != 0) {
                obj_type = OBJECT_BOMBED_PASSAGE_VERTICAL;
            }
        }

        /* Bombable wall bottom */
        if (obj_type == OBJECT_BOMBABLE_WALL_BOTTOM || obj_type == OBJECT_HIDDEN_BOMBABLE_WALL_BOTTOM) {
            if ((room_status & 0x08) != 0) {
                obj_type = OBJECT_BOMBED_PASSAGE_VERTICAL;
            }
        }

        /* Bombable wall left */
        if (obj_type == OBJECT_BOMBABLE_WALL_LEFT || obj_type == OBJECT_HIDDEN_BOMBABLE_WALL_LEFT) {
            if ((room_status & 0x02) != 0) {
                obj_type = OBJECT_BOMBED_PASSAGE_HORIZONTAL;
            }
        }

        /* Bombable wall right */
        if (obj_type == OBJECT_BOMBABLE_WALL_RIGHT || obj_type == OBJECT_HIDDEN_BOMBABLE_WALL_RIGHT) {
            if ((room_status & 0x01) != 0) {
                obj_type = OBJECT_BOMBED_PASSAGE_HORIZONTAL;
            }
        }

        /* Chest (open object) */
        if (obj_type == OBJECT_CHEST_OPEN) {
            if ((room_status & 0x10) == 0) {
                obj_type = gb_read(gb, hMultiPurposeH);
            }
        }

        /* Hidden stairs */
        if (obj_type == OBJECT_HIDDEN_STAIRS_DOWN) {
            if ((room_status & 0x10) == 0) {
                return bytes_consumed; /* Return without loading this object */
            }
        }

        /* Stairs */
        if (obj_type == OBJECT_STAIRS_DOWN || obj_type == OBJECT_HIDDEN_STAIRS_DOWN || obj_type == OBJECT_STAIRS_UP) {
            gb_write(gb, hStaircase, STAIRCASE_INACTIVE);
            gb_write(gb, hStaircasePosY, (pos & 0xF0) + 0x10);
            gb_write(gb, hStaircasePosX, ((pos & 0x0F) << 4) + 0x08);
        }

        /* Raised fences */
        if (obj_type == OBJECT_RAISED_FENCE_BOTTOM || obj_type == OBJECT_RAISED_FENCE_TOP) {
            if ((room_status & 0x10) == 0) {
                obj_type = OBJECT_WALL_TOP;
            }
        } else if (obj_type == OBJECT_RAISED_FENCE_LEFT || obj_type == OBJECT_RAISED_FENCE_RIGHT) {
            if ((room_status & 0x10) == 0) {
                obj_type = OBJECT_WALL_BOTTOM;
            }
        }

        /* Breakable objects */
        if (gb_read(gb, hMapId) >= MAP_CAVE_B) {
            if (obj_type == OBJECT_BOMBABLE_BLOCK) {
                if ((room_status & 0x40) != 0) {
                    obj_type = OBJECT_FLOOR_0D;
                }
            }
        }
        if (obj_type == OBJECT_KEYHOLE_BLOCK) {
            if ((room_status & 0x40) != 0) {
                obj_type = OBJECT_FLOOR_0D;
            }
        }
    }

    /* Shared post-processing (breakableObjectEnd -> closedChestEnd) */
    if (obj_type == OBJECT_CHEST_CLOSED) {
        if ((room_status & 0x10) != 0) {
            obj_type = OBJECT_CHEST_OPEN;
        }
    }

    /* Final copy */
    uint8_t multi0 = gb_read(gb, hMultiPurpose0);
    if (multi0 == 0) {
        CopyObjectToActiveRoomMap(gb, pos, obj_type);
    } else {
        uint8_t count = multi0 & 0x0F;
        uint16_t hl = wRoomObjects + pos;
        FillRoomWithConsecutiveObjects(gb, hl, obj_type, count);
    }

    return bytes_consumed;
}
