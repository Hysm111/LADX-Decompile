#include "home/link.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/sfx.h"
#include "constants/joypad.h"
#include "constants/inventory.h"
#include "constants/dialog.h"
#include "constants/link.h"

void disableMovementInTransition(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wLinkMotionState, LINK_MOTION_MAP_FADE_OUT);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, wD478, 0);
}

void playNoiseStairs(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hNoiseSfx, NOISE_SFX_STAIRS);
    disableMovementInTransition(gb);
}

void ApplyMapFadeOutTransitionWithNoise(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hMusicFadeOutTimer, 0x30);
    playNoiseStairs(gb);
}

void ApplyMapFadeOutTransition(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hMusicFadeOutTimer, 0x30);
    disableMovementInTransition(gb);
}

void ApplyMapFadeOutTransitionWithSound(GBState *gb) {
    if (!gb) return;

    uint8_t category = gb_read(gb, wWarp0MapCategory);
    uint8_t indoor = gb_read(gb, wIsIndoor);

    if (category == 1 && indoor != 0) {
        gb_write(gb, hContinueMusicAfterWarp, 1);
        playNoiseStairs(gb);
    } else {
        ApplyMapFadeOutTransitionWithNoise(gb);
    }
}

void ResetPegasusBoots(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wPegasusBootsChargeMeter, 0);
    gb_write(gb, wIsRunningWithPegasusBoots, 0);
}

void ResetSpinAttack(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wIsUsingSpinAttack, 0);
    gb_write(gb, wSwordCharge, 0);
    ResetPegasusBoots(gb);
}

void CopyLinkFinalPositionToPosition(GBState *gb) {
    if (!gb) return;

    uint8_t x = gb_read(gb, hLinkFinalPositionX);
    uint8_t y = gb_read(gb, hLinkFinalPositionY);
    gb_write(gb, hLinkPositionX, x);
    gb_write(gb, hLinkPositionY, y);
}

void UpdateLinkWalkingAnimation_trampoline(GBState *gb, void (*update_func)(GBState *)) {
    if (!gb) return;

    /* ld a, BANK(LinkAnimationsLists) -> bank 2 */
    gb_write(gb, rSelectROMBank, 0x02);
    if (update_func) {
        update_func(gb);
    }
    ReloadSavedBank(gb);
}

#include "home/audio.h"
#include "home/gameplay.h"
#include "home/check_items_to_use.h"
#include "home/vfx.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/link.h"
#include "constants/vfx.h"

static const uint8_t SwordCollisionMapX[12] = {
    0x16, 0xFA, 0x08, 0x08,
    0x16, 0x16, 0x08, 0xFA, 0xFA, 0xFA, 0x08, 0x16
};

static const uint8_t SwordCollisionMapY[12] = {
    0x08, 0x08, 0xFA, 0x16,
    0x08, 0x16, 0x16, 0x16, 0x08, 0xFA, 0xFA, 0xFA
};

static const uint8_t LinkDirectionToSwordCollisionRangeX[4] = {
    0x12, 0xEE, 0xFC, 0x04
};

static const uint8_t LinkDirectionToSwordCollisionRangeY[4] = {
    0x04, 0x04, 0xEE, 0x12
};

static const uint8_t XPositionIncrementPegasusRunning[4] = {
    32, (uint8_t)-32, 0, 0
};

static const uint8_t YPositionIncrementPegasusRunning[4] = {
    0, 0, (uint8_t)-32, 32
};

void CheckItemsSwordCollision(GBState *gb, uint8_t physics_flags, void (*alert_sword_moblins)(GBState *)) {
    if (!gb) return;
    if (gb_read(gb, wC16D) == 0) return;

    uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
    gb_write(gb, hMultiPurpose0, (uint8_t)(gb_read(gb, hLinkPositionX) + LinkDirectionToSwordCollisionRangeX[dir]));
    gb_write(gb, hMultiPurpose1, (uint8_t)(gb_read(gb, hLinkPositionY) + LinkDirectionToSwordCollisionRangeY[dir]));

    gb_write(gb, wSwordMoblinAlertingSoundCounter, 0x04);
    if (alert_sword_moblins) {
        alert_sword_moblins(gb);
    } else {
        AlertSwordMoblins(gb);
    }

    gb_write(gb, wC1C4, 0x10);

    if ((physics_flags & 0xF0) == 0x90) {
        gb_write(gb, hNoiseSfx, NOISE_SFX_CLINK);
    } else {
        gb_write(gb, hJingle, JINGLE_SWORD_POKING);
    }
}

void CheckStaticSwordCollision(GBState *gb,
                               uint8_t (*get_object_physics)(GBState *, uint8_t, uint8_t),
                               void (*reveal_object)(GBState *),
                               void (*start_smashing_rock)(GBState *, uint8_t),
                               void (*alert_sword_moblins)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wC1C4) != 0) return;
    if (gb_read(gb, wIsRunningWithPegasusBoots) == 0) {
        if (gb_read(gb, wC16A) == 0x05) return;
    }

    uint8_t dir;
    if (gb_read(gb, wIsUsingSpinAttack) != 0) {
        dir = (uint8_t)(gb_read(gb, wSwordDirection) + 4);
    } else {
        dir = gb_read(gb, hLinkDirection);
    }
    if (dir >= 12) dir = 0;

    /* Compute horizontal intersected area */
    uint8_t x = (uint8_t)(gb_read(gb, hLinkPositionX) + SwordCollisionMapX[dir] - 8) & 0xF0;
    gb_write(gb, hIntersectedObjectLeft, x);

    /* Compute vertical intersected area */
    uint8_t c = (uint8_t)((x >> 4) | (x << 4));
    uint8_t y = (uint8_t)(gb_read(gb, hLinkPositionY) + SwordCollisionMapY[dir] - 16) & 0xF0;
    gb_write(gb, hIntersectedObjectTop, y);

    /* Set hObjectUnderEntity */
    uint8_t e = y | c;
    uint16_t hl = (uint16_t)(wRoomObjects + e);
    if ((hl >> 8) != 0xD7) return;

    uint8_t obj = gb_read(gb, hl);
    gb_write(gb, hObjectUnderEntity, obj);

    uint8_t indoor = gb_read(gb, wIsIndoor);
    uint8_t flags = 0;
    if (get_object_physics) {
        flags = get_object_physics(gb, obj, indoor);
    }

    if ((flags >= 0xD0 && flags < 0xD4) || flags >= 0x90 || flags == 0x01) {
        CheckItemsSwordCollision(gb, flags, alert_sword_moblins);
        return;
    }

    uint8_t c_variant = 0;
    if (indoor == 0) {
        c_variant++;
        if (obj == 0xD3 || obj == 0x5C) {
            /* Keep c_variant = 1 */
        } else if (obj == 0x0A) {
            c_variant = 0xFF;
        } else {
            return;
        }
    } else {
        if (obj != 0xDD) return;
    }

    gb_write(gb, hActiveEntitySpriteVariant, c_variant);
    if (reveal_object) {
        reveal_object(gb);
    } else {
        gb_write(gb, rSelectROMBank, 0x21);
        ReloadSavedBank(gb);
    }

    if (gb_read(gb, wIsRunningWithPegasusBoots) == 0 && gb_read(gb, wC16A) == 0x05) {
        gb_write(gb, wSwordCharge, 0);
        gb_write(gb, wC16D, 0x0C);
    }

    uint8_t rock_idx = 0;
    bool spawned = SpawnPlayerProjectile(gb, ENTITY_LIFTABLE_ROCK, &rock_idx);
    if (spawned) {
        gb_write(gb, wLinkAttackStepAnimationCountdown, 0);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + rock_idx), (uint8_t)(gb_read(gb, hIntersectedObjectLeft) + 8));
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + rock_idx), (uint8_t)(gb_read(gb, hIntersectedObjectTop) + 16));
        gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + rock_idx), gb_read(gb, hActiveEntitySpriteVariant));
        if (start_smashing_rock) {
            start_smashing_rock(gb, rock_idx);
        }
    }

    /* Drop random item (~1/8 chance) */
    if ((GetRandomByte(gb) & 0x07) != 0) return;
    if (gb_read(gb, hObjectUnderEntity) == 0xD3) return; /* OBJECT_BUSH_GROUND_STAIRS */

    uint8_t rnd = GetRandomByte(gb);
    uint8_t drop_type = (rnd & 1) ? ENTITY_DROPPABLE_HEART : ENTITY_DROPPABLE_RUPEE;
    uint8_t drop_idx = 0;
    bool drop_spawned = SpawnPlayerProjectile(gb, drop_type, &drop_idx);
    if (!drop_spawned) return;

    gb_write(gb, (uint16_t)(wEntitiesPosXTable + drop_idx), (uint8_t)(gb_read(gb, hIntersectedObjectLeft) + 8));
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + drop_idx), (uint8_t)(gb_read(gb, hIntersectedObjectTop) + 16));
    gb_write(gb, (uint16_t)(wEntitiesSlowTransitionCountdownTable + drop_idx), 0x80);
    gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown1Table + drop_idx), 0x18);
    gb_write(gb, (uint16_t)(wEntitiesSpeedZTable + drop_idx), 0x10);
}

void CheckStaticSwordCollision_trampoline(GBState *gb,
                                          uint8_t (*get_object_physics)(GBState *, uint8_t, uint8_t),
                                          void (*reveal_object)(GBState *),
                                          void (*start_smashing_rock)(GBState *, uint8_t),
                                          void (*alert_sword_moblins)(GBState *)) {
    if (!gb) return;
    CheckStaticSwordCollision(gb, get_object_physics, reveal_object, start_smashing_rock, alert_sword_moblins);
    SwitchBank(gb, 0x02);
}

void DisplayTransientVfxForLinkRunning(GBState *gb) {
    if (!gb) return;

    uint8_t blocked = (gb_read(gb, hFrameCounter) & 0x07) |
                      gb_read(gb, hLinkPositionZ) |
                      gb_read(gb, hLinkInteractiveMotionBlocked) |
                      gb_read(gb, wIsLinkInTheAir);
    if (blocked != 0) return;

    gb_write(gb, hMultiPurpose0, gb_read(gb, hLinkPositionX));

    if (gb_read(gb, wLinkObjectPhysics) == OBJ_PHYSICS_SHALLOW_WATER) {
        gb_write(gb, hMultiPurpose1, gb_read(gb, hLinkPositionY));
        gb_write(gb, hJingle, JINGLE_WATER_SPLASH);
        AddTranscientVfx(gb, TRANSCIENT_VFX_PEGASUS_SPLASH);
    } else {
        gb_write(gb, hNoiseSfx, NOISE_SFX_FOOTSTEP);
        gb_write(gb, hMultiPurpose1, (uint8_t)(gb_read(gb, hLinkPositionY) + 0x06));
        AddTranscientVfx(gb, TRANSCIENT_VFX_PEGASUS_DUST);
    }
}

void ClearLinkPositionIncrement(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hLinkSpeedX, 0);
    gb_write(gb, hLinkSpeedY, 0);
}

void UsePegasusBoots(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsSideScrolling) != 0) {
        if (gb_read(gb, hLinkPhysicsModifier) != 0) return;
        if ((gb_read(gb, hLinkDirection) & DIRECTION_VERTICAL_MASK) != 0) return;
    }

    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) return;
    if ((gb_read(gb, hLinkPositionZ) | gb_read(gb, wIsLinkInTheAir)) != 0) return;

    gb_write(gb, wConsecutiveStepsCount, (uint8_t)(gb_read(gb, wConsecutiveStepsCount) + 2));
    DisplayTransientVfxForLinkRunning(gb);

    uint8_t meter = (uint8_t)(gb_read(gb, wPegasusBootsChargeMeter) + 1);
    gb_write(gb, wPegasusBootsChargeMeter, meter);

    if (meter != MAX_PEGASUS_BOOTS_CHARGE) return;

    gb_write(gb, wIsRunningWithPegasusBoots, meter);
    gb_write(gb, wIsUsingSpinAttack, 0);
    gb_write(gb, wSwordCharge, 0);

    uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
    gb_write(gb, hLinkSpeedX, XPositionIncrementPegasusRunning[dir]);
    gb_write(gb, hLinkSpeedY, YPositionIncrementPegasusRunning[dir]);
    gb_write(gb, wC1AC, 0);
}

#include "home/dialog.h"

static const uint8_t LinkAnimationsList_WalkingNoShield[8] = {
    LINK_ANIMATION_STATE_STANDING_RIGHT, LINK_ANIMATION_STATE_WALKING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,  LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_UP,    LINK_ANIMATION_STATE_WALKING_UP,
    LINK_ANIMATION_STATE_STANDING_DOWN,  LINK_ANIMATION_STATE_WALKING_DOWN
};

static const uint8_t LinkAnimationsList_WalkCarryingDefaultShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_RIGHT, LINK_ANIMATION_STATE_WALKING_SHIELD_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,         LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_UP,    LINK_ANIMATION_STATE_WALKING_SHIELD_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_DOWN,  LINK_ANIMATION_STATE_WALKING_SHIELD_DOWN
};

static const uint8_t LinkAnimationsList_WalkUsingDefaultShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_RIGHT, LINK_ANIMATION_STATE_WALKING_SHIELD_USE_RIGHT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_LEFT,  LINK_ANIMATION_STATE_WALKING_SHIELD_USE_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_UP,    LINK_ANIMATION_STATE_WALKING_SHIELD_USE_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_DOWN,  LINK_ANIMATION_STATE_WALKING_SHIELD_USE_DOWN
};

static const uint8_t LinkAnimationsList_WalkCarryingMirrorShield[8] = {
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_RIGHT, LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,                LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_UP,           LINK_ANIMATION_STATE_WALKING_SHIELD_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_DOWN,         LINK_ANIMATION_STATE_WALKING_SHIELD_DOWN
};

static const uint8_t LinkAnimationsList_WalkUsingMirrorShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_RIGHT,       LINK_ANIMATION_STATE_WALKING_SHIELD_USE_RIGHT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_LEFT,        LINK_ANIMATION_STATE_WALKING_SHIELD_USE_LEFT,
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_USE_UP,   LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_USE_UP,
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_USE_DOWN, LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_USE_DOWN
};

static const uint8_t LinkAnimationsList_PushingObject[8] = {
    LINK_ANIMATION_STATE_STANDING_PUSHING_RIGHT, LINK_ANIMATION_STATE_WALKING_PUSHING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_PUSHING_LEFT,  LINK_ANIMATION_STATE_WALKING_PUSHING_LEFT,
    LINK_ANIMATION_STATE_STANDING_PUSHING_UP,    LINK_ANIMATION_STATE_WALKING_PUSHING_UP,
    LINK_ANIMATION_STATE_STANDING_PUSHING_DOWN,  LINK_ANIMATION_STATE_WALKING_PUSHING_DOWN
};

static const uint8_t LinkAnimationsList_LiftingObject[8] = {
    LINK_ANIMATION_STATE_STANDING_LIFTING_RIGHT, LINK_ANIMATION_STATE_WALKING_LIFTING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LIFTING_LEFT,  LINK_ANIMATION_STATE_WALKING_LIFTING_LEFT,
    LINK_ANIMATION_STATE_STANDING_LIFTING_UP,    LINK_ANIMATION_STATE_WALKING_LIFTING_UP,
    LINK_ANIMATION_STATE_STANDING_LIFTING_DOWN,  LINK_ANIMATION_STATE_WALKING_LIFTING_DOWN
};

static const uint8_t Data_002_4948[8] = {
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_RIGHT, LINK_ANIMATION_STATE_MOVING_SWIMMING_1_RIGHT,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_LEFT,  LINK_ANIMATION_STATE_MOVING_SWIMMING_1_LEFT,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_UP,    LINK_ANIMATION_STATE_MOVING_SWIMMING_1_UP,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN,  LINK_ANIMATION_STATE_MOVING_SWIMMING_1_DOWN
};

static const uint8_t Data_002_4950[8] = {
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2
};

static const uint8_t LinkAnimationsList_WalkSideScrolling[8] = {
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_RIGHT_UP,  LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_RIGHT_UP,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_LEFT_DOWN, LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_LEFT_DOWN,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_RIGHT_UP,  LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_RIGHT_UP,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_LEFT_DOWN, LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_LEFT_DOWN
};

void func_1819(GBState *gb, void (*func_020_4ab3)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (func_020_4ab3) {
        func_020_4ab3(gb);
    }
    ReloadSavedBank(gb);
}

void func_1828(GBState *gb, void (*func_020_49ba)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (func_020_49ba) {
        func_020_49ba(gb);
    }
    ReloadSavedBank(gb);
}

void func_1A22(GBState *gb, void (*func_020_6c4f)(GBState *), void (*fade_out_music)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (func_020_6c4f) func_020_6c4f(gb);
    if (fade_out_music) fade_out_music(gb);
    ReloadSavedBank(gb);
}

void func_1A39(GBState *gb, void (*func_020_6c7a)(GBState *), void (*func_020_563b)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x20);
    if (func_020_6c7a) func_020_6c7a(gb);
    if (func_020_563b) func_020_563b(gb);
    ReloadSavedBank(gb);
}

void ApplyLinkMotionState(GBState *gb,
                          void (*func_002_753a)(GBState *),
                          void (*label_002_5310)(GBState *),
                          void (*label_002_538b)(GBState *)) {
    if (!gb) return;

    if (func_002_753a) {
        func_002_753a(gb);
    }

    if (gb_read(gb, wLinkMotionState) == LINK_MOTION_SWIMMING) {
        return;
    }

    if (gb_read(gb, wC16A) != 0) {
        uint8_t c145 = gb_read(gb, wC145);
        uint8_t c13b = gb_read(gb, wC13B);
        gb_write(gb, hMultiPurpose0, (uint8_t)(c145 + c13b));
        gb_write(gb, hMultiPurpose1, gb_read(gb, hLinkPositionX));
        gb_write(gb, hMultiPurpose3, 0);

        if (gb_read(gb, wSwordCharge) >= MAX_SWORD_CHARGE) {
            uint8_t frame = gb_read(gb, hFrameCounter);
            uint8_t flash = (uint8_t)((frame << 2) & 0x10);
            gb_write(gb, hMultiPurpose3, flash);
        }

        gb_write(gb, hMultiPurpose2, gb_read(gb, wSwordDirection));

        if (gb_read(gb, hLinkPositionY) >= 0x88) {
            return;
        }

        func_1819(gb, NULL);
        return;
    }

    uint8_t countdown = gb_read(gb, wLinkAttackStepAnimationCountdown);
    if ((countdown & 0x80) != 0) {
        if (label_002_5310) {
            label_002_5310(gb);
        }

        if ((gb_read(gb, wLinkAttackStepAnimationCountdown) & ATTACK_STEP_DURATION_MASK) == 0x0C) {
            if (gb_read(gb, wDialogState) == 0 && gb_read(gb, wRoomTransitionState) == 0) {
                UpdateLinkDirectionFromJoypad(gb);
                uint8_t fireball_idx = 0;
                bool spawned = SpawnPlayerProjectile(gb, ENTITY_MAGIC_ROD_FIREBALL, &fireball_idx);
                if (spawned) {
                    gb_write(gb, hNoiseSfx, NOISE_SFX_MAGIC_ROD);
                    if (label_002_538b) {
                        label_002_538b(gb);
                    }
                }
            }
        }
        gb_write(gb, wLinkAttackStepAnimationCountdown, countdown);
    }
}

void SetSpawnLocation(GBState *gb, uint16_t warp_data_addr) {
    if (!gb) return;
    gb_write(gb, hMultiPurpose0, 0);
    for (int i = 0; i < 5; i++) {
        uint8_t val = gb_read(gb, (uint16_t)(warp_data_addr + i));
        gb_write(gb, (uint16_t)(wSpawnLocationData + i), val);
        gb_write(gb, hMultiPurpose0, (uint8_t)(i + 1));
    }
    gb_write(gb, (uint16_t)(wSpawnLocationData + 5), gb_read(gb, wIndoorRoom));
}

void label_19DA(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hLinkDirection, 0);
}

void LinkMotionMapFadeInHandler(GBState *gb,
                                void (*func_002_754f)(GBState *),
                                void (*func_020_6c7a)(GBState *),
                                void (*func_020_563b)(GBState *)) {
    if (!gb) return;

    if (func_002_754f) {
        func_002_754f(gb);
    }

    if (gb_read(gb, wD474) != 0) {
        gb_write(gb, wD474, 0);
        gb_write(gb, wTransitionGfxFrameCount, 0x30);
        gb_write(gb, wTransitionGfx, TRANSITION_GFX_MANBO_OUT);
        gb_write(gb, wTransitionSequenceCounter, 0x04);
    } else {
        func_1A39(gb, func_020_6c7a, func_020_563b);
        if (gb_read(gb, wTransitionSequenceCounter) != 0x04) {
            return;
        }
    }

    uint8_t motion = (gb_read(gb, wD463) == 0x01) ? 0x01 : LINK_MOTION_DEFAULT;
    gb_write(gb, wLinkMotionState, motion);

    if (gb_read(gb, wDidStealItem) != 0) {
        gb_write(gb, wDidStealItem, 0);
        OpenDialogInTable0(gb, 0x36);
    }
}

void LinkMotionMapFadeOutHandler(GBState *gb,
                                 void (*func_002_754f)(GBState *),
                                 void (*func_020_6c4f)(GBState *),
                                 void (*fade_out_music)(GBState *),
                                 void (*label_004_7a5f)(GBState *),
                                 void (*load_minimap)(GBState *)) {
    if (!gb) return;

    if (func_002_754f) {
        func_002_754f(gb);
    }

    if (gb_read(gb, wC3C9) != 0) {
        gb_write(gb, wC3C9, 0);
        ApplyMapFadeOutTransitionWithNoise(gb);
        return;
    }

    func_1A22(gb, func_020_6c4f, fade_out_music);
    gb_write(gb, wScreenShakeCountdown, 0);
    gb_write(gb, wC1A8, 1);

    if (gb_read(gb, wTransitionSequenceCounter) != 0x04) {
        return;
    }

    gb_write(gb, hBaseScrollX, 0);
    gb_write(gb, hBaseScrollY, 0);
    gb_write(gb, hDungeonTitleMessageCountdown, 0);
    gb_write(gb, wBGPaletteTransitionEffect, 0);
    gb_write(gb, wDDD7, 0);

    for (int i = 0; i < 16; i++) {
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + i), 0);
    }

    if (gb_read(gb, wItemPickedUpInShop) != 0) {
        SwitchBank(gb, 4);
        if (label_004_7a5f) {
            label_004_7a5f(gb);
        }
        gb_write(gb, wIsThief, (uint8_t)(gb_read(gb, wIsThief) + 1));
        gb_write(gb, wHasStolenFromShop, (uint8_t)(gb_read(gb, wHasStolenFromShop) + 1));
        gb_write(gb, wPhotos1, (uint8_t)(gb_read(gb, wPhotos1) | 0x40));
        gb_write(gb, wDidStealItem, 1);
        gb_write(gb, hLinkAnimationState, 0);
    }

    uint8_t old_side_scrolling = gb_read(gb, hIsSideScrolling);
    gb_write(gb, hMultiPurposeD, old_side_scrolling);
    gb_write(gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(gb, wGameplaySubtype, 0);
    gb_write(gb, wObjectAffectingBGPalette, 0);
    gb_write(gb, hIsSideScrolling, 0);

    uint8_t old_is_indoor = gb_read(gb, wIsIndoor);
    gb_write(gb, hFreeWarpDataAddress, old_is_indoor);

    uint16_t warp_ptr = wWarpStructs;
    if (old_is_indoor == 0) {
        uint8_t link_tile = (uint8_t)(((gb_read(gb, hLinkPositionX) >> 4) & 0x0F) |
                                      ((gb_read(gb, hLinkPositionY) - 8) & 0xF0));
        uint8_t c = 4;
        for (uint8_t i = 0; i < 4; i++) {
            if (link_tile == gb_read(gb, (uint16_t)(wWarpPositions + i))) {
                c = i;
                break;
            }
        }
        warp_ptr = (uint16_t)(wWarp0MapCategory + c * 5);
    }

    uint8_t map_category = gb_read(gb, warp_ptr);
    gb_write(gb, wIsIndoor, map_category);
    if (map_category == 2) {
        gb_write(gb, hIsSideScrolling, 2);
        gb_write(gb, wIsIndoor, 1);
        gb_write(gb, hLinkPhysicsModifier, 1);
    }

    uint8_t map_id = gb_read(gb, (uint16_t)(warp_ptr + 1));
    gb_write(gb, hMapId, map_id);

    uint8_t map_room = gb_read(gb, (uint16_t)(warp_ptr + 2));
    gb_write(gb, hMapRoom, map_room);

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read(gb, hFreeWarpDataAddress) != 0) {
            gb_write(gb, wActivePowerUp, 0);
        }
    } else {
        SwitchBank(gb, 0x14);
        if (gb_read(gb, hFreeWarpDataAddress) == 0) {
            gb_write(gb, wActivePowerUp, 0);
            if (map_id < MAP_CAVE_B) {
                if (load_minimap) load_minimap(gb);
                gb_write(gb, hDungeonTitleMessageCountdown, 0x30);
                gb_write(gb, wSwitchBlocksState, 0);
                gb_write(gb, wSwitchableObjectAnimationStage, 0);
            }
        }
    }

    gb_write(gb, wMapEntrancePositionX, gb_read(gb, (uint16_t)(warp_ptr + 3)));
    gb_write(gb, wMapEntrancePositionY, gb_read(gb, (uint16_t)(warp_ptr + 4)));

    if (gb_read(gb, hIsSideScrolling) != 0) {
        label_19DA(gb);
        return;
    }

    if (gb_read(gb, hMultiPurposeD) != 0) {
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        SetSpawnLocation(gb, warp_ptr);
        return;
    }

    if (map_id >= 0x0A && map_id != MAP_COLOR_DUNGEON) {
        SetSpawnLocation(gb, warp_ptr);
        return;
    }

    SwitchBank(gb, 0x14);
    SetSpawnLocation(gb, warp_ptr);
    if (map_id == MAP_COLOR_DUNGEON) {
        gb_write(gb, (uint16_t)(wSpawnLocationData + 5), 0x3A);
    }
}

void UpdateLinkWalkingAnimation(GBState *gb) {
    if (!gb) return;

    int8_t steps = (int8_t)gb_read(gb, wConsecutiveStepsCount);
    uint8_t d = (uint8_t)((steps >> 3) & 0x01);

    uint8_t bc = (uint8_t)((gb_read(gb, hLinkDirection) << 1) | d);
    if (bc >= 8) bc = 0;

    const uint8_t *table = NULL;

    if (gb_read(gb, wLinkMotionState) == LINK_MOTION_SWIMMING) {
        if (gb_read(gb, hLinkPhysicsModifier) != 0) {
            table = Data_002_4950;
        } else {
            table = Data_002_4948;
        }
    } else if (gb_read(gb, hIsSideScrolling) != 0 && gb_read(gb, hLinkPhysicsModifier) == 2) {
        table = LinkAnimationsList_WalkSideScrolling;
    } else if (gb_read(gb, wIsCarryingLiftedObject) == 1) {
        table = LinkAnimationsList_LiftingObject;
    } else if (gb_read(gb, hLinkSlowWalkingSpeed) == 0 && gb_read(gb, wIsLinkPushing) != 0) {
        table = LinkAnimationsList_PushingObject;
    } else {
        uint8_t mirror = gb_read(gb, wHasMirrorShield);
        bool using_shield = (gb_read(gb, wIsUsingShield) != 0);

        if (mirror == 0) {
            table = LinkAnimationsList_WalkingNoShield;
        } else if (mirror == 1) {
            table = using_shield ? LinkAnimationsList_WalkUsingDefaultShield : LinkAnimationsList_WalkCarryingDefaultShield;
        } else {
            table = using_shield ? LinkAnimationsList_WalkUsingMirrorShield : LinkAnimationsList_WalkCarryingMirrorShield;
        }
    }

    gb_write(gb, hLinkAnimationState, table[bc]);
}

static const uint8_t SwordAreaXForDirection[4] = { 0x0C, 0x03, 0x08, 0x08 };
static const uint8_t SwordAreaYForDirection[4] = { 0x0A, 0x0A, 0x05, 0x10 };
static const uint8_t LinkDirectionToLinkAnimationState_2[4] = {
    LINK_ANIMATION_STATE_GRABBING_RIGHT,
    LINK_ANIMATION_STATE_GRABBING_LEFT,
    LINK_ANIMATION_STATE_GRABBING_UP,
    LINK_ANIMATION_STATE_GRABBING_DOWN
};
static const uint8_t LinkDirectionToLiftDirectionButton[4] = { 2, 1, 8, 4 };
static const uint8_t data_1F59[4] = { 0xFC, 4, 0, 0 };
static const uint8_t data_1F5D[4] = { 0, 0, 4, 0 };

void ComputeLinkPosition(GBState *gb, uint8_t direction) {
    if (!gb) return;

    uint8_t speed_raw = gb_read(gb, (uint16_t)(hLinkSpeedX + direction));
    uint8_t speed_swap_hi = (uint8_t)(((speed_raw << 4) | (speed_raw >> 4)) & 0xF0);

    uint8_t acc = gb_read(gb, (uint16_t)(wC11A + direction));
    uint16_t sum_acc = (uint16_t)acc + speed_swap_hi;
    gb_write(gb, (uint16_t)(wC11A + direction), (uint8_t)sum_acc);
    uint8_t carry = (sum_acc > 0xFF) ? 1 : 0;

    int8_t speed_int = (int8_t)speed_raw >> 4;
    uint8_t pos = gb_read(gb, (uint16_t)(hLinkPositionX + direction));
    uint8_t new_pos = (uint8_t)(pos + speed_int + carry);
    gb_write(gb, (uint16_t)(hLinkPositionX + direction), new_pos);
}

void UpdateFinalLinkPosition(GBState *gb) {
    if (!gb) return;
    if (gb_read(gb, wInventoryAppearing) != 0) {
        return;
    }
    ComputeLinkPosition(gb, 1); /* Vertical */
    ComputeLinkPosition(gb, 0); /* Horizontal */
}

void func_21E1(GBState *gb) {
    if (!gb) return;

    uint8_t vel_raw = gb_read(gb, hLinkVelocityZ);
    uint8_t vel_swap_hi = (uint8_t)(((vel_raw << 4) | (vel_raw >> 4)) & 0xF0);

    uint8_t acc = gb_read(gb, wC149);
    uint16_t sum_acc = (uint16_t)acc + vel_swap_hi;
    gb_write(gb, wC149, (uint8_t)sum_acc);
    uint8_t carry = (sum_acc > 0xFF) ? 1 : 0;

    int8_t vel_int = (int8_t)vel_raw >> 4;
    uint8_t pos_z = gb_read(gb, hLinkPositionZ);
    uint8_t new_pos_z = (uint8_t)(pos_z + vel_int + carry);
    gb_write(gb, hLinkPositionZ, new_pos_z);
}

void RevealObjectUnderObject_trampoline(GBState *gb, void (*reveal_object)(GBState *)) {
    if (!gb) return;
    gb_write(gb, rSelectROMBank, 0x14);
    if (reveal_object) {
        reveal_object(gb);
    }
    ReloadSavedBank(gb);
}

void label_2183(GBState *gb,
                uint8_t (*spawn_projectile)(GBState *, uint8_t),
                void (*func_003_5795)(GBState *)) {
    if (!gb) return;
    if (!spawn_projectile) return;

    uint8_t slot = spawn_projectile(gb, ENTITY_LIFTABLE_ROCK);
    if (slot == 0xFF) {
        return; /* Carry flag set: spawn failed */
    }

    gb_write(gb, hWaveSfx, WAVE_SFX_LIFT_UP);
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), 0x07);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + slot), gb_read(gb, hMultiPurposeE));

    SwitchBank(gb, 0x03);
    if (func_003_5795) {
        func_003_5795(gb);
    }
}

void func_2165(GBState *gb,
               void (*reveal_object)(GBState *),
               uint8_t (*spawn_projectile)(GBState *, uint8_t),
               void (*func_003_5795)(GBState *)) {
    if (!gb) return;
    gb_write(gb, hObjectUnderEntity, gb_read(gb, hMultiPurpose0));
    RevealObjectUnderObject_trampoline(gb, reveal_object);
    gb_write(gb, wC15D, gb_read(gb, hLinkDirection));
    label_2183(gb, spawn_projectile, func_003_5795);
}

void label_1F69(GBState *gb,
                uint8_t (*get_physics)(GBState *, uint8_t, uint8_t),
                void (*open_dialog_table0)(GBState *, uint8_t),
                void (*open_dialog_table1)(GBState *, uint8_t),
                void (*open_dialog_table2)(GBState *, uint8_t),
                void (*func_014_5900)(GBState *),
                void (*spawn_chest)(GBState *),
                void (*label_002_48b0)(GBState *),
                void (*func_014_50c3)(GBState *),
                void (*reveal_object)(GBState *),
                uint8_t (*spawn_projectile)(GBState *, uint8_t),
                void (*func_003_5795)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0 ||
        gb_read(gb, wIsCarryingLiftedObject) != 0 ||
        gb_read(gb, hLinkPositionZ) != 0 ||
        gb_read(gb, wLinkMotionState) != LINK_MOTION_DEFAULT) {
        return;
    }

    uint8_t dir = gb_read(gb, hLinkDirection) & 0x03;
    uint8_t obj_left = (uint8_t)(((gb_read(gb, hLinkPositionX) + SwordAreaXForDirection[dir] - 8)) & 0xF0);
    gb_write(gb, hIntersectedObjectLeft, obj_left);

    uint8_t c = (uint8_t)((obj_left >> 4) | (obj_left << 4));
    uint8_t obj_top = (uint8_t)(((gb_read(gb, hLinkPositionY) + SwordAreaYForDirection[dir] - 0x10)) & 0xF0);
    gb_write(gb, hIntersectedObjectTop, obj_top);

    uint8_t mp1 = (uint8_t)(obj_top | (c & 0x0F));
    gb_write(gb, hMultiPurpose1, mp1);

    uint16_t room_obj_addr = (uint16_t)(wRoomObjects + mp1);
    if ((room_obj_addr >> 8) != 0xD7) {
        gb_write(gb, wPullCounter, 0);
        return;
    }

    uint8_t obj_id = gb_read(gb, room_obj_addr);
    gb_write(gb, hMultiPurpose0, obj_id);

    uint8_t physics = 0;
    if (get_physics) {
        physics = get_physics(gb, gb_read(gb, wIsIndoor), obj_id);
    }
    gb_write(gb, hMultiPurpose5, physics);

    if (obj_id != 0x9A) {
        if (physics == 0x00 || physics == 0x50 || physics == 0x51 ||
            physics < 0x11 || physics >= 0xD4 || (physics >= 0x7C && physics < 0xD0)) {
            gb_write(gb, wPullCounter, 0);
            return;
        }

        if ((obj_id == 0x6F || obj_id == 0x5E || obj_id == 0xD4) && gb_read(gb, wIsIndoor) != 0) {
            goto jp_2098_label;
        }
    }

    if (dir == DIRECTION_UP) {
        gb_write(gb, wItemUsageContext, ITEM_USAGE_READING_TEXT);
        uint8_t joypad = gb_read(gb, hJoypadState);
        if ((joypad & (J_A | J_B)) != 0) {
            if (obj_id == OBJECT_WEATHER_VANE_BASE) {
                if (open_dialog_table1) open_dialog_table1(gb, Dialog18E);
                goto special_cases_end;
            }
            if (obj_id == OBJECT_OWL_STATUE || obj_id == OBJECT_SIGNPOST) {
                uint8_t ocarina = gb_read(gb, wOcarinaSongFlags);
                uint8_t sign_dialog = Dialog1A9;
                if (gb->rom) {
                    /* Bank 14 SignpostDialogTable */
                    size_t off = (size_t)0x14 * 0x4000 + (0x5118 - 0x4000) + gb_read(gb, hMapRoom);
                    if (off < gb->rom_size) sign_dialog = gb->rom[off];
                }
                if (sign_dialog == Dialog1A9 && (ocarina & 1)) {
                    sign_dialog = Dialog1AF;
                }
                if (sign_dialog == Dialog1AF && !(ocarina & 1)) {
                    uint8_t pos = (uint8_t)(((obj_left >> 4) & 0x0F) | (obj_top & 0xF0));
                    gb_write(gb, wMazeSignpostPos, pos);
                    goto special_cases_end;
                }
                if (sign_dialog == Dialog083) {
                    if (open_dialog_table0) open_dialog_table0(gb, Dialog083);
                } else if (sign_dialog == Dialog22D) {
                    if (open_dialog_table2) open_dialog_table2(gb, Dialog22D);
                } else {
                    if (open_dialog_table1) open_dialog_table1(gb, sign_dialog);
                }
                goto special_cases_end;
            }
            if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                if (open_dialog_table2) open_dialog_table2(gb, Dialog278);
                goto special_cases_end;
            }
            if (gb_read(gb, wSwordLevel) == 0 && gb_read(gb, hMapRoom) == ROOM_INDOOR_B_MARIN_HOUSE) {
                if (open_dialog_table0) open_dialog_table0(gb, Dialog0FF);
            } else if (gb_read(gb, hMapRoom) == UNKNOWN_ROOM_FA) {
                if (open_dialog_table0) open_dialog_table0(gb, Dialog0FC);
            } else {
                if (open_dialog_table0) open_dialog_table0(gb, Dialog0FD);
            }
            goto special_cases_end;
        }
    }

jp_2098_label:
    if (obj_id == OBJECT_CHEST_CLOSED) {
        if ((gb_read(gb, wRoomEvent) & EVENT_TRIGGER_MASK) != TRIGGER_THROW_POT_AT_CHEST &&
            dir == DIRECTION_UP &&
            (gb_read(gb, hJoypadState) & (J_A | J_B)) != 0) {
            if (gb_read(gb, hIsSideScrolling) != 0 || dir == DIRECTION_UP) {
                if (func_014_5900) {
                    gb_write(gb, rSelectROMBank, 0x14);
                    func_014_5900(gb);
                }
                if (spawn_chest) {
                    gb_write(gb, rSelectROMBank, 0x02);
                    spawn_chest(gb);
                }
            }
        }
    }

special_cases_end:
    {
        uint8_t pressed = gb_read(gb, hPressedButtonsMask);
        bool has_bracelet = false;
        if (gb_read(gb, (uint16_t)(wInventoryItems + 0)) == INVENTORY_POWER_BRACELET && (pressed & J_B)) {
            has_bracelet = true;
        } else if (gb_read(gb, (uint16_t)(wInventoryItems + 1)) == INVENTORY_POWER_BRACELET && (pressed & J_A)) {
            has_bracelet = true;
        }
        if (!has_bracelet) {
            return;
        }
    }

    if (label_002_48b0) {
        gb_write(gb, rSelectROMBank, 0x02);
        label_002_48b0(gb);
    }
    gb_write(gb, hLinkInteractiveMotionBlocked, 1);
    gb_write(gb, hLinkAnimationState, LinkDirectionToLinkAnimationState_2[dir]);

    if ((gb_read(gb, hPressedButtonsMask) & LinkDirectionToLiftDirectionButton[dir]) == 0) {
        gb_write(gb, wPullCounter, 0);
        return;
    }

    gb_write(gb, wC13C, data_1F59[dir]);
    gb_write(gb, wC13B, data_1F5D[dir]);
    gb_write(gb, hLinkAnimationState, (uint8_t)(gb_read(gb, hLinkAnimationState) + 1));

    uint8_t required = (gb_read(gb, wActivePowerUp) == ACTIVE_POWER_UP_PIECE_OF_POWER) ? 3 : 8;
    uint8_t pull = (uint8_t)(gb_read(gb, wPullCounter) + 1);
    gb_write(gb, wPullCounter, pull);
    if (pull < required) {
        return;
    }

    gb_write(gb, hMultiPurposeE, 0);
    if (obj_id == 0x8E || obj_id == 0x20) {
        func_2165(gb, reveal_object, spawn_projectile, func_003_5795);
        if (func_014_50c3) {
            gb_write(gb, rSelectROMBank, 0x14);
            func_014_50c3(gb);
            ReloadSavedBank(gb);
        }
        return;
    }

    if (gb_read(gb, wIsIndoor) != 0) {
        return;
    }

    if (obj_id == 0x5C) {
        gb_write(gb, hMultiPurposeE, 1);
    }
    func_2165(gb, reveal_object, spawn_projectile, func_003_5795);
}

void label_1F69_trampoline(GBState *gb,
                           uint8_t (*get_physics)(GBState *, uint8_t, uint8_t),
                           void (*open_dialog_table0)(GBState *, uint8_t),
                           void (*open_dialog_table1)(GBState *, uint8_t),
                           void (*open_dialog_table2)(GBState *, uint8_t),
                           void (*func_014_5900)(GBState *),
                           void (*spawn_chest)(GBState *),
                           void (*label_002_48b0)(GBState *),
                           void (*func_014_50c3)(GBState *),
                           void (*reveal_object)(GBState *),
                           uint8_t (*spawn_projectile)(GBState *, uint8_t),
                           void (*func_003_5795)(GBState *)) {
    label_1F69(gb, get_physics, open_dialog_table0, open_dialog_table1, open_dialog_table2,
               func_014_5900, spawn_chest, label_002_48b0, func_014_50c3,
               reveal_object, spawn_projectile, func_003_5795);
    SwitchBank(gb, 2);
}
