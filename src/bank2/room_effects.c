#include "bank2/room_effects.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/memory.h"
#include "constants/sfx.h"

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
