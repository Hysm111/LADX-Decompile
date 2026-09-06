#include "home/link.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/sfx.h"

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
