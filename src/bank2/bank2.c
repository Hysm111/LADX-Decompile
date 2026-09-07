#include "bank2/bank2.h"
#include "constants/directions.h"
#include "constants/joypad.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/physics.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "home/check_items_to_use.h"
#include "home/link.h"
#include "home/vfx.h"

const int8_t HookshotChainSpeedX[4] = {
    0x30,  /* DIRECTION_RIGHT:  HOOKSHOT_CHAIN_SPEED ($30) */
    -0x30, /* DIRECTION_LEFT:  -HOOKSHOT_CHAIN_SPEED (-$30) */
    0x00,  /* DIRECTION_UP:     0 */
    0x00   /* DIRECTION_DOWN:   0 */
};

const int8_t HookshotChainSpeedY[4] = {
    0x00,  /* DIRECTION_RIGHT:  0 */
    0x00,  /* DIRECTION_LEFT:   0 */
    -0x30, /* DIRECTION_UP:    -HOOKSHOT_CHAIN_SPEED (-$30) */
    0x30   /* DIRECTION_DOWN:   HOOKSHOT_CHAIN_SPEED ($30) */
};

const uint8_t DirectionToLinkAnimationState[4] = {
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT, /* 0: 0x11 */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT,  /* 1: 0x10 */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,    /* 2: 0x0F */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN   /* 3: 0x0E */
};

const int8_t HorizontalIncrementForLinkPosition[32] = {
    /* Normal walking (first 16 entries) */
    0x00, 0x10, (int8_t)0xF0, 0x00, 0x00, 0x0C, (int8_t)0xF4, 0x00,
    0x00, 0x0C, (int8_t)0xF4, 0x00, 0x00, 0x00, 0x00,         0x00,
    /* Walking with piece of power (next 16 entries) */
    0x00, 0x14, (int8_t)0xEC, 0x00, 0x00, 0x0F, (int8_t)0xF1, 0x00,
    0x00, 0x0F, (int8_t)0xF1, 0x00, 0x00, 0x00, 0x00,         0x00
};

const int8_t VerticalIncrementForLinkPosition[32] = {
    /* Normal walking (first 16 entries) */
    0x00,         0x00,         0x00,         0x00,
    (int8_t)0xF0, (int8_t)0xF4, (int8_t)0xF4, 0x00,
    0x10,         0x0C,         0x0C,         0x00,
    0x00,         0x00,         0x00,         0x00,
    /* Walking with piece of power (next 16 entries) */
    0x00,         0x00,         0x00,         0x00,
    (int8_t)0xEC, (int8_t)0xF1, (int8_t)0xF1, 0x00,
    0x14,         0x0F,         0x0F,         0x00,
    0x00,         0x00,         0x00,         0x00
};

const uint8_t JoypadToLinkDirection[11] = {
    DIRECTION_KEEP,  /* 0: none */
    DIRECTION_RIGHT, /* 1: right */
    DIRECTION_LEFT,  /* 2: left */
    DIRECTION_KEEP,  /* 3: right + left */
    DIRECTION_UP,    /* 4: up */
    DIRECTION_KEEP,  /* 5: up + right */
    DIRECTION_KEEP,  /* 6: up + left */
    DIRECTION_KEEP,  /* 7: up + right + left */
    DIRECTION_DOWN,  /* 8: down */
    DIRECTION_KEEP,  /* 9: down + right */
    DIRECTION_KEEP   /* 10: down + left */
};

const int8_t Data_002_44E7[6] = {
    0, -16, 16, 0, -1, 1
};

const int8_t Data_002_68B1[3] = {
    0x00, 0x10, (int8_t)0xF0
};

bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return false;

    uint16_t slot;
    if (spawn_new_entity) {
        slot = spawn_new_entity(gb, ENTITY_CHEST_WITH_ITEM);
        if (slot == 0xFFFF) {
            return false;
        }
        /* Callback typically returns an initialized entity slot; original decrement if needed */
        uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot));
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), (uint8_t)(status - 1));
    } else {
        /* Default slot search: find free slot scanning backwards from 15 down to 0 */
        int found = -1;
        for (int i = MAX_ENTITIES - 1; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) == ENTITY_STATUS_DISABLED) {
                found = i;
                break;
            }
        }
        if (found < 0) {
            return false;
        }
        slot = (uint16_t)found;
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), ENTITY_STATUS_INIT);
        gb_write(gb, (uint16_t)(wEntitiesTypeTable + slot), ENTITY_CHEST_WITH_ITEM);
    }

    /* Position chest: X = (hIntersectedObjectLeft & 0xF0) + 8, Y = (hIntersectedObjectTop & 0xF0) + 0x10 */
    uint8_t obj_left = (uint8_t)((gb_read_hram(gb, hIntersectedObjectLeft) & 0xF0) + 0x08);
    uint8_t obj_top = (uint8_t)((gb_read_hram(gb, hIntersectedObjectTop) & 0xF0) + 0x10);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), obj_left);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), obj_top);

    /* Variant: wEntitiesSpriteVariantTable[slot] = hMultiPurpose8 */
    uint8_t variant = gb_read_hram(gb, hMultiPurpose8);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + slot), variant);

    return true;
}

void UseOcarina(GBState *gb) {
    if (!gb) return;

    /* ld a, [wLinkPlayingOcarinaCountdown]; ld hl, wIsLinkInTheAir; or [hl];
     * ld hl, wIsUsingHookshot; or [hl]; ret nz */
    uint8_t countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    uint8_t in_air = gb_read(gb, wIsLinkInTheAir);
    uint8_t hookshot = gb_read(gb, wIsUsingHookshot);
    if ((countdown | in_air | hookshot) != 0) {
        return;
    }

    /* xor a; ld [wC5A4], a; ld [wC5A5], a */
    gb_write(gb, wC5A4, 0);
    gb_write(gb, wC5A5, 0);

    /* call CopyLinkFinalPositionToPosition */
    CopyLinkFinalPositionToPosition(gb);

    /* ld a, [wOcarinaSongFlags]; and $07; jr nz, .playSong */
    uint8_t song_flags = gb_read(gb, wOcarinaSongFlags) & 0x07;
    if (song_flags == 0) {
        /* wLinkPlayingOcarinaCountdown = $D0; hWaveSfx = WAVE_SFX_OCARINA_NOSONG */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_NOSONG);
        return;
    }

    /* ld a, [wSelectedSongIndex] */
    uint8_t song_idx = gb_read(gb, wSelectedSongIndex);
    switch (song_idx) {
        case 0:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xDC);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_BALLAD);
            break;
        case 1:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_MAMBO);
            break;
        case 2:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xBB);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_FROG);
            break;
        default:
            break;
    }
}

bool FireHookshot(GBState *gb) {
    if (!gb) return false;

    /* ld a, [wIsLinkInTheAir]; and a; ret nz */
    if (gb_read(gb, wIsLinkInTheAir) != 0) {
        return false;
    }

    /* Spawn hookshot chain projectile */
    uint8_t slot = 0;
    if (!SpawnPlayerProjectile(gb, ENTITY_HOOKSHOT_CHAIN, &slot)) {
        return false;
    }

    /* Transition countdown = $2A; variant = 0 */
    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + slot), 0x2A);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + slot), 0);

    /* Assign directional velocity vectors */
    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), (uint8_t)HookshotChainSpeedX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), (uint8_t)HookshotChainSpeedY[dir]);

    return true;
}

void func_002_4338(GBState *gb) {
    if (!gb) return;

    /* ld a, [wIsCarryingLiftedObject]; cp $02; jr c, .ret_4345 */
    uint8_t a = gb_read(gb, wIsCarryingLiftedObject);
    if (a < 0x02) {
        return;
    }

    /* ldh [hLinkAnimationState], a */
    gb_write_hram(gb, hLinkAnimationState, a);

    /* ld a, $01; ldh [hLinkInteractiveMotionBlocked], a */
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
}

void func_002_434A(GBState *gb) {
    if (!gb) return;

    /* ld a, [wLinkAttackStepAnimationCountdown]; and ATTACK_STEP_DURATION_MASK; jr z, .jr_4367 */
    uint8_t countdown = gb_read(gb, wLinkAttackStepAnimationCountdown);
    if ((countdown & ATTACK_STEP_DURATION_MASK) == 0) {
        /* xor a; ld [wLinkAttackStepAnimationCountdown], a; ret */
        gb_write(gb, wLinkAttackStepAnimationCountdown, 0);
        return;
    }

    /* dec a; ld [wLinkAttackStepAnimationCountdown], a */
    countdown--;
    gb_write(gb, wLinkAttackStepAnimationCountdown, countdown);

    /* ldh a, [hLinkDirection]; ld e, a; ld d, $00; ld hl, DirectionToLinkAnimationState; add hl, de */
    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    gb_write_hram(gb, hLinkAnimationState, DirectionToLinkAnimationState[dir]);
}

void MoveLinkToPressedButtonDirection(GBState *gb, uint8_t offset) {
    if (!gb) return;

    /* ldh a, [hPressedButtonsMask]; and $0F; or e; ld e, a */
    uint8_t idx = ((gb_read_hram(gb, hPressedButtonsMask) & 0x0F) | offset) & 0x1F;

    /* hLinkSpeedX = [HorizontalIncrementForLinkPosition + de] */
    gb_write_hram(gb, hLinkSpeedX, (uint8_t)HorizontalIncrementForLinkPosition[idx]);

    /* hLinkSpeedY = [VerticalIncrementForLinkPosition + de] */
    gb_write_hram(gb, hLinkSpeedY, (uint8_t)VerticalIncrementForLinkPosition[idx]);
}

void func_002_438F(GBState *gb, uint8_t offset) {
    if (!gb) return;

    /* ldh a, [hPressedButtonsMask]; and $0F; or e; ld e, a */
    uint8_t idx = ((gb_read_hram(gb, hPressedButtonsMask) & 0x0F) | offset) & 0x1F;

    /* Horizontal: sub [hl]; jr z, .jr_43A7; inc [hl]; bit 7, a; jr z, .jr_43A7; dec [hl]; dec [hl] */
    uint8_t target_x = (uint8_t)HorizontalIncrementForLinkPosition[idx];
    uint8_t speed_x = gb_read_hram(gb, hLinkSpeedX);
    uint8_t diff_x = (uint8_t)(target_x - speed_x);
    if (diff_x != 0) {
        if ((diff_x & 0x80) == 0) {
            speed_x++;
        } else {
            speed_x--;
        }
        gb_write_hram(gb, hLinkSpeedX, speed_x);
    }

    /* Vertical: sub [hl]; jr z, .ret_43B9; inc [hl]; bit 7, a; jr z, .ret_43B9; dec [hl]; dec [hl] */
    uint8_t target_y = (uint8_t)VerticalIncrementForLinkPosition[idx];
    uint8_t speed_y = gb_read_hram(gb, hLinkSpeedY);
    uint8_t diff_y = (uint8_t)(target_y - speed_y);
    if (diff_y != 0) {
        if ((diff_y & 0x80) == 0) {
            speed_y++;
        } else {
            speed_y--;
        }
        gb_write_hram(gb, hLinkSpeedY, speed_y);
    }
}

void shallowWaterVfx(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hLinkPositionY]; ldh [hMultiPurpose1], a */
    gb_write_hram(gb, hMultiPurpose1, gb_read_hram(gb, hLinkPositionY));

    /* ldh a, [hLinkPositionX]; ldh [hMultiPurpose0], a */
    gb_write_hram(gb, hMultiPurpose0, gb_read_hram(gb, hLinkPositionX));

    /* ld a, JINGLE_WATER_SPLASH; ldh [hJingle], a */
    gb_write_hram(gb, hJingle, JINGLE_WATER_SPLASH);

    /* ld a, TRANSCIENT_VFX_PEGASUS_SPLASH; jp AddTranscientVfx */
    AddTranscientVfx(gb, TRANSCIENT_VFX_PEGASUS_SPLASH);
}

void ApplyLinkGroundMotion(GBState *gb, void (*apply_ground_physics)(GBState *)) {
    if (!gb) return;

    /* ld a, [wIsLinkInTheAir]; and a; jp z, .return */
    if (gb_read(gb, wIsLinkInTheAir) == 0) {
        return;
    }

    /* ldh a, [hIsSideScrolling]; and a; jp nz, .return */
    if (gb_read_hram(gb, hIsSideScrolling) != 0) {
        return;
    }

    /* .noChecks: call func_21E1 */
    func_21E1(gb);

    /* hLinkVelocityZ = hLinkVelocityZ - 2 */
    uint8_t vel_z = gb_read_hram(gb, hLinkVelocityZ);
    vel_z = (uint8_t)(vel_z - 2);
    gb_write_hram(gb, hLinkVelocityZ, vel_z);

    /* ld a, -1; ld [wConsecutiveStepsCount], a */
    gb_write(gb, wConsecutiveStepsCount, 0xFF);

    /* ld a, [wC10A]; ld hl, wIsRunningWithPegasusBoots; or [hl]; jr nz, .joypadVerticalEnd */
    uint8_t c10a = gb_read(gb, wC10A);
    uint8_t running = gb_read(gb, wIsRunningWithPegasusBoots);
    if ((c10a | running) == 0) {
        /* ld a, [wD475]; and a; jr nz, .jr_002_451E */
        /* ld a, [wItemUsageContext]; cp ITEM_USAGE_ON_RAFT; jr nz, .jr_002_4523 */
        if (gb_read(gb, wD475) != 0 || gb_read(gb, wItemUsageContext) == ITEM_USAGE_ON_RAFT) {
            /* .jr_002_451E: call ClearLinkPositionIncrement; jr .joypadVerticalEnd */
            ClearLinkPositionIncrement(gb);
        } else {
            /* .jr_002_4523: Move Link to the direction pressed on the Joypad */
            uint8_t pressed = gb_read_hram(gb, hPressedButtonsMask);
            uint8_t horiz = pressed & (J_RIGHT | J_LEFT);
            if (horiz != 0) {
                /* ld hl, Data_002_68B1; add hl, de; ldh a, [hLinkSpeedX]; sub [hl] */
                uint8_t target_x = (uint8_t)Data_002_68B1[horiz];
                uint8_t speed_x = gb_read_hram(gb, hLinkSpeedX);
                uint8_t diff_x = (uint8_t)(speed_x - target_x);
                if (diff_x != 0) {
                    /* bit 7, a; jr nz, .jr_002_453D (+1); ld e, $FF (-1) */
                    if ((diff_x & 0x80) != 0) {
                        speed_x = (uint8_t)(speed_x + 1);
                    } else {
                        speed_x = (uint8_t)(speed_x - 1);
                    }
                    gb_write_hram(gb, hLinkSpeedX, speed_x);
                }
            }

            /* Joypad Vertical: ldh a, [hPressedButtonsMask]; rra; rra; and $03 */
            uint8_t vert = (pressed >> 2) & 0x03;
            if (vert != 0) {
                /* ld hl, Data_002_44E7; add hl, de; ldh a, [hLinkSpeedY]; sub [hl] */
                uint8_t target_y = (uint8_t)Data_002_44E7[vert];
                uint8_t speed_y = gb_read_hram(gb, hLinkSpeedY);
                uint8_t diff_y = (uint8_t)(speed_y - target_y);
                if (diff_y != 0) {
                    /* bit 7, a; jr nz, .jr_002_455E (+1); ld e, -1 */
                    if ((diff_y & 0x80) != 0) {
                        speed_y = (uint8_t)(speed_y + 1);
                    } else {
                        speed_y = (uint8_t)(speed_y - 1);
                    }
                    gb_write_hram(gb, hLinkSpeedY, speed_y);
                }
            }
        }
    }

    /* .joypadVerticalEnd:
     * ldh a, [hLinkPositionZ]; and a; jr z, .jr_002_456C; and $80; jr z, .return
     */
    uint8_t pos_z = gb_read_hram(gb, hLinkPositionZ);
    if (pos_z != 0 && (pos_z & 0x80) == 0) {
        return;
    }

    /* .jr_002_456C:
     * call ResetPegasusBoots
     * ldh [hLinkPositionZ], a (a = 0)
     * ld [wC149], a
     * ldh [hLinkVelocityZ], a
     * ld [wIsLinkInTheAir], a
     * ld [wC152], a
     * ld [wC153], a
     * ld [wC10A], a
     */
    ResetPegasusBoots(gb);
    gb_write_hram(gb, hLinkPositionZ, 0);
    gb_write(gb, wC149, 0);
    gb_write_hram(gb, hLinkVelocityZ, 0);
    gb_write(gb, wIsLinkInTheAir, 0);
    gb_write(gb, wC152, 0);
    gb_write(gb, wC153, 0);
    gb_write(gb, wC10A, 0);

    /* ldh a, [hLinkPositionY]; cp $88; jr nc, .return */
    uint8_t pos_y = gb_read_hram(gb, hLinkPositionY);
    if (pos_y >= 0x88) {
        return;
    }

    /* call ApplyLinkGroundPhysics */
    if (apply_ground_physics) {
        apply_ground_physics(gb);
    }

    /* ldh a, [hObjectUnderLink]; cp $61; jr z, .return */
    if (gb_read_hram(gb, hObjectUnderLink) == 0x61) {
        return;
    }

    /* ld a, [wLinkObjectPhysics] */
    uint8_t obj_physics = gb_read(gb, wLinkObjectPhysics);
    if (obj_physics == OBJ_PHYSICS_SHALLOW_WATER) {
        shallowWaterVfx(gb);
        return;
    }
    if (obj_physics == OBJ_PHYSICS_DEEP_WATER ||
        obj_physics == OBJ_PHYSICS_LAVA ||
        obj_physics == OBJ_PHYSICS_PIT ||
        obj_physics == OBJ_PHYSICS_PIT_WARP) {
        return;
    }

    /* ld a, NOISE_SFX_FOOTSTEP; ldh [hNoiseSfx], a */
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_FOOTSTEP);
}

void label_002_44B5(GBState *gb, void (*check_map_transition)(GBState *)) {
    if (!gb) return;

    /* ld a, [wLinkGroundStatus]; ld [wC130], a; xor a; ld [wLinkGroundStatus], a */
    uint8_t ground_status = gb_read(gb, wLinkGroundStatus);
    gb_write(gb, wC130, ground_status);
    gb_write(gb, wLinkGroundStatus, 0);

    /* jp CheckPositionForMapTransition */
    if (check_map_transition) {
        check_map_transition(gb);
    }
}

void func_002_44AD(GBState *gb, void (*check_map_transition)(GBState *)) {
    if (!gb) return;

    /* ld a, [wInventoryAppearing]; and a; ret nz */
    if (gb_read(gb, wInventoryAppearing) != 0) {
        return;
    }

    /* call UpdateFinalLinkPosition */
    UpdateFinalLinkPosition(gb);

    label_002_44B5(gb, check_map_transition);
}

bool func_002_44C2(GBState *gb, void (*check_map_transition)(GBState *)) {
    if (!gb) return false;

    /* ld a, [wIgnoreLinkCollisionsCountdown]; and a; ret z */
    uint8_t cd = gb_read(gb, wIgnoreLinkCollisionsCountdown);
    if (cd == 0) {
        return false;
    }

    /* dec a; ld [wIgnoreLinkCollisionsCountdown], a */
    cd--;
    gb_write(gb, wIgnoreLinkCollisionsCountdown, cd);

    /* call UpdateFinalLinkPosition */
    UpdateFinalLinkPosition(gb);

    /* call CheckPositionForMapTransition */
    if (check_map_transition) {
        check_map_transition(gb);
    }

    /* ld a, [wCollisionType]; and a; jr z, jr_002_44E3 */
    uint8_t col = gb_read(gb, wCollisionType);
    if (col != 0) {
        /* and $03; jr z, .jr_44E0 */
        if ((col & 0x03) != 0) {
            /* xor a; ldh [hLinkSpeedY], a */
            gb_write_hram(gb, hLinkSpeedY, 0);
        } else {
            /* .jr_44E0: xor a; ldh [hLinkSpeedX], a */
            gb_write_hram(gb, hLinkSpeedX, 0);
        }
    }

    /* jr_002_44E3: pop af; jp ApplyLinkMotionState */
    ApplyLinkMotionState(gb, NULL, NULL, NULL);
    return true;
}
