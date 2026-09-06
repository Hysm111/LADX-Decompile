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

void AnimateEntitiesAndRestoreBank(GBState *gb, uint8_t bank, void (*animate_entities)(GBState *)) {
    if (!gb) return;

    if (animate_entities) {
        animate_entities(gb);
    }
    SwitchBank(gb, bank);
}

void AnimateEntitiesAndRestoreBank17(GBState *gb, void (*animate_entities)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    AnimateEntitiesAndRestoreBank(gb, 0x17, animate_entities);
}

void AnimateEntitiesAndRestoreBank01(GBState *gb, void (*animate_entities)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    AnimateEntitiesAndRestoreBank(gb, 0x01, animate_entities);
}

void AnimateEntitiesAndRestoreBank02(GBState *gb, void (*animate_entities)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    AnimateEntitiesAndRestoreBank(gb, 0x02, animate_entities);
}

uint8_t CanBowWowEatEntity(GBState *gb, uint8_t entity_type) {
    if (!gb) return 0;

    gb_write(gb, rSelectROMBank, 0x14);
    uint8_t val = gb_read(gb, (uint16_t)(BowWowEatableEntitiesTable + entity_type));
    gb_write(gb, rSelectROMBank, 0x05);
    return val;
}

void label_3935(GBState *gb, void (*func_019_7c50)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, 0x19);
    if (func_019_7c50) {
        func_019_7c50(gb);
    }
    SwitchBank(gb, 0x03);
}

void LiftableRockStartSmashingAnimation_trampoline(GBState *gb, void (*start_smashing)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    if (start_smashing) {
        start_smashing(gb);
    }
    ReloadSavedBank(gb);
}

void label_394D(GBState *gb, void (*func_014_54ac)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x14);
    if (func_014_54ac) {
        func_014_54ac(gb);
    }
    ReloadSavedBank(gb);
}

void CreateFollowingNpcEntity_trampoline(GBState *gb, void (*create_following_npc)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, 0x01);
    if (create_following_npc) {
        create_following_npc(gb);
    }
    SwitchBank(gb, 0x02);
}

void ConfigureNewEntity_trampoline(GBState *gb, void (*configure_new_entity)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    if (configure_new_entity) {
        configure_new_entity(gb);
    }
    ReloadSavedBank(gb);
}

void GetEntityDirectionToLink_trampoline(GBState *gb, void (*get_direction)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    if (get_direction) {
        get_direction(gb);
    }
    ReloadSavedBank(gb);
}

void label_397B(GBState *gb, void (*func_014_5347)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x14);
    if (func_014_5347) {
        func_014_5347(gb);
    }
    gb_write(gb, rSelectROMBank, 0x03);
}

static const uint8_t s_default_hitbox_positions[64] = {
    0x08, 0x05, 0x08, 0x05,
    0x08, 0x0A, 0x08, 0x0A,
    0x08, 0x0A, 0x08, 0x0A,
    0x08, 0x10, 0x04, 0x0A,
    0x08, 0x02, 0x08, 0x02,
    0x08, 0x13, 0x08, 0x13,
    0x08, 0x06, 0x06, 0x08,
    0x08, 0x07, 0x06, 0x0A,
    0x08, 0x06, 0x10, 0x30,
    0x08, 0x07, 0x04, 0x0A,
    0x0C, 0x07, 0xFC, 0x04,
    0x10, 0x10, 0x0C, 0x12,
    0x08, 0x08, 0x02, 0x08,
    0x10, 0x0C, 0x08, 0x10,
    0x08, 0x07, 0x0C, 0x08,
    0x08, 0x08, 0x02, 0x08,
};

void ConfigureEntityHitbox(GBState *gb, uint16_t entity_index) {
    if (!gb) return;

    uint8_t flags = gb_read(gb, (uint16_t)(wEntitiesHitboxFlagsTable + entity_index));
    uint8_t offset = flags & 0x7C;
    uint16_t dest = (uint16_t)(wEntitiesHitboxPositionTable + (entity_index * 4));

    for (uint8_t i = 0; i < 4; i++) {
        uint8_t val;
        if (gb->rom && ((uint32_t)(HitboxPositions + offset + i) < gb->rom_size)) {
            val = gb_read(gb, (uint16_t)(HitboxPositions + offset + i));
        } else {
            val = s_default_hitbox_positions[(offset + i) < 64 ? (offset + i) : 0];
        }
        gb_write(gb, (uint16_t)(dest + i), val);
    }
}

void SetEntitySpriteVariant(GBState *gb, uint16_t entity_index, uint8_t variant) {
    if (!gb) return;
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + entity_index), variant);
}

void IncrementEntityState(GBState *gb, uint16_t entity_index) {
    if (!gb) return;
    uint16_t addr = (uint16_t)(wEntitiesStateTable + entity_index);
    uint8_t val = gb_read(gb, addr);
    gb_write(gb, addr, (uint8_t)(val + 1));
}

void HurtBySpikes_trampoline(GBState *gb, void (*hurt_by_spikes)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x02);
    if (hurt_by_spikes) {
        hurt_by_spikes(gb);
    }
    ReloadSavedBank(gb);
}

void ApplyEntityInteractionWithBackground_trampoline(GBState *gb, void (*apply_interaction)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (apply_interaction) {
        apply_interaction(gb);
    }
    ReloadSavedBank(gb);
}

void label_3B2E(GBState *gb, void (*apply_sword_intersection)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (apply_sword_intersection) {
        apply_sword_intersection(gb);
    }
    ReloadSavedBank(gb);
}

void DefaultEnemyDamageCollisionHandler_trampoline(GBState *gb, void (*default_handler)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (default_handler) {
        default_handler(gb);
    }
    ReloadSavedBank(gb);
}

void label_3B44(GBState *gb, void (*func_003_6c6b)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (func_003_6c6b) {
        func_003_6c6b(gb);
    }
    ReloadSavedBank(gb);
}

void CheckLinkCollisionWithProjectile_trampoline(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (check_collision) {
        check_collision(gb);
    }
    ReloadSavedBank(gb);
}

void CheckLinkCollisionWithEnemy_trampoline(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (check_collision) {
        check_collision(gb);
    }
    ReloadSavedBank(gb);
}

void label_3B65(GBState *gb, void (*func_003_73eb)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (func_003_73eb) {
        func_003_73eb(gb);
    }
    ReloadSavedBank(gb);
}

void label_3B70(GBState *gb, void (*func_003_6e2b)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (func_003_6e2b) {
        func_003_6e2b(gb);
    }
    ReloadSavedBank(gb);
}

void label_3B7B(GBState *gb, void (*func_003_75a2)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (func_003_75a2) {
        func_003_75a2(gb);
    }
    ReloadSavedBank(gb);
}

void ApplyVectorTowardsLink_trampoline(GBState *gb, void (*apply_vector)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (apply_vector) {
        apply_vector(gb);
    }
    ReloadSavedBank(gb);
}

void GetVectorTowardsLink_trampoline(GBState *gb, void (*get_vector)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (get_vector) {
        get_vector(gb);
    }
    ReloadSavedBank(gb);
}
