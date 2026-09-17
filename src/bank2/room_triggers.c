#include "bank2/room_triggers.h"
#include "bank2/room_effects.h"
#include "bank2/room_events.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "home/room.h"

void CheckKillSidescrollBossTrigger(GBState *gb) {
    if (!gb) return;

    uint16_t address = gb_read_hram(gb, hMapId) == MAP_EAGLES_TOWER
        ? wIndoorBRoomStatus + 0xE8 : wIndoorARoomStatus + 0xFF;
    if ((gb_read(gb, address) & ROOM_STATUS_EVENT_2) != 0) {
        MarkTriggerAsResolved(gb);
    }
}

void CheckLightTorchesTrigger(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wC1A2) == 2) {
        MarkTriggerAsResolved(gb);
    }
}

void CheckStepOnButtonTrigger(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wSwitchButtonPressed) != 0) {
        MarkTriggerAsResolved(gb);
    }
}

void CheckKillInOrderTrigger(GBState *gb) {
    if (!gb) return;

    for (uint8_t index = 0; index < 3; index++) {
        if (gb_read(gb, (uint16_t)(wKillOrder + index)) != index) return;
    }
    MarkTriggerAsResolved(gb);
}

void CheckKillEnemiesTrigger(GBState *gb) {
    if (!gb) return;

    for (int slot = MAX_ENTITIES - 1; slot >= 0; slot--) {
        if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot)) != 0 &&
            (gb_read(gb, (uint16_t)(wEntitiesOptions1Table + slot)) &
             ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL) == 0) {
            return;
        }
    }
    if (gb_read_hram(gb, hMultiPurpose0) == TRIGGER_KILL_SPECIALS) {
        if (gb_read(gb, wD460) == 0 || gb_read(gb, wEnemyWasKilled) != 0) return;
    }
    MarkTriggerAsResolved(gb);
}

void CheckAnswerTunicsTrigger(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wRoomEventEffectExecuted) != 0) return;

    gb_write_hram(gb, hMultiPurpose0, 0);
    bool room_12 = gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_12;
    for (uint16_t slot = 0; slot < MAX_ENTITIES; slot++) {
        uint8_t type = gb_read(gb, (uint16_t)(wEntitiesTypeTable + slot));
        if (room_12) {
            if (type != 0xF6 && type != 0xF7) continue;
        } else {
            if (type != 0xEF && type != 0xF0 && type != 0xF1) continue;
        }
        if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot)) == 0) continue;
        uint16_t table = room_12 ? wEntitiesStateTable : wEntitiesSpriteVariantTable;
        if (gb_read(gb, (uint16_t)(table + slot)) != (room_12 ? 4 : 8)) continue;
        gb_write_hram(gb, hMultiPurpose0, (uint8_t)(gb_read_hram(gb, hMultiPurpose0) + 1));
    }

    if (room_12) {
        if (gb_read_hram(gb, hMultiPurpose0) != 2) return;
        MarkTriggerAsResolved(gb);
        if (!EventEffectGuard(gb)) return;
        uint16_t address = GetRoomStatusAddress(gb);
        uint8_t status = (uint8_t)(gb_read(gb, address) | ROOM_STATUS_EVENT_1);
        gb_write(gb, address, status);
        gb_write_hram(gb, hRoomStatus, status);
        return;
    }

    uint8_t required = gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_0A ? 9 : 4;
    if (gb_read_hram(gb, hMultiPurpose0) != required) return;
    MarkTriggerAsResolved(gb);
    uint8_t room = gb_read_hram(gb, hMapRoom);
    if (room == ROOM_OW_MARIN_BRIDGE) return;
    if (room == UNKNOWN_ROOM_0A) {
        if (!EventEffectGuard(gb)) return;
        OpenShutterDoorsEffectHandler(gb);
        /* This branch updates only the color-dungeon saved byte, not HRAM. */
        uint16_t address = (uint16_t)(wColorDungeonRoomStatus + gb_read_hram(gb, hMapRoom));
        gb_write(gb, address, (uint8_t)(gb_read(gb, address) | ROOM_STATUS_EVENT_1));
        return;
    }
    RevealChestEffectHandler(gb);
}

bool CheckTriggersResolution(GBState *gb, uint8_t event) {
    uint8_t trigger = event & EVENT_TRIGGER_MASK;
    /* Invalid IDs index instruction bytes in the ROM, not a return entry.
     * Reject unsupported C inputs rather than inventing their execution. */
    if (!gb || trigger < 1 || trigger > 16) return false;

    gb_write_hram(gb, hMultiPurpose0, trigger);
    switch (trigger) {
    case 1:
    case 8:
        CheckKillEnemiesTrigger(gb);
        break;
    case 3:
        CheckStepOnButtonTrigger(gb);
        break;
    case 5:
        CheckLightTorchesTrigger(gb);
        break;
    case 6:
        CheckKillInOrderTrigger(gb);
        break;
    case 10:
        CheckKillSidescrollBossTrigger(gb);
        break;
    case 16:
        CheckAnswerTunicsTrigger(gb);
        break;
    default:
        /* Declared Events.return entries: 2, 4, 7, 9, 11..15. */
        break;
    }
    return true;
}

bool ExecuteRoomTriggersAndEffects(GBState *gb,
                                   uint16_t (*spawn_key)(GBState *, uint8_t),
                                   uint16_t (*spawn_fairy)(GBState *, uint8_t)) {
    if (!gb) return false;
    uint8_t event = gb_read(gb, wRoomEvent);
    if (event == 0) return true;

    uint8_t trigger = event & EVENT_TRIGGER_MASK;
    if (trigger < 1 || trigger > 16 || !spawn_key || !spawn_fairy) return false;
    CheckTriggersResolution(gb, event);

    /* A checker can execute an effect itself and clear wRoomEvent. The
     * assembly reloads it here, so do not dispatch from the saved event. */
    switch ((gb_read(gb, wRoomEvent) & EVENT_EFFECT_MASK) >> 5) {
    case 0:
        break;
    case 1:
        OpenShutterDoorsEffectHandler(gb);
        break;
    case 2:
        KillAllEnemiesEffectHandler(gb);
        break;
    case 3:
        RevealChestEffectHandler(gb);
        break;
    case 4:
        DropKeyEffectHandler(gb, spawn_key);
        break;
    case 5:
        RevealStaircaseEffectHandler(gb);
        break;
    case 6:
        ClearMidbossEffectHandler(gb);
        break;
    case 7:
        DropFairyEffectHandler(gb, spawn_fairy);
        break;
    }
    return true;
}
