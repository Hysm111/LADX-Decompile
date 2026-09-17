#include "bank2/room_effects.h"
#include "bank2/room_events.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "home/bank.h"
#include "home/entities.h"
#include "home/gameplay.h"
#include "home/vfx.h"

bool EventEffectGuard(GBState *gb) {
    if (!gb) return false;

    /* Follow the branch instructions, not the contradictory assembly prose. */
    if ((gb_read_hram(gb, hRoomStatus) & ROOM_STATUS_EVENT_1) != 0 ||
        gb_read(gb, wRoomEventEffectExecuted) == 0) {
        return false;
    }

    gb_write(gb, wRoomEvent, 0);
    return true;
}

void KillAllEnemiesEffectHandler(GBState *gb) {
    if (!EventEffectGuard(gb)) return;

    for (int slot = MAX_ENTITIES - 1; slot >= 0; slot--) {
        uint16_t physics_addr = (uint16_t)(wEntitiesPhysicsFlagsTable + slot);
        uint8_t physics = gb_read(gb, physics_addr);
        if ((physics & ENTITY_PHYSICS_HARMLESS) != 0) {
            continue;
        }

        uint16_t status_addr = (uint16_t)(wEntitiesStatusTable + slot);
        if (gb_read(gb, status_addr) < ENTITY_STATUS_ACTIVE) {
            continue;
        }

        gb_write(gb, status_addr, ENTITY_STATUS_DYING);
        gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown3Table + slot), 0x1F);
        gb_write(gb, physics_addr, (uint8_t)((physics & ENTITY_PHYSICS_MASK) | 2));
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_ENEMY_DESTROYED);
    }
}

void DropFairyEffectHandler(GBState *gb,
                            uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb || !spawn_new_entity) return;
    if (!EventEffectGuard(gb)) return;

    uint16_t de = SpawnNewEntity_trampoline(gb, ENTITY_DROPPABLE_FAIRY,
                                          spawn_new_entity);
    /* No carry check at 02:5DCA: even failed allocation uses the raw DE. */
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), 0x88);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), 0x30);
    gb_write(gb, (uint16_t)(wEntitiesSlowTransitionCountdownTable + de), 0x80);
    gb_write_hram(gb, hMultiPurpose0, 0x88);
    gb_write_hram(gb, hMultiPurpose1, 0x30);
    MakeEffectObjectAppear(gb, TRANSCIENT_VFX_POOF);
}

void RevealStaircaseEffectHandler(GBState *gb) {
    if (!EventEffectGuard(gb)) return;

    gb_write_hram(gb, hMultiPurpose0, 0x88);
    gb_write_hram(gb, hMultiPurpose1, 0x20);
    MakeEffectObjectAppear(gb, TRANSCIENT_VFX_STAIRS_APPEARS);
}

void MakeEffectObjectAppear(GBState *gb, uint8_t vfx_type) {
    if (!gb) return;

    AddTranscientVfx(gb, vfx_type);
    uint16_t address = GetRoomStatusAddress(gb);
    uint8_t status = (uint8_t)(gb_read(gb, address) | ROOM_STATUS_EVENT_1);
    gb_write(gb, address, status);
    gb_write_hram(gb, hRoomStatus, status);
}

void DropKeyEffectHandler(GBState *gb,
                          uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb || !spawn_new_entity) return;
    if (!EventEffectGuard(gb)) return;

    /* The assembly tests only the room byte, not the current map. */
    if (gb_read_hram(gb, hMapRoom) == ROOM_INDOOR_A_ANGLERS_TUNNEL_KEY_DROP) {
        uint16_t address = GetRoomStatusAddress(gb);
        uint8_t status = (uint8_t)(gb_read(gb, address) | ROOM_STATUS_EVENT_1);
        gb_write(gb, address, status);
        gb_write_hram(gb, hRoomStatus, status);
    }
    label_002_5425(gb, spawn_new_entity);
}

void ClearMidbossEffectHandler(GBState *gb) {
    if (!gb) return;

    uint16_t address = (uint16_t)(wHasInstrument1 + gb_read_hram(gb, hMapId));
    if ((gb_read(gb, address) & 1) != 0) return;
    OpenShutterDoorsEffectHandler(gb);
}

void OpenShutterDoorsEffectHandler(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wShutterDoorEventExecuted) == 0) {
        CloseDoors(gb);
    }
    if (gb_read(gb, wRoomEventEffectExecuted) == 0) return;

    if (gb_read(gb, wRoomEvent) == (TRIGGER_KILL_ALL_ENEMIES | EFFECT_CLEAR_MIDBOSS)) {
        uint8_t map_id = gb_read_hram(gb, hMapId);
        uint16_t instrument_address = (uint16_t)(wHasInstrument1 + map_id);
        gb_write(gb, instrument_address, (uint8_t)(gb_read(gb, instrument_address) | 1));

        /* Unlike GetRoomStatusAddress, this routing ignores wIsIndoor. */
        uint16_t base = wIndoorARoomStatus;
        if (map_id == MAP_COLOR_DUNGEON) {
            base = wColorDungeonRoomStatus;
        } else if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
            base = wIndoorBRoomStatus;
        }
        uint16_t address = (uint16_t)(base + gb_read_hram(gb, hMapRoom));
        gb_write(gb, address, (uint8_t)(gb_read(gb, address) | ROOM_STATUS_EVENT_2));
        /* The original does not synchronize hRoomStatus here. */
        gb_write_hram(gb, hJingle, JINGLE_DUNGEON_WARP_APPEAR);
    }

    if (gb_read(gb, wShutterDoorEventExecuted) == 0) return;
    gb_write(gb, wRoomEvent, 0);
    gb_write(gb, wEnqueueDoorsOpening, 1);
    EnqueueDoorUnlockedSfx(gb);
}

void CloseDoors(GBState *gb) {
    if (!gb) return;

    if ((uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x11) >= 0x7E) return;
    if ((uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x16) >= 0x5E) return;
    if (gb_read(gb, wRoomEventEffectExecuted) != 0) return;

    gb_write(gb, wEnqueueDoorsClosing, 1);
    gb_write(gb, wShutterDoorEventExecuted, 1);
    gb_write(gb, wC111, 4);
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_DOOR_CLOSED);
}

/* ========================================================================= */
/* Chest and staircase object reveal (02:5EA3 - 02:5F9E)                     */
/* ========================================================================= */

/* ChestTileIds.dmg / .cgb (02:5EA3). On CGB both chest halves use the same
 * tiles, horizontally flipped by the attribute data. */
const uint8_t ChestTileIds[8] = {
    0x60, 0x70,
    0x61, 0x71,
    0x60, 0x70,
    0x60, 0x70
};

/* StaircaseTileIds (02:5F54). The trailing four zero bytes are unused in the
 * original ROM: the chest has DMG and CGB variants, the staircase does not. */
const uint8_t StaircaseTileIds[8] = {
    0x6A, 0x7A,
    0x6B, 0x7B,
    0x00, 0x00,
    0x00, 0x00
};

void RevealChestEffectHandler(GBState *gb) {
    if (!gb) return;
    if (!EventEffectGuard(gb)) return;

    gb_write_hram(gb, hMultiPurpose0, 0x88);
    /* Branch-faithful: y' >= 0x10 selects 0x30 regardless of the computed
     * value; only y' < 0x10 with x' < 0x20 reaches 0x40. */
    uint8_t y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x30 + 0x08);
    if (y >= 0x10) {
        y = 0x30;
    } else {
        uint8_t x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x88 + 0x10);
        y = (x < 0x20) ? 0x40 : 0x30;
    }
    gb_write_hram(gb, hMultiPurpose1, y);
    AddTranscientVfx(gb, TRANSCIENT_VFX_CHEST_APPEARS);
}

/* Shared tail from 02:5F27: emit the 10-byte BG column command for the two
 * columns of the revealed object, then set palette 2 on CGB. */
static void EmitObjectDrawCommand(GBState *gb, const uint8_t *tiles,
                                  void (*get_bg_attr_addr)(GBState *)) {
    uint8_t cmd_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + cmd_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(cmd_size + 0x0A));

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, bg_low);
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, *tiles++);
    gb_write(gb, hl++, *tiles++);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, (uint8_t)(bg_low + 1));
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, *tiles++);
    gb_write(gb, hl++, *tiles);
    gb_write(gb, hl, 0x00);

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_91D(gb, 0x02, get_bg_attr_addr);
    }
}

void func_002_5ED3(GBState *gb, void (*get_bg_attr_addr)(GBState *)) {
    if (!gb) return;

    /* Same overlap window as RevealChestEffectHandler, selecting the object
     * row instead of the VFX row. */
    uint8_t top = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x30 + 0x08);
    if (top >= 0x10) {
        top = 0x20;
    } else {
        uint8_t x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x88 + 0x10);
        top = (x < 0x20) ? 0x30 : 0x20;
    }
    gb_write_hram(gb, hIntersectedObjectTop, top);
    gb_write_hram(gb, hIntersectedObjectLeft, 0x80);

    uint8_t object_index = (uint8_t)((top & 0xF0) | 0x08);
    gb_write(gb, (uint16_t)(wRoomObjects + object_index), 0xA0);
    gb_write(gb, wDDD8, 0xA0);
    GetIntersectedObjectBGAddress(gb);

    const uint8_t *tiles = ChestTileIds;
    if (gb_read_hram(gb, hIsGBC) != 0) {
        tiles = &ChestTileIds[4];
    }
    EmitObjectDrawCommand(gb, tiles, get_bg_attr_addr);
}

void func_002_5F5C(GBState *gb, void (*get_bg_attr_addr)(GBState *)) {
    if (!gb) return;

    gb_write_hram(gb, hStaircase, STAIRCASE_INACTIVE);
    gb_write_hram(gb, hIntersectedObjectTop, 0x10);
    gb_write_hram(gb, hStaircasePosY, 0x20);
    gb_write_hram(gb, hIntersectedObjectLeft, 0x80);
    gb_write_hram(gb, hStaircasePosX, 0x88);

    uint8_t object_index =
        (uint8_t)((gb_read_hram(gb, hIntersectedObjectTop) & 0xF0) | 0x08);
    gb_write(gb, (uint16_t)(wRoomObjects + object_index), 0xBE);
    gb_write(gb, wDDD8, 0xBE);
    GetIntersectedObjectBGAddress(gb);

    EmitObjectDrawCommand(gb, StaircaseTileIds, get_bg_attr_addr);
}
