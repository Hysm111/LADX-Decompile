#include "home/room.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/maps.h"
#include "constants/rooms.h"
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
