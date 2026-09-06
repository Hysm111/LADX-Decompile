#include "home/entities.h"
#include "home/bank.h"
#include "constants/memory.h"
#include "constants/hardware.h"
#include "constants/entities.h"

uint8_t IsZero(GBState *gb, uint16_t hl, uint16_t bc) {
    if (!gb) return 0;
    return gb_read(gb, (uint16_t)(hl + bc));
}

uint8_t GetEntitySlowTransitionCountdown(GBState *gb, uint16_t entity_index) {
    return IsZero(gb, wEntitiesSlowTransitionCountdownTable, entity_index);
}

uint8_t GetEntityPrivateCountdown1(GBState *gb, uint16_t entity_index) {
    return IsZero(gb, wEntitiesPrivateCountdown1Table, entity_index);
}

uint8_t GetEntityTransitionCountdown(GBState *gb, uint16_t entity_index) {
    return IsZero(gb, wEntitiesTransitionCountdownTable, entity_index);
}

void DecrementEntityIgnoreHitsCountdown(GBState *gb, uint16_t entity_index) {
    if (!gb) return;
    uint16_t addr = (uint16_t)(wEntitiesIgnoreHitsCountdownTable + entity_index);
    uint8_t val = gb_read(gb, addr);
    if (val != 0) {
        gb_write(gb, addr, (uint8_t)(val - 1));
    }
}

void CreateTradingItemEntity(GBState *gb, uint16_t (*spawn_func)(GBState *, uint8_t entity_type)) {
    if (!gb) return;

    uint16_t de = 0;
    if (spawn_func) {
        de = spawn_func(gb, ENTITY_TRADING_ITEM);
    }

    uint8_t link_x = gb_read(gb, hLinkPositionX);
    uint8_t link_y = gb_read(gb, hLinkPositionY);

    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), link_x);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), link_y);
}

uint16_t SpawnNewEntity_trampoline(GBState *gb, uint8_t entity_type, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, 0x03);
    uint16_t de = 0;
    if (spawn_new_entity) {
        de = spawn_new_entity(gb, entity_type);
    }
    ReloadSavedBank(gb);
    return de;
}

uint16_t SpawnNewEntityInRange_trampoline(GBState *gb, uint8_t entity_type, uint16_t (*spawn_in_range)(GBState *, uint8_t)) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, 0x03);
    uint16_t de = 0;
    if (spawn_in_range) {
        de = spawn_in_range(gb, entity_type);
    }
    ReloadSavedBank(gb);
    return de;
}
