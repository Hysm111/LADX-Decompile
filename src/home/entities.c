#include "home/entities.h"
#include "home/bank.h"
#include "constants/memory.h"
#include "constants/hardware.h"
#include "constants/entities.h"
#include "constants/sfx.h"
#include "constants/maps.h"
#include "constants/gameplay.h"

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

void AnimateEntities(GBState *gb, const AnimateEntitiesCallbacks *callbacks) {
    if (!gb) return;

    /* Play the Boss Agony audio effect if needed */
    uint8_t agony = gb_read(gb, wBossAgonySFXCountdown);
    if (agony != 0) {
        agony--;
        gb_write(gb, wBossAgonySFXCountdown, agony);
        if (agony == 0) {
            gb_write(gb, hWaveSfx, WAVE_SFX_BOSS_DEATH_CRY);
        }
    }

    /* If no dialog is open, decrement wC111 */
    if (gb_read(gb, wDialogState) == 0) {
        uint8_t c111 = gb_read(gb, wC111);
        gb_write(gb, wC1A8, c111);
        if (c111 != 0) {
            gb_write(gb, wC111, (uint8_t)(c111 - 1));
        }
    }

    /* If Link is passing out, return */
    if (gb_read(gb, wLinkMotionState) == LINK_MOTION_PASS_OUT) {
        return;
    }

    gb_write(gb, wC3C1, 0);

    uint8_t map_id = gb_read(gb, hMapId);
    uint8_t slot_idx = 0;
    if (map_id < MAP_CAVE_B) {
        slot_idx = gb_read(gb, hFrameCounter) & 0x03;
    }
    static const uint8_t s_data_3989[4] = {0, 8, 0x10, 0x18};
    gb_write(gb, wOAMNextAvailableSlot, s_data_3989[slot_idx]);

    gb_write(gb, rSelectROMBank, 0x20);
    if (callbacks && callbacks->func_020_4303) {
        callbacks->func_020_4303(gb);
    }
    gb_write(gb, rSelectROMBank, 0x00);

    if (gb_read(gb, wDialogState) == 0) {
        gb_write(gb, wItemUsageContext, 0);
    }

    SwitchBank(gb, 0x20);
    if (callbacks && callbacks->func_020_6352) {
        callbacks->func_020_6352(gb);
    }

    /* For each entity slot (MAX_ENTITIES - 1 down to 0) */
    for (int c = MAX_ENTITIES - 1; c >= 0; c--) {
        gb_write(gb, wActiveEntityIndex, (uint8_t)c);
        uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + c));
        if (status != 0) {
            gb_write(gb, hActiveEntityStatus, status);
            if (callbacks && callbacks->AnimateEntity) {
                callbacks->AnimateEntity(gb, (uint16_t)c);
            }
        }
    }
}

void ResetEntity_trampoline(GBState *gb, void (*reset_entity)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x15);
    if (reset_entity) {
        reset_entity(gb);
    }
    gb_write(gb, rSelectROMBank, 0x03);
}

void AnimateEntity(GBState *gb, uint16_t entity_index, const AnimateEntityCallbacks *callbacks) {
    if (!gb) return;

    uint8_t type = gb_read(gb, (uint16_t)(wEntitiesTypeTable + entity_index));
    gb_write(gb, hActiveEntityType, type);

    uint8_t state = gb_read(gb, (uint16_t)(wEntitiesStateTable + entity_index));
    gb_write(gb, hActiveEntityState, state);

    uint8_t variant = gb_read(gb, (uint16_t)(wEntitiesSpriteVariantTable + entity_index));
    gb_write(gb, hActiveEntitySpriteVariant, variant);

    SwitchBank(gb, 0x19);

    bool is_lifted = false;
    if (type == ENTITY_RAFT_RAFT_OWNER) {
        if (gb_read(gb, hLinkSlowWalkingSpeed) != 0) {
            is_lifted = true;
        }
    }
    if (gb_read(gb, hActiveEntityStatus) == ENTITY_STATUS_LIFTED) {
        is_lifted = true;
    }

    if (is_lifted) {
        if (callbacks && callbacks->UpdateEntityPositionForRoomTransition) {
            callbacks->UpdateEntityPositionForRoomTransition(gb);
        }
        if (callbacks && callbacks->CopyEntityPositionToActivePosition) {
            callbacks->CopyEntityPositionToActivePosition(gb, entity_index);
        } else {
            CopyEntityPositionToActivePosition(gb, entity_index);
        }
    } else {
        if (callbacks && callbacks->CopyEntityPositionToActivePosition) {
            callbacks->CopyEntityPositionToActivePosition(gb, entity_index);
        } else {
            CopyEntityPositionToActivePosition(gb, entity_index);
        }
        if (callbacks && callbacks->UpdateEntityPositionForRoomTransition) {
            callbacks->UpdateEntityPositionForRoomTransition(gb);
        }
    }

    SwitchBank(gb, 0x14);
    if (callbacks && callbacks->UpdateEntityTimers) {
        callbacks->UpdateEntityTimers(gb);
    }

    SwitchBank(gb, 0x03);

    uint8_t status = gb_read(gb, hActiveEntityStatus);
    if (callbacks) {
        switch (status) {
            case ENTITY_STATUS_DISABLED:
                break;
            case ENTITY_STATUS_DYING:
                if (callbacks->EntityDeathHandler) callbacks->EntityDeathHandler(gb);
                break;
            case ENTITY_STATUS_FALLING:
                if (callbacks->EntityFallHandler) callbacks->EntityFallHandler(gb);
                break;
            case ENTITY_STATUS_BURNING:
                if (callbacks->EntityBurningHandler) callbacks->EntityBurningHandler(gb);
                break;
            case ENTITY_STATUS_INIT:
                if (callbacks->EntityInitHandler) callbacks->EntityInitHandler(gb);
                break;
            case ENTITY_STATUS_ACTIVE:
                if (callbacks->ExecuteActiveEntityHandler) callbacks->ExecuteActiveEntityHandler(gb);
                break;
            case ENTITY_STATUS_STUNNED:
                if (callbacks->EntityStunnedHandler) callbacks->EntityStunnedHandler(gb);
                break;
            case ENTITY_STATUS_LIFTED:
                if (callbacks->EntityLiftedHandler) callbacks->EntityLiftedHandler(gb);
                break;
            case ENTITY_STATUS_THROWN:
                if (callbacks->EntityThrownHandler) callbacks->EntityThrownHandler(gb);
                break;
            default:
                break;
        }
    }
}

void ExecuteActiveEntityHandler_trampoline(GBState *gb, void (*execute_active_handler)(GBState *)) {
    if (!gb) return;
    if (execute_active_handler) {
        execute_active_handler(gb);
    }
    SwitchBank(gb, 0x03);
}

void ExecuteActiveEntityHandler(GBState *gb, void (*handler_dispatch)(GBState *, uint8_t bank, uint16_t addr)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);

    uint8_t type = gb_read(gb, hActiveEntityType);
    uint16_t entry_addr = (uint16_t)(EntityHandlersTable + (type * 3));

    uint8_t low = gb_read(gb, entry_addr);
    uint8_t high = gb_read(gb, (uint16_t)(entry_addr + 1));
    uint8_t bank = gb_read(gb, (uint16_t)(entry_addr + 2));

    uint16_t handler_addr = (uint16_t)((high << 8) | low);
    SwitchBank(gb, bank);

    if (handler_dispatch) {
        handler_dispatch(gb, bank, handler_addr);
    }
}

void ClearEntitySpeed(GBState *gb, uint16_t entity_index) {
    if (!gb) return;
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + entity_index), 0);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + entity_index), 0);
}

void CopyEntityPositionToActivePosition(GBState *gb, uint16_t entity_index) {
    if (!gb) return;
    uint8_t posX = gb_read(gb, (uint16_t)(wEntitiesPosXTable + entity_index));
    gb_write(gb, hActiveEntityPosX, posX);

    uint8_t posY = gb_read(gb, (uint16_t)(wEntitiesPosYTable + entity_index));
    gb_write(gb, hActiveEntityPosY, posY);

    uint8_t posZ = gb_read(gb, (uint16_t)(wEntitiesPosZTable + entity_index));
    gb_write(gb, hActiveEntityVisualPosY, (uint8_t)(posY - posZ));
}

bool SkipDisabledEntityDuringRoomTransition(GBState *gb, uint16_t entity_index) {
    if (!gb) return false;
    if (gb_read(gb, wRoomTransitionState) == 0) {
        return false;
    }
    uint8_t posX = gb_read(gb, hActiveEntityPosX);
    if ((uint8_t)(posX - 1) >= 0xC0) {
        return true;
    }
    uint8_t posY = gb_read(gb, hActiveEntityVisualPosY);
    if ((uint8_t)(posY - 1) >= 0x88) {
        return true;
    }
    if (gb_read(gb, (uint16_t)(wEntitiesPosXSignTable + entity_index)) != 0) {
        return true;
    }
    if (gb_read(gb, (uint16_t)(wEntitiesPosYSignTable + entity_index)) != 0) {
        return true;
    }
    return false;
}

void label_3C71(GBState *gb, uint16_t entity_index, void (*func_015_7995)(GBState *, uint16_t)) {
    if (!gb) return;
    if (func_015_7995) {
        func_015_7995(gb, entity_index);
    }
    ReloadSavedBank(gb);
}

void label_3CD9(GBState *gb, uint16_t entity_index, void (*func_015_7995)(GBState *, uint16_t)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x15);
    label_3C71(gb, entity_index, func_015_7995);
}

void RenderActiveEntitySpritesPair(GBState *gb, const uint8_t *display_list, const EntityRenderCallbacks *callbacks) {
    if (!gb || !display_list) return;

    uint8_t variant = gb_read(gb, hActiveEntitySpriteVariant);
    if (variant == 0xFF) return;

    uint16_t entity_index = gb_read(gb, wActiveEntityIndex);
    if (SkipDisabledEntityDuringRoomTransition(gb, entity_index)) {
        return;
    }

    uint16_t oam_dest = (uint16_t)(wDynamicOAMBuffer + gb_read(gb, wOAMNextAvailableSlot));
    uint8_t visualPosY = gb_read(gb, hActiveEntityVisualPosY);
    uint8_t posX = gb_read(gb, hActiveEntityPosX);
    uint8_t shake = gb_read(gb, wScreenShakeHorizontal);
    uint8_t flip = gb_read(gb, hActiveEntityFlipAttribute);

    /* Sprite 0 Y */
    gb_write(gb, oam_dest++, visualPosY);

    /* Sprite 0 X */
    uint8_t x_adj0 = (flip & OAMF_XFLIP) ? 8 : 0;
    uint8_t x0 = (uint8_t)(x_adj0 + posX - shake);
    gb_write(gb, oam_dest++, x0);

    /* Sprite 0 Tile */
    uint16_t list_idx = (uint16_t)(variant * 4);
    uint8_t tiles_offset = gb_read(gb, hActiveEntityTilesOffset);
    uint8_t tile0 = (uint8_t)(display_list[list_idx] + tiles_offset);
    gb_write(gb, oam_dest++, tile0);
    if ((tile0 & 0x0F) == 0x0F) {
        gb_write(gb, (uint16_t)(oam_dest - 3), 0xF0);
    }

    /* Sprite 0 Attr */
    uint8_t attr0 = (uint8_t)(display_list[list_idx + 1] ^ flip);
    if (gb_read(gb, hIsGBC) != 0 && (flip & OAMF_PAL1) != 0) {
        attr0 = (uint8_t)((attr0 & (uint8_t)(~OAMF_PALMASK)) | OAM_GBC_PAL_4);
    }
    gb_write(gb, oam_dest++, attr0);

    /* Sprite 1 Y */
    gb_write(gb, oam_dest++, visualPosY);

    /* Sprite 1 X */
    uint8_t x_adj1 = (flip & OAMF_XFLIP) ? 0 : 8;
    uint8_t x1 = (uint8_t)(posX + x_adj1 - shake);
    gb_write(gb, oam_dest++, x1);

    /* Sprite 1 Tile */
    uint8_t tile1 = (uint8_t)(display_list[list_idx + 2] + tiles_offset);
    gb_write(gb, oam_dest++, tile1);
    if ((tile1 & 0x0F) == 0x0F) {
        gb_write(gb, (uint16_t)(oam_dest - 3), 0xF0);
    }

    /* Sprite 1 Attr */
    uint8_t attr1 = (uint8_t)(display_list[list_idx + 3] ^ flip);
    if (gb_read(gb, hIsGBC) != 0 && (flip & OAMF_PAL1) != 0) {
        attr1 = (uint8_t)((attr1 & (uint8_t)(~OAMF_PALMASK)) | OAM_GBC_PAL_4);
    }
    gb_write(gb, oam_dest++, attr1);

    /* Bank $15 calls */
    gb_write(gb, rSelectROMBank, 0x15);
    if (callbacks && callbacks->func_015_795D) {
        callbacks->func_015_795D(gb, entity_index);
    }
    if (callbacks && callbacks->func_015_7995) {
        callbacks->func_015_7995(gb, entity_index);
    }
    ReloadSavedBank(gb);
}

void RenderActiveEntitySprite(GBState *gb, const uint8_t *display_list, const EntityRenderCallbacks *callbacks) {
    if (!gb || !display_list) return;

    uint8_t variant = gb_read(gb, hActiveEntitySpriteVariant);
    if (variant == 0xFF) return;

    uint16_t entity_index = gb_read(gb, wActiveEntityIndex);
    if (SkipDisabledEntityDuringRoomTransition(gb, entity_index)) {
        return;
    }

    uint16_t oam_dest = (uint16_t)(wDynamicOAMBuffer + gb_read(gb, wOAMNextAvailableSlot));

    uint8_t visualPosY = gb_read(gb, hActiveEntityVisualPosY);
    if (gb_read(gb, hIsSideScrolling) != 0) {
        visualPosY = (uint8_t)(visualPosY - 4);
        gb_write(gb, hActiveEntityVisualPosY, visualPosY);
    }
    gb_write(gb, oam_dest++, visualPosY);

    uint8_t shake = gb_read(gb, wScreenShakeHorizontal);
    uint8_t posX = gb_read(gb, hActiveEntityPosX);
    uint8_t x = (uint8_t)(posX + 4 - shake);
    gb_write(gb, oam_dest++, x);

    uint16_t list_idx = (uint16_t)(variant * 2);
    uint8_t tile = display_list[list_idx];
    gb_write(gb, oam_dest++, tile);

    uint8_t flip = gb_read(gb, hActiveEntityFlipAttribute);
    uint8_t attr;
    if (gb_read(gb, hIsGBC) != 0 &&
        gb_read(gb, wGameplayType) != GAMEPLAY_CREDITS &&
        flip != 0) {
        attr = (uint8_t)((display_list[list_idx + 1] & (uint8_t)(~OAMF_PALMASK)) | OAM_GBC_PAL_4);
    } else {
        attr = (uint8_t)(display_list[list_idx + 1] ^ flip);
    }
    gb_write(gb, oam_dest++, attr);

    gb_write(gb, rSelectROMBank, 0x15);
    if (callbacks && callbacks->func_015_795D) {
        callbacks->func_015_795D(gb, entity_index);
    }
    if (callbacks && callbacks->func_015_7995) {
        callbacks->func_015_7995(gb, entity_index);
    }
    ReloadSavedBank(gb);
}

static void RenderActiveEntitySpritesRectInternal(GBState *gb, const uint8_t *display_list, uint8_t sprite_count, uint16_t dest_base, void (*func_015_795D)(GBState *, uint16_t)) {
    if (!gb || !display_list) return;

    uint8_t variant = gb_read(gb, hActiveEntitySpriteVariant);
    if (variant == 0xFF) return;

    uint16_t entity_index = gb_read(gb, wActiveEntityIndex);
    if (SkipDisabledEntityDuringRoomTransition(gb, entity_index)) {
        return;
    }

    uint16_t de = dest_base;
    uint8_t visualPosY = gb_read(gb, hActiveEntityVisualPosY);
    uint8_t posX = gb_read(gb, hActiveEntityPosX);
    uint8_t shake = gb_read(gb, wScreenShakeHorizontal);
    uint8_t tiles_offset = gb_read(gb, hActiveEntityTilesOffset);
    uint8_t flip = gb_read(gb, hActiveEntityFlipAttribute);

    for (uint8_t i = 0; i < sprite_count; i++) {
        uint16_t idx = (uint16_t)(i * 4);
        /* Y */
        gb_write(gb, de++, (uint8_t)(visualPosY + display_list[idx]));
        /* X */
        gb_write(gb, de++, (uint8_t)(posX + display_list[idx + 1] - shake));
        /* Tile */
        uint8_t tile = display_list[idx + 2];
        gb_write(gb, de++, (uint8_t)(tile + tiles_offset));
        if (tile == 0xFF) {
            gb_write(gb, (uint16_t)(de - 1), 0);
        }
        /* Attr */
        uint8_t attr = (uint8_t)(display_list[idx + 3] ^ flip);
        if (gb_read(gb, hIsGBC) != 0 && flip != 0) {
            attr = (uint8_t)((attr & (uint8_t)(~OAMF_PALMASK)) | OAM_GBC_PAL_4);
        }
        gb_write(gb, de++, attr);
    }

    gb_write(gb, rSelectROMBank, 0x15);
    if (func_015_795D) {
        func_015_795D(gb, entity_index);
    }
    ReloadSavedBank(gb);
}

void RenderActiveEntitySpritesRect(GBState *gb, const uint8_t *display_list, uint8_t sprite_count, void (*func_015_795D)(GBState *, uint16_t)) {
    if (!gb) return;
    uint16_t dest = (uint16_t)(wDynamicOAMBuffer + gb_read(gb, wOAMNextAvailableSlot));
    RenderActiveEntitySpritesRectInternal(gb, display_list, sprite_count, dest, func_015_795D);
}

void RenderActiveEntitySpritesRectUsingAllOAM(GBState *gb, const uint8_t *display_list, uint8_t sprite_count, void (*func_015_795D)(GBState *, uint16_t)) {
    RenderActiveEntitySpritesRectInternal(gb, display_list, sprite_count, wOAMBuffer, func_015_795D);
}

void func_015_7964_trampoline(GBState *gb, void (*func_015_7964)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x15);
    if (func_015_7964) {
        func_015_7964(gb);
    }
    ReloadSavedBank(gb);
}

void EntityInitMiniMoldorm_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x04);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitMoldorm_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x04);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitFacade_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x04);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitSlimeEye_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x04);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitGenie_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x36);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitSlimeEel_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x05);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitDodongoSnake_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x05);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitHotHead_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x05);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void EntityInitEvilEagle_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x05);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void Entity67Handler_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x05);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void CheckPositionForMapTransition_trampoline(GBState *gb, void (*check_pos)(GBState *)) {
    if (!gb) return;
    uint8_t saved_bank = gb_read(gb, wCurrentBank);
    SwitchBank(gb, 0x02);
    if (check_pos) check_pos(gb);
    SwitchBank(gb, saved_bank);
}

void GhiniMovement_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x04);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void SmashRock_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (func) func(gb);
    ReloadSavedBank(gb);
}

void LoadHeartsAndRupeesCount(GBState *gb, void (*load_rupees)(GBState *), void (*load_hearts)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x02);
    if (load_rupees) load_rupees(gb);
    if (load_hearts) load_hearts(gb);
    ReloadSavedBank(gb);
}

void SpawnChestWithItemAndRestoreBank3(GBState *gb, void (*spawn_chest)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x02);
    if (spawn_chest) spawn_chest(gb);
    SwitchBank(gb, 0x03);
}

void DrawABButtonSlots(GBState *gb, void (*draw_slots)(GBState *, uint16_t bc, uint8_t e)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (draw_slots) draw_slots(gb, 0x0001, 0xFF);
    ReloadSavedBank(gb);
}

void GiveInventoryItem_trampoline(GBState *gb, uint8_t item, void (*give_item)(GBState *, uint8_t item)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x03);
    if (give_item) give_item(gb, item);
    ReloadSavedBank(gb);
}

void func_006_783C_trampoline(GBState *gb, void (*func)(GBState *)) {
    if (!gb) return;
    SwitchBank(gb, 0x06);
    if (func) func(gb);
    SwitchBank(gb, 0x03);
}

void UnloadAllEntities(GBState *gb) {
    if (!gb) return;
    for (uint16_t i = 0; i < MAX_ENTITIES; i++) {
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
    }
}
