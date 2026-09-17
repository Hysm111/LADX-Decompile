#include "bank2/room_effects.h"
#include "bank2/room_events.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "home/entities.h"
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
