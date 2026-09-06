#include "home/check_items_to_use.h"
#include "home/bank.h"
#include "home/audio.h"
#include "home/gameplay.h"
#include "constants/inventory.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"
#include "constants/sfx.h"

/* Direction-indexed projectile offsets and speeds (00:139D - 00:13B4) */
static const uint8_t PlayerProjectileOffsetXPerDirection[4] = { 0, 0, 0, 0 };
static const uint8_t PlayerProjectileOffsetYPerDirection[4] = { 0, 0, 0, 0 };
static const uint8_t PlayerProjectileSpeedXPerDirection[4]   = { 32, (uint8_t)-32, 0, 0 };
static const uint8_t PlayerProjectileSpeedYPerDirection[4]   = { 0, 0, (uint8_t)-32, 32 };

/* Arrow speeds with and without Piece of Power (00:13AD, 00:13B5) */
static const uint8_t data_13AD[8] = {
    0x30, 0xD0, 0x00, 0x00,
    0x40, 0xC0, 0x00, 0x00
};

static const uint8_t data_13B5[8] = {
    0x00, 0x00, 0xD0, 0x30,
    0x00, 0x00, 0xC0, 0x40
};

/* Pegasus boots jump boosts (00:14C3, 00:14C7) */
static const uint8_t PegasusBootsJumpBoostXTable[4] = { 28, (uint8_t)-28, 0, 0 };
static const uint8_t PegasusBootsJumpBoostYTable[4] = { 0, 0, (uint8_t)-28, 28 };

/* Random sword swing sounds (00:1524) */
static const uint8_t SwordRandomSfxTable[4] = {
    NOISE_SFX_SWORD_SWING_A,
    NOISE_SFX_SWORD_SWING_B,
    NOISE_SFX_SWORD_SWING_C,
    NOISE_SFX_SWORD_SWING_D
};

/* Joypad direction conversion table (Bank 2 0x4905) */
static const uint8_t JoypadToLinkDirection[16] = {
    DIRECTION_KEEP,  /* 0: none */
    DIRECTION_RIGHT, /* 1: J_RIGHT */
    DIRECTION_LEFT,  /* 2: J_LEFT */
    DIRECTION_KEEP,  /* 3: J_RIGHT | J_LEFT */
    DIRECTION_UP,    /* 4: J_UP */
    DIRECTION_KEEP,  /* 5: J_UP | J_RIGHT */
    DIRECTION_KEEP,  /* 6: J_UP | J_LEFT */
    DIRECTION_KEEP,  /* 7 */
    DIRECTION_DOWN,  /* 8: J_DOWN */
    DIRECTION_KEEP,  /* 9: J_DOWN | J_RIGHT */
    DIRECTION_KEEP,  /* 10: J_DOWN | J_LEFT */
    DIRECTION_KEEP,  /* 11 */
    DIRECTION_KEEP,  /* 12 */
    DIRECTION_KEEP,  /* 13 */
    DIRECTION_KEEP,  /* 14 */
    DIRECTION_KEEP   /* 15 */
};

static uint8_t bcd_decrement(uint8_t val) {
    uint8_t lo = val & 0x0F;
    uint8_t hi = (val >> 4) & 0x0F;
    if (lo == 0) {
        lo = 9;
        if (hi > 0) hi--;
    } else {
        lo--;
    }
    return (uint8_t)((hi << 4) | lo);
}

void SetShieldVals(GBState *gb, void (*sync_shield_trampoline)(GBState *)) {
    if (!gb) return;
    gb_write(gb, wIsUsingShield, 0x01);
    gb_write(gb, wHasMirrorShield, gb_read(gb, wShieldLevel));
    if (sync_shield_trampoline) {
        sync_shield_trampoline(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}

void HoldSwordIfNeeded(GBState *gb, uint8_t item) {
    if (!gb) return;
    if (item != INVENTORY_SWORD) return;

    uint8_t ctx = gb_read(gb, wItemUsageContext) & (ITEM_USAGE_NEAR_NPC | ITEM_USAGE_READING_TEXT);
    if ((ctx | gb_read(gb, wSwordAnimationState)) != 0) return;
    if (gb_read(gb, wC160) != 0) return;

    gb_write(gb, wC1AC, 0x00);
    gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
    gb_write(gb, wSwordCollisionEnabled, SWORD_ANIMATION_STATE_HOLDING);
}

void UseShield(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wIsLinkPushing) != 0) return;
    gb_write(gb, hNoiseSfx, NOISE_SFX_DRAW_SHIELD);
}

void UseShovel(GBState *gb, bool (*check_poking)(GBState *)) {
    if (!gb) return;
    if ((gb_read(gb, wLinkUsingShovel) | gb_read(gb, wIsLinkInTheAir)) != 0) return;

    bool poking = false;
    if (check_poking) {
        poking = check_poking(gb);
    }
    if (poking) {
        gb_write(gb, hJingle, JINGLE_SWORD_POKING);
    } else {
        gb_write(gb, hNoiseSfx, NOISE_SFX_SHOVEL_DIG);
    }

    gb_write(gb, wLinkUsingShovel, 0x01);
    gb_write(gb, wLinkUsingShovelTimer, 0x00);
}

void UseHookshot(GBState *gb, void (*fire_hookshot)(GBState *)) {
    if (!gb) return;
    if (gb_read(gb, wIsUsingHookshot) != 0) return;
    if (fire_hookshot) {
        fire_hookshot(gb);
    }
}

void UseMagicRod(GBState *gb) {
    if (!gb) return;
    if ((gb_read(gb, wSwordAnimationState) | gb_read(gb, wLinkAttackStepAnimationCountdown)) != 0) {
        return;
    }
    if (gb_read(gb, wActiveProjectileCount) >= 2) {
        return;
    }
    gb_write(gb, wLinkAttackStepAnimationCountdown, 0x0E | ATTACK_STEP_ITEM_MAGIC_ROD);
}

bool SpawnPlayerProjectile(GBState *gb, uint8_t entity_type, uint8_t *out_entity_index) {
    if (!gb) return false;

    /* Find empty entity slot */
    int slot = -1;
    for (int i = 0; i < 16; i++) {
        if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) == 0) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        return false; /* carry set */
    }

    uint8_t de = (uint8_t)slot;
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), 4); /* ENTITY_STATUS_INIT */
    gb_write(gb, (uint16_t)(wEntitiesTypeTable + de), entity_type);

    gb_write(gb, wLinkAttackStepAnimationCountdown, 0x0C);

    uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;

    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), (uint8_t)(gb_read(gb, hLinkPositionX) + PlayerProjectileOffsetXPerDirection[dir]));
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), (uint8_t)(gb_read(gb, hLinkPositionY) + PlayerProjectileOffsetYPerDirection[dir]));
    gb_write(gb, (uint16_t)(wEntitiesPosZTable + de), (uint8_t)(gb_read(gb, hLinkPositionZ) + 1));
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + de), PlayerProjectileSpeedXPerDirection[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + de), PlayerProjectileSpeedYPerDirection[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), dir);
    gb_write(gb, (uint16_t)(wEntitiesDirectionTable + de), dir);
    gb_write(gb, (uint16_t)(wEntitiesThrownDirectionTable + de), dir);
    gb_write(gb, (uint16_t)(wEntitiesUnknowTableJ + de), 0x01);

    if (out_entity_index) {
        *out_entity_index = de;
    }
    return true; /* carry clear */
}

void PlaceBomb(GBState *gb, bool (*spawn_bomb_projectile)(GBState *), void (*convert_to_bomb_arrow)(GBState *)) {
    if (!gb) return;
    if (gb_read(gb, wHasPlacedBomb) >= 1) return;

    uint8_t count = gb_read(gb, wBombCount);
    if (count == 0) {
        PlayWrongAnswerJingle(gb);
        return;
    }

    gb_write(gb, wBombCount, bcd_decrement(count));

    bool spawned = true;
    if (spawn_bomb_projectile) {
        spawned = spawn_bomb_projectile(gb);
    } else {
        uint8_t bomb_idx = 0;
        spawned = SpawnPlayerProjectile(gb, ENTITY_BOMB, &bomb_idx);
        if (spawned) {
            gb_write(gb, wLatestDroppedBombEntityIndex, bomb_idx);
        }
    }
    if (!spawned) return;

    if (convert_to_bomb_arrow) {
        convert_to_bomb_arrow(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}

void UsePowerBracelet(GBState *gb) {
    (void)gb;
}

void UseBoomerang(GBState *gb, bool (*spawn_projectile)(GBState *, uint8_t, uint8_t *), void (*func_020_4bff)(GBState *)) {
    if (!gb) return;
    if (gb_read(gb, wActiveProjectileCount) != 0) return;

    bool spawned;
    if (spawn_projectile) {
        spawned = spawn_projectile(gb, ENTITY_BOOMERANG, NULL);
    } else {
        spawned = SpawnPlayerProjectile(gb, ENTITY_BOOMERANG, NULL);
    }
    if (!spawned) return;

    if (func_020_4bff) {
        func_020_4bff(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}

void UpdateLinkDirectionFromJoypad(GBState *gb) {
    if (!gb) return;
    uint8_t dpad = gb_read(gb, hPressedButtonsMask) & (J_RIGHT | J_LEFT | J_UP | J_DOWN);
    uint8_t dir = JoypadToLinkDirection[dpad & 0x0F];
    if (dir != DIRECTION_KEEP) {
        gb_write(gb, hLinkDirection, dir);
    }
}

bool ShootArrow(GBState *gb, bool (*spawn_projectile)(GBState *, uint8_t, uint8_t *), void (*func_157c_cb)(GBState *)) {
    if (!gb) return false;

    if (gb_read(gb, wIsShootingArrow) != 0) return false;
    if (gb_read(gb, wActiveProjectileCount) >= 2) return false;

    gb_write(gb, wIsShootingArrow, 0x10);

    uint8_t count = gb_read(gb, wArrowCount);
    if (count == 0) {
        PlayWrongAnswerJingle(gb);
        return false;
    }

    gb_write(gb, wArrowCount, bcd_decrement(count));

    if (func_157c_cb) {
        func_157c_cb(gb);
    } else {
        UpdateLinkDirectionFromJoypad(gb);
    }

    uint8_t entity_idx = 0;
    bool spawned;
    if (spawn_projectile) {
        spawned = spawn_projectile(gb, ENTITY_ARROW, &entity_idx);
    } else {
        spawned = SpawnPlayerProjectile(gb, ENTITY_ARROW, &entity_idx);
    }
    if (!spawned) return false;

    gb_write(gb, wLatestShotArrowEntityIndex, entity_idx);

    if (gb_read(gb, wBombArrowCooldown) != 0) {
        uint8_t bomb_idx = gb_read(gb, wLatestDroppedBombEntityIndex);
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + bomb_idx), 0x00);
        gb_write(gb, (uint16_t)(wEntitiesStateTable + entity_idx), 0x01);
        gb_write(gb, wBombArrowCooldown, 0x00);
    } else {
        gb_write(gb, hNoiseSfx, NOISE_SFX_WHOOSH);
        gb_write(gb, wBombArrowCooldown, 0x06);
    }

    uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
    uint8_t offset = dir;
    if (gb_read(gb, wActivePowerUp) == 0x01) {
        offset += 4;
    }

    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + entity_idx), data_13AD[offset]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + entity_idx), data_13B5[offset]);

    return true;
}

void UseMagicPowder(GBState *gb, uint16_t (*spawn_entity)(GBState *, uint8_t), void (*sprinkle_powder)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wLinkAttackStepAnimationCountdown) != 0) return;

    if (gb_read(gb, wHasToadstool) != 0) {
        if (gb_read(gb, hLinkPositionZ) != 0) return;
        gb_write(gb, wDialogGotItem, 0x02); /* DIALOG_GOT_TOADSTOOL */
        gb_write(gb, wDialogGotItemCountdown, 0x2A);
        return;
    }

    if (gb_read(gb, wMagicPowderCount) == 0) {
        PlayWrongAnswerJingle(gb);
        return;
    }

    bool spawned;
    if (spawn_entity) {
        spawned = (spawn_entity(gb, ENTITY_MAGIC_POWDER_SPRINKLE) != 0);
    } else {
        spawned = SpawnPlayerProjectile(gb, ENTITY_MAGIC_POWDER_SPRINKLE, NULL);
    }
    if (!spawned) return;

    if (sprinkle_powder) {
        sprinkle_powder(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}

void UseRocsFeather(GBState *gb, void (*update_final_link_pos)(GBState *), void (*check_pos_for_map_trans)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wC130) == 0x07) return;
    if (gb_read(gb, wIsLinkInTheAir) != 0) return;

    gb_write(gb, wIsLinkInTheAir, 0x01);
    gb_write(gb, wC152, 0x00);
    gb_write(gb, wC153, 0x00);
    gb_write(gb, hJingle, JINGLE_FEATHER_JUMP);

    if (gb_read(gb, hIsSideScrolling) != 0) {
        gb_write(gb, hLinkVelocityZ, 0x20);
        if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
            uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
            gb_write(gb, hLinkSpeedX, PegasusBootsJumpBoostXTable[dir]);
            gb_write(gb, hLinkSpeedY, PegasusBootsJumpBoostYTable[dir]);
        }

        uint8_t pressed = gb_read(gb, hPressedButtonsMask);
        if ((pressed & (J_LEFT | J_RIGHT)) != 0) {
            gb_write(gb, hLinkSpeedY, 0xE8);
        } else {
            gb_write(gb, hLinkSpeedY, 0xEA);
        }

        gb_write(gb, hLinkVelocityZ, 0x00);

        if (update_final_link_pos) {
            update_final_link_pos(gb);
        }
        if (check_pos_for_map_trans) {
            check_pos_for_map_trans(gb);
        }
    } else {
        gb_write(gb, hLinkVelocityZ, 0x20);
        if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
            uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
            gb_write(gb, hLinkSpeedX, PegasusBootsJumpBoostXTable[dir]);
            gb_write(gb, hLinkSpeedY, PegasusBootsJumpBoostYTable[dir]);
        }
    }
}

void UseSword(GBState *gb, void (*reset_spin_attack)(GBState *), void (*clear_link_position_increment)(GBState *)) {
    if (!gb) return;

    if ((gb_read(gb, wC16D) | gb_read(gb, wIsUsingSpinAttack)) != 0) return;

    gb_write(gb, wC138, 0x03);
    gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_SWING_START);
    gb_write(gb, wSwordCollisionEnabled, SWORD_ANIMATION_STATE_SWING_START);
    gb_write(gb, wC160, 0x00);
    gb_write(gb, wC1AC, 0x00);

    uint8_t sfx_idx = GetRandomByte(gb) & 0x03;
    gb_write(gb, hNoiseSfx, SwordRandomSfxTable[sfx_idx]);

    UpdateLinkDirectionFromJoypad(gb);

    if (gb_read(gb, wIsLinkInTheAir) == 0) {
        if (reset_spin_attack) {
            reset_spin_attack(gb);
        }
        if (clear_link_position_increment) {
            clear_link_position_increment(gb);
        }
    }

    if (gb_read(gb, wActiveProjectileCount) != 0) return;
    if (gb_read(gb, wFullHearts) == 0) return;
    if (gb_read(gb, wSwordLevel) != 0x02) return;

    SpawnPlayerProjectile(gb, ENTITY_SWORD_BEAM, NULL);
    gb_write(gb, wLinkAttackStepAnimationCountdown, 0x00);
}

void UseItem(GBState *gb, uint8_t item, const ItemUsageCallbacks *callbacks) {
    if (!gb) return;
    switch (item) {
        case INVENTORY_SWORD:
            if (callbacks && callbacks->use_sword) callbacks->use_sword(gb);
            else UseSword(gb, NULL, NULL);
            break;
        case INVENTORY_SHIELD:
            UseShield(gb);
            break;
        case INVENTORY_BOMBS:
            PlaceBomb(gb, callbacks ? callbacks->spawn_bomb_projectile : NULL,
                          callbacks ? callbacks->convert_to_bomb_arrow : NULL);
            break;
        case INVENTORY_POWER_BRACELET:
            if (callbacks && callbacks->use_power_bracelet) callbacks->use_power_bracelet(gb);
            else UsePowerBracelet(gb);
            break;
        case INVENTORY_BOW:
            if (callbacks && callbacks->shoot_arrow) callbacks->shoot_arrow(gb);
            else ShootArrow(gb, NULL, NULL);
            break;
        case INVENTORY_BOOMERANG:
            if (callbacks && callbacks->use_boomerang) callbacks->use_boomerang(gb);
            else UseBoomerang(gb, NULL, NULL);
            break;
        case INVENTORY_HOOKSHOT:
            UseHookshot(gb, callbacks ? callbacks->fire_hookshot : NULL);
            break;
        case INVENTORY_ROCS_FEATHER:
            if (callbacks && callbacks->use_rocs_feather) callbacks->use_rocs_feather(gb);
            else UseRocsFeather(gb, NULL, NULL);
            break;
        case INVENTORY_OCARINA:
            if (callbacks && callbacks->use_ocarina) callbacks->use_ocarina(gb);
            break;
        case INVENTORY_MAGIC_POWDER:
            if (callbacks && callbacks->use_magic_powder) callbacks->use_magic_powder(gb);
            else UseMagicPowder(gb, NULL, NULL);
            break;
        case INVENTORY_SHOVEL:
            UseShovel(gb, callbacks ? callbacks->check_shovel_poking : NULL);
            break;
        case INVENTORY_MAGIC_ROD:
            UseMagicRod(gb);
            break;
        default:
            break;
    }
}

void CheckItemsToUse(GBState *gb, void (*use_pegasus_boots)(GBState *),
                     void (*use_item)(GBState *, uint8_t),
                     void (*color_dungeon_check)(GBState *)) {
    if (!gb) return;

    if ((gb_read(gb, wBlockItemUsage) | gb_read(gb, wC167) | gb_read(gb, wIsUsingHookshot)) != 0) {
        return;
    }

    /* Configure sword and shield */
    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
        uint8_t a_slot = gb_read(gb, wInventoryItems_AButtonSlot);
        uint8_t b_slot = gb_read(gb, wInventoryItems_BButtonSlot);
        if (a_slot == INVENTORY_SWORD || b_slot == INVENTORY_SWORD) {
            uint8_t state = gb_read(gb, wSwordAnimationState);
            if ((uint8_t)(state - 1) >= SWORD_ANIMATION_STATE_SWING_END) {
                gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_HOLDING);
                gb_write(gb, wC16A, SWORD_ANIMATION_STATE_HOLDING);
            }
        } else if (a_slot == INVENTORY_SHIELD || b_slot == INVENTORY_SHIELD) {
            SetShieldVals(gb, NULL);
        }
    } else {
        gb_write(gb, wIsUsingShield, 0);
        gb_write(gb, wHasMirrorShield, 0);
    }

    /* Check gel / lifted object */
    if (gb_read(gb, wIsGelClingingToLink) != 0) return;
    if (gb_read(gb, wIsCarryingLiftedObject) != 0) return;

    /* Check sword animation / motion blocked */
    uint8_t sword_anim = gb_read(gb, wSwordAnimationState);
    if (sword_anim == 0 || sword_anim != SWORD_ANIMATION_STATE_SWING_MIDDLE || gb_read(gb, wC138) < 3) {
        if (gb_read(gb, hLinkInteractiveMotionBlocked) != 0) {
            return;
        }
    }

    uint8_t pressed = gb_read(gb, hPressedButtonsMask);
    uint8_t b_slot = gb_read(gb, wInventoryItems_BButtonSlot);
    uint8_t a_slot = gb_read(gb, wInventoryItems_AButtonSlot);

    /* Pegasus Boots B */
    if (b_slot == INVENTORY_PEGASUS_BOOTS) {
        if (pressed & J_B) {
            if (use_pegasus_boots) use_pegasus_boots(gb);
        } else {
            gb_write(gb, wPegasusBootsChargeMeter, 0);
        }
    }

    /* Pegasus Boots A */
    if (a_slot == INVENTORY_PEGASUS_BOOTS) {
        if (pressed & J_A) {
            if (use_pegasus_boots) use_pegasus_boots(gb);
        } else {
            gb_write(gb, wPegasusBootsChargeMeter, 0);
        }
    }

    /* Shield A */
    if (a_slot == INVENTORY_SHIELD) {
        gb_write(gb, wHasMirrorShield, gb_read(gb, wShieldLevel));
        if (pressed & J_A) {
            uint8_t ctx = gb_read(gb, wItemUsageContext);
            if (ctx != ITEM_USAGE_NEAR_NPC && ctx != ITEM_USAGE_READING_TEXT) {
                SetShieldVals(gb, NULL);
            }
        }
    }

    /* Shield B */
    if (b_slot == INVENTORY_SHIELD) {
        gb_write(gb, wHasMirrorShield, gb_read(gb, wShieldLevel));
        if (pressed & J_B) {
            SetShieldVals(gb, NULL);
        }
    }

    uint8_t joypad = gb_read(gb, hJoypadState);

    /* Next item B */
    if (joypad & J_B) {
        if (gb_read(gb, wItemUsageContext) != ITEM_USAGE_READING_TEXT) {
            if (use_item) {
                use_item(gb, b_slot);
            }
        }
    }

    /* Next item A */
    if (joypad & J_A) {
        uint8_t ctx = gb_read(gb, wItemUsageContext);
        if (ctx != ITEM_USAGE_NEAR_NPC && ctx != ITEM_USAGE_READING_TEXT) {
            if (use_item) {
                use_item(gb, a_slot);
            }
        }
    }

    /* Sword B */
    if (pressed & J_B) {
        HoldSwordIfNeeded(gb, b_slot);
    }

    /* Sword A */
    if (pressed & J_A) {
        HoldSwordIfNeeded(gb, a_slot);
    }

    /* Color Dungeon check */
    if (color_dungeon_check) {
        color_dungeon_check(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}
