#include "home/check_items_to_use.h"
#include "home/bank.h"
#include "home/audio.h"
#include "constants/inventory.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"
#include "constants/sfx.h"

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
    }
    if (!spawned) return;

    if (convert_to_bomb_arrow) {
        convert_to_bomb_arrow(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x20);
        ReloadSavedBank(gb);
    }
}

void UseItem(GBState *gb, uint8_t item, const ItemUsageCallbacks *callbacks) {
    if (!gb) return;
    switch (item) {
        case INVENTORY_SWORD:
            if (callbacks && callbacks->use_sword) callbacks->use_sword(gb);
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
            break;
        case INVENTORY_BOW:
            if (callbacks && callbacks->shoot_arrow) callbacks->shoot_arrow(gb);
            break;
        case INVENTORY_BOOMERANG:
            if (callbacks && callbacks->use_boomerang) callbacks->use_boomerang(gb);
            break;
        case INVENTORY_HOOKSHOT:
            UseHookshot(gb, callbacks ? callbacks->fire_hookshot : NULL);
            break;
        case INVENTORY_ROCS_FEATHER:
            if (callbacks && callbacks->use_rocs_feather) callbacks->use_rocs_feather(gb);
            break;
        case INVENTORY_OCARINA:
            if (callbacks && callbacks->use_ocarina) callbacks->use_ocarina(gb);
            break;
        case INVENTORY_MAGIC_POWDER:
            if (callbacks && callbacks->use_magic_powder) callbacks->use_magic_powder(gb);
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
