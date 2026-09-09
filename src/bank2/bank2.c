#include "bank2/bank2.h"
#include "bank2/audio.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/physics.h"
#include "constants/sfx.h"
#include "constants/vfx.h"
#include "constants/dialog.h"
#include "home/check_items_to_use.h"
#include "home/link.h"
#include "home/vfx.h"
#include "home/dialog.h"
#include "home/entities.h"
#include "constants/maps.h"
#include "constants/rooms.h"
#include "constants/gfx.h"
#include "home/bank.h"
#include "home/room.h"
#include "home/gameplay.h"
#include "bank1/room_transition.h"

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

const uint8_t LinkAnimationsList_WalkingNoShield[8] = {
    LINK_ANIMATION_STATE_STANDING_RIGHT, LINK_ANIMATION_STATE_WALKING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,  LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_UP,    LINK_ANIMATION_STATE_WALKING_UP,
    LINK_ANIMATION_STATE_STANDING_DOWN,  LINK_ANIMATION_STATE_WALKING_DOWN
};

const uint8_t LinkAnimationsList_WalkCarryingDefaultShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_RIGHT, LINK_ANIMATION_STATE_WALKING_SHIELD_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,         LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_UP,    LINK_ANIMATION_STATE_WALKING_SHIELD_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_DOWN,  LINK_ANIMATION_STATE_WALKING_SHIELD_DOWN
};

const uint8_t LinkAnimationsList_WalkUsingDefaultShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_RIGHT, LINK_ANIMATION_STATE_WALKING_SHIELD_USE_RIGHT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_LEFT,  LINK_ANIMATION_STATE_WALKING_SHIELD_USE_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_UP,    LINK_ANIMATION_STATE_WALKING_SHIELD_USE_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_DOWN,  LINK_ANIMATION_STATE_WALKING_SHIELD_USE_DOWN
};

const uint8_t LinkAnimationsList_WalkCarryingMirrorShield[8] = {
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_RIGHT, LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LEFT,                LINK_ANIMATION_STATE_WALKING_LEFT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_UP,           LINK_ANIMATION_STATE_WALKING_SHIELD_UP,
    LINK_ANIMATION_STATE_STANDING_SHIELD_DOWN,         LINK_ANIMATION_STATE_WALKING_SHIELD_DOWN
};

const uint8_t LinkAnimationsList_WalkUsingMirrorShield[8] = {
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_RIGHT,       LINK_ANIMATION_STATE_WALKING_SHIELD_USE_RIGHT,
    LINK_ANIMATION_STATE_STANDING_SHIELD_USE_LEFT,        LINK_ANIMATION_STATE_WALKING_SHIELD_USE_LEFT,
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_USE_UP,   LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_USE_UP,
    LINK_ANIMATION_STATE_STANDING_MIRROR_SHIELD_USE_DOWN, LINK_ANIMATION_STATE_WALKING_MIRROR_SHIELD_USE_DOWN
};

const uint8_t LinkAnimationsList_PushingObject[8] = {
    LINK_ANIMATION_STATE_STANDING_PUSHING_RIGHT, LINK_ANIMATION_STATE_WALKING_PUSHING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_PUSHING_LEFT,  LINK_ANIMATION_STATE_WALKING_PUSHING_LEFT,
    LINK_ANIMATION_STATE_STANDING_PUSHING_UP,    LINK_ANIMATION_STATE_WALKING_PUSHING_UP,
    LINK_ANIMATION_STATE_STANDING_PUSHING_DOWN,  LINK_ANIMATION_STATE_WALKING_PUSHING_DOWN
};

const uint8_t LinkAnimationsList_LiftingObject[8] = {
    LINK_ANIMATION_STATE_STANDING_LIFTING_RIGHT, LINK_ANIMATION_STATE_WALKING_LIFTING_RIGHT,
    LINK_ANIMATION_STATE_STANDING_LIFTING_LEFT,  LINK_ANIMATION_STATE_WALKING_LIFTING_LEFT,
    LINK_ANIMATION_STATE_STANDING_LIFTING_UP,    LINK_ANIMATION_STATE_WALKING_LIFTING_UP,
    LINK_ANIMATION_STATE_STANDING_LIFTING_DOWN,  LINK_ANIMATION_STATE_WALKING_LIFTING_DOWN
};

const uint8_t Data_002_4948[8] = {
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_RIGHT, LINK_ANIMATION_STATE_MOVING_SWIMMING_1_RIGHT,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_LEFT,  LINK_ANIMATION_STATE_MOVING_SWIMMING_1_LEFT,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_UP,    LINK_ANIMATION_STATE_MOVING_SWIMMING_1_UP,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN,  LINK_ANIMATION_STATE_MOVING_SWIMMING_1_DOWN
};

const uint8_t Data_002_4950[8] = {
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2,
    LINK_ANIMATION_STATE_HOLD_SWIMMING_2, LINK_ANIMATION_STATE_MOVING_SWIMMING_2
};

const uint8_t LinkAnimationsList_WalkSideScrolling[8] = {
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_RIGHT_UP,  LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_RIGHT_UP,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_LEFT_DOWN, LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_LEFT_DOWN,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_RIGHT_UP,  LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_RIGHT_UP,
    LINK_ANIMATION_STATE_STANDING_SIDE_SCROLL_LEFT_DOWN, LINK_ANIMATION_STATE_WALKING_SIDE_SCROLL_LEFT_DOWN
};

const uint8_t Data_002_49CA[72] = {
    0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01
};

const int8_t Data_002_4A12[2] = { 0x08, (int8_t)0xF8 };
const int8_t Data_002_4A14[2] = { 0x06, 0x01 };

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

void ApplyLinkGroundMotion_noChecks(GBState *gb, void (*apply_ground_physics)(GBState *)) {
    if (!gb) return;

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

    ApplyLinkGroundMotion_noChecks(gb, apply_ground_physics);
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

void OverheadWalkPhysics(GBState *gb, void (*check_map_transition)(GBState *)) {
    if (!gb) return;

    /* ld a, [wIndoorRoom]; ld [wD46B], a */
    gb_write(gb, wD46B, gb_read(gb, wIndoorRoom));

    /* call func_002_44C2 */
    if (func_002_44C2(gb, check_map_transition)) {
        return;
    }

    /* ldh a, [hLinkPositionZ]; and a; jr nz, .jr_43CE
     * ldh a, [hLinkInteractiveMotionBlocked]; and a; jp nz, label_002_44B5 */
    if (gb_read_hram(gb, hLinkPositionZ) == 0 && gb_read_hram(gb, hLinkInteractiveMotionBlocked) != 0) {
        label_002_44B5(gb, check_map_transition);
        return;
    }

    /* .jr_43CE: ld a, [wIsRunningWithPegasusBoots]; and a; jr z, jr_002_4402 */
    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
        /* ldh a, [hJoypadState]; and J_RIGHT | J_LEFT | J_UP | J_DOWN; jr z, .jr_43E6 */
        uint8_t joy_dir = gb_read_hram(gb, hJoypadState) & (J_RIGHT | J_LEFT | J_UP | J_DOWN);
        if (joy_dir != 0) {
            uint8_t target_dir = (joy_dir < sizeof(JoypadToLinkDirection)) ? JoypadToLinkDirection[joy_dir] : DIRECTION_KEEP;
            uint8_t link_dir = gb_read_hram(gb, hLinkDirection);
            if (link_dir != target_dir) {
                /* jr_002_43F4:
                 * ld [wC199+1], a
                 * ld a, [wC199]; add bashC; ld [wC199], a
                 * call ResetSpinAttack */
                gb_write(gb, (uint16_t)(wC199 + 1), link_dir);
                gb_write(gb, wC199, (uint8_t)(gb_read(gb, wC199) + 0x0C));
                ResetSpinAttack(gb);
                goto jr_002_4402;
            }
        }

        /* .jr_43E6:
         * ld a, [wConsecutiveStepsCount]; add bash2; ld [wConsecutiveStepsCount], a
         * call DisplayTransientVfxForLinkRunning
         * jp label_002_4464 */
        uint8_t steps = gb_read(gb, wConsecutiveStepsCount);
        gb_write(gb, wConsecutiveStepsCount, (uint8_t)(steps + 2));
        DisplayTransientVfxForLinkRunning(gb);
        goto label_002_4464;
    }

jr_002_4402:
    /* ld a, [wIsLinkInTheAir]; and a; jp nz, label_002_4464 */
    if (gb_read(gb, wIsLinkInTheAir) != 0) {
        goto label_002_4464;
    }

    /* ld a, [wActivePowerUp]; cp ACTIVE_POWER_UP_PIECE_OF_POWER; jr nz, .jr_4416; ld e, 0 */
    uint8_t offset = (gb_read(gb, wActivePowerUp) == ACTIVE_POWER_UP_PIECE_OF_POWER) ? 0x10 : 0x00;

    /* MoveLinkToPressedButtonDirection */
    MoveLinkToPressedButtonDirection(gb, offset);

    /* jr_002_442A: ld a, [wFreeMovementMode]; and a; jr z, .jr_443A */
    if (gb_read(gb, wFreeMovementMode) != 0) {
        gb_write_hram(gb, hLinkSpeedX, (uint8_t)(gb_read_hram(gb, hLinkSpeedX) << 1));
        gb_write_hram(gb, hLinkSpeedY, (uint8_t)(gb_read_hram(gb, hLinkSpeedY) << 1));
    }

    /* .jr_443A:
     * ld a, e; and bashF; ld e, a; jr z, jr_002_4459 */
    uint8_t pressed = gb_read_hram(gb, hPressedButtonsMask) & 0x0F;
    if (pressed == 0) {
        /* jr_002_4459:
         * ld a, [wPegasusBootsChargeMeter]; and a; jr nz, label_002_4464
         * ld a, bash7; ld [wConsecutiveStepsCount], a */
        if (gb_read(gb, wPegasusBootsChargeMeter) == 0) {
            gb_write(gb, wConsecutiveStepsCount, 0x07);
        }
    } else {
        /* inc [wConsecutiveStepsCount] */
        uint8_t steps = gb_read(gb, wConsecutiveStepsCount);
        gb_write(gb, wConsecutiveStepsCount, (uint8_t)(steps + 1));

        /* JoypadToLinkDirection */
        if (pressed < sizeof(JoypadToLinkDirection)) {
            uint8_t new_dir = JoypadToLinkDirection[pressed];
            if (new_dir != DIRECTION_KEEP) {
                if (gb_read(gb, wC16E) == 0) {
                    gb_write_hram(gb, hLinkDirection, new_dir);
                }
            }
        }
    }

label_002_4464:
    {
        uint8_t e_mask = 0x03;
        if (gb_read(gb, wIsGelClingingToLink) != 0) {
            /* e = 3 */
        } else {
            e_mask = 0x01;
            bool rooster_in_air = (gb_read(gb, wIsCarryingLiftedObject) != 0) &&
                                  (gb_read(gb, wLiftedEntityType) == ENTITY_ROOSTER) &&
                                  (gb_read(gb, wIsLinkInTheAir) != 0);
            if (rooster_in_air || gb_read_hram(gb, hLinkSlowWalkingSpeed) != 0) {
                /* e = 1 */
            } else {
                uint8_t ground_status = gb_read(gb, wLinkGroundStatus);
                if (ground_status == 0) {
                    func_002_44AD(gb, check_map_transition);
                    return;
                }
                if (ground_status != 0x07) {
                    /* jr_002_44A9: ldh a, [hFrameCounter]; and [hl]; ret z */
                    if ((gb_read_hram(gb, hFrameCounter) & ground_status) == 0) {
                        return;
                    }
                    func_002_44AD(gb, check_map_transition);
                    return;
                }
                /* ground_status == 0x07 */
                if (gb_read(gb, wFreeMovementMode) != 0) {
                    func_002_44AD(gb, check_map_transition);
                    return;
                }
                if (gb_read(gb, wPitSlippingCounter) >= PIT_MAX_SLIPPING) {
                    e_mask = 0x07;
                } else {
                    e_mask = 0x01;
                }
            }
        }

        /* jr_002_44A2:
         * ldh a, [hFrameCounter]; and e; jr nz, label_002_44B5; jr func_002_44AD */
        if ((gb_read_hram(gb, hFrameCounter) & e_mask) != 0) {
            label_002_44B5(gb, check_map_transition);
        } else {
            func_002_44AD(gb, check_map_transition);
        }
    }
}

void func_002_436C(GBState *gb,
                   void (*side_scrolling_physics)(GBState *),
                   void (*check_map_transition)(GBState *)) {
    if (!gb) return;

    /* ldh a, [hIsSideScrolling]; and a; jr z, jr_002_43BA */
    if (gb_read_hram(gb, hIsSideScrolling) == 0) {
        OverheadWalkPhysics(gb, check_map_transition);
        return;
    }

    /* ld a, [wFreeMovementMode]; and a; jr z, jp_002_68B7 */
    if (gb_read(gb, wFreeMovementMode) == 0) {
        if (side_scrolling_physics) {
            side_scrolling_physics(gb);
        }
        return;
    }

    /* jr jr_002_43BA */
    OverheadWalkPhysics(gb, check_map_transition);
}

void LinkMotionDefault(GBState *gb,
                       void (*side_scrolling_physics)(GBState *),
                       void (*check_map_transition)(GBState *),
                       void (*ocarina_handler)(GBState *),
                       void (*func_002_753a)(GBState *),
                       void (*update_link_animation)(GBState *),
                       void (*func_002_4b49)(GBState *),
                       void (*apply_ground_physics)(GBState *)) {
    if (!gb) return;

    /* Decrement wIsShootingArrow if not already zero */
    uint8_t arrow = gb_read(gb, wIsShootingArrow);
    if (arrow != 0) {
        gb_write(gb, wIsShootingArrow, (uint8_t)(arrow - 1));
    }

    /* Decrement wC1C4 if not already zero */
    uint8_t c1c4 = gb_read(gb, wC1C4);
    if (c1c4 != 0) {
        gb_write(gb, wC1C4, (uint8_t)(c1c4 - 1));
    }

    /* Decrement wBombArrowCooldown if not already zero */
    uint8_t bomb = gb_read(gb, wBombArrowCooldown);
    if (bomb != 0) {
        gb_write(gb, wBombArrowCooldown, (uint8_t)(bomb - 1));
    }

    /* call func_002_436C */
    func_002_436C(gb, side_scrolling_physics, check_map_transition);

    /* Decrement wC16E if not already zero */
    uint8_t c16e = gb_read(gb, wC16E);
    if (c16e != 0) {
        gb_write(gb, wC16E, (uint8_t)(c16e - 1));
    }

    /* ldh a, [hLinkInteractiveMotionBlocked]; cp bash2; jr nz, .interactiveMotionBlockedEnd */
    if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) == 0x02) {
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);
        gb_write_hram(gb, hLinkSpeedX, 0);
        gb_write_hram(gb, hLinkSpeedY, 0);
        gb_write_hram(gb, hLinkVelocityZ, 0);
        if (ocarina_handler) {
            ocarina_handler(gb);
        } else {
            LinkPlayingOcarinaHandler(gb, NULL, NULL);
        }
        if (func_002_753a) {
            func_002_753a(gb);
        }
        return;
    }

    /* .interactiveMotionBlockedEnd: */
    UpdateLinkWalkingAnimation(gb);
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);

    label_1F69_trampoline(gb, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CheckItemsToUse(gb, NULL, NULL, NULL);
    ApplyLinkGroundMotion(gb, apply_ground_physics);
    func_002_434A(gb);
    if (update_link_animation) {
        update_link_animation(gb);
    } else {
        UpdateLinkAnimation(gb, NULL);
    }
    if (func_002_4b49) {
        func_002_4b49(gb);
    } else {
        func_002_4B49(gb, NULL, NULL);
    }
    ApplyLinkMotionState(gb, NULL, NULL, NULL);
    func_002_4338(gb);
    if (ocarina_handler) {
        ocarina_handler(gb);
    } else {
        LinkPlayingOcarinaHandler(gb, NULL, NULL);
    }

    /* ld a, [wRoomTransitionState]; and a; jr nz, .return */
    if (gb_read(gb, wRoomTransitionState) != 0) {
        return;
    }

    /* ld a, [wSwordAnimationState]; ld [wC16A], a */
    uint8_t sword_anim = gb_read(gb, wSwordAnimationState);
    gb_write(gb, wC16A, sword_anim);

    if (sword_anim == SWORD_ANIMATION_STATE_HOLDING) {
        if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
            /* .resetSwordCharge: xor a; ld [wSwordCharge], a */
            gb_write(gb, wSwordCharge, 0);
            return;
        }

        /* xor a; ld [wSwordAnimationState], a */
        gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_NONE);

        uint8_t charge = gb_read(gb, wSwordCharge);
        if (charge == MAX_SWORD_CHARGE) {
            return;
        }
        charge++;
        gb_write(gb, wSwordCharge, charge);
        if (charge == MAX_SWORD_CHARGE) {
            gb_write_hram(gb, hJingle, JINGLE_CHARGING_SWORD);
        }
        return;
    }

    /* .lowerSword: */
    if (gb_read(gb, wItemUsageContext) == ITEM_USAGE_NEAR_NPC) {
        gb_write(gb, wSwordCharge, 0);
        return;
    }

    if (gb_read(gb, wSwordCharge) != MAX_SWORD_CHARGE) {
        gb_write(gb, wSwordCharge, 0);
        return;
    }

    /* wSwordCharge == MAX_SWORD_CHARGE */
    if (gb_read(gb, wC16E) == 0) {
        gb_write(gb, wIsUsingSpinAttack, USING_SPIN_ATTACK_MAX);
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_SPIN_ATTACK);
    }
    gb_write(gb, wSwordCharge, 0);
}

/* Bank 2 Sword & Spin Attack Tables */
const uint8_t LinkDirectionToStaticSwordCollitionCheckNeeded[24] = {
    0x00, 0x00, 0x08, 0x06,
    0x00, 0x06, 0x00, 0x00,
    0x08, 0x0A, 0x00, 0x0A,
    0x00, 0x00, 0x08, 0x10,
    0x00, 0x10, 0x00, 0x00,
    0x08, 0x08, 0x00, 0x08
};

const uint8_t LinkDirectionTo_wC141[24] = {
    0x00, 0x00, 0x05, 0x0A, 0x00, 0x0A, 0x00, 0x00,
    0x05, 0x0A, 0x00, 0x0A, 0x00, 0x00, 0x05, 0x08,
    0x00, 0x08, 0x00, 0x00, 0x05, 0x08, 0x00, 0x08
};

const uint8_t LinkDirectionToOffset[24] = {
    0x00, 0x00, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00,
    0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x08, 0x08,
    0x00, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x08
};

const uint8_t LinkDirectionTo_wC143[24] = {
    0x00, 0x00, 0x05, 0x08, 0x00, 0x08, 0x00, 0x00,
    0x05, 0x08, 0x00, 0x08, 0x00, 0x00, 0x05, 0x08,
    0x00, 0x08, 0x00, 0x00, 0x05, 0x08, 0x00, 0x08
};

const uint8_t LinkDirectionToSwordDirection[24] = {
    SWORD_DIRECTION_RIGHT,        SWORD_DIRECTION_TOP,    SWORD_DIRECTION_RIGHT_TOP,    SWORD_DIRECTION_RIGHT,
    SWORD_DIRECTION_RIGHT_BOTTOM, SWORD_DIRECTION_RIGHT,  SWORD_DIRECTION_RIGHT,        SWORD_DIRECTION_TOP,
    SWORD_DIRECTION_LEFT_TOP,     SWORD_DIRECTION_LEFT,   SWORD_DIRECTION_LEFT_BOTTOM,  SWORD_DIRECTION_LEFT,
    SWORD_DIRECTION_RIGHT,        SWORD_DIRECTION_RIGHT,  SWORD_DIRECTION_RIGHT_TOP,    SWORD_DIRECTION_TOP,
    SWORD_DIRECTION_LEFT_TOP,     SWORD_DIRECTION_TOP,    SWORD_DIRECTION_RIGHT,        SWORD_DIRECTION_LEFT,
    SWORD_DIRECTION_LEFT_BOTTOM,  SWORD_DIRECTION_BOTTOM, SWORD_DIRECTION_RIGHT_BOTTOM, SWORD_DIRECTION_BOTTOM
};

const uint8_t LinkDirectionToLinkAnimationState1[24] = {
    LINK_ANIMATION_STATE_STANDING_DOWN,        LINK_ANIMATION_STATE_UNKNOWN_18,          LINK_ANIMATION_STATE_UNKNOWN_19,          LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT,
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT, LINK_ANIMATION_STATE_HIDDEN,              LINK_ANIMATION_STATE_STANDING_DOWN,       LINK_ANIMATION_STATE_UNKNOWN_16,
    LINK_ANIMATION_STATE_UNKNOWN_17,           LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT, LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT, LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_STANDING_DOWN,        LINK_ANIMATION_STATE_UNKNOWN_14,          LINK_ANIMATION_STATE_UNKNOWN_15,          LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,    LINK_ANIMATION_STATE_HIDDEN,              LINK_ANIMATION_STATE_STANDING_DOWN,       LINK_ANIMATION_STATE_UNKNOWN_12,
    LINK_ANIMATION_STATE_UNKNOWN_13,           LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN, LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN, LINK_ANIMATION_STATE_HIDDEN
};

const int8_t LinkDirectionTo_wC13A[24] = {
    0x00, 0x00, 0x0D, 0x13, (int8_t)0x10, 0x0B, 0x00, (int8_t)0xF8,
    (int8_t)0xF3, (int8_t)0xED, (int8_t)0xF0, (int8_t)0xF5, 0x00, 0x10, 0x0D, (int8_t)0xF8,
    (int8_t)0xF5, (int8_t)0xF8, 0x00, (int8_t)0xF0, (int8_t)0xF3, 0x00, 0x0C, 0x00
};

const int8_t LinkDirectionTo_wC139[24] = {
    0x00, (int8_t)0xF0, (int8_t)0xF3, 0x00, 0x0C, 0x00, 0x00, (int8_t)0xF0,
    (int8_t)0xF3, 0x00, 0x0C, 0x00, 0x00, (int8_t)0xF8, (int8_t)0xF3, (int8_t)0xF0,
    (int8_t)0xF3, (int8_t)0xF5, 0x00, 0x00, 0x0D, 0x10, 0x0D, 0x0D
};

const int8_t LinkDirectionTo_wC13C[24] = {
    0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00,
    0x00, (int8_t)0xFD, (int8_t)0xFD, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const int8_t LinkDirectionTo_wC13B[24] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (int8_t)0xFD,
    (int8_t)0xFD, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03
};

const uint8_t SwordAnimationStateToUnknow[8] = {
    0x00, 0x03, 0x03, 0x08, 0x01, 0x01, 0x01, 0x01
};

const uint8_t UnkownToLinkStateTable[16] = {
    LINK_ANIMATION_STATE_UNKNOWN_61, LINK_ANIMATION_STATE_UNKNOWN_62, LINK_ANIMATION_STATE_UNKNOWN_63, LINK_ANIMATION_STATE_STANDING_DOWN,
    LINK_ANIMATION_STATE_JUMPING_1,  LINK_ANIMATION_STATE_JUMPING_2,  LINK_ANIMATION_STATE_JUMPING_3,  LINK_ANIMATION_STATE_STANDING_DOWN,
    LINK_ANIMATION_STATE_UNKNOWN_67, LINK_ANIMATION_STATE_UNKNOWN_68, LINK_ANIMATION_STATE_UNKNOWN_69, LINK_ANIMATION_STATE_STANDING_DOWN,
    LINK_ANIMATION_STATE_UNKNOWN_64, LINK_ANIMATION_STATE_UNKNOWN_65, LINK_ANIMATION_STATE_UNKNOWN_66, LINK_ANIMATION_STATE_STANDING_DOWN
};

const uint8_t FrameCounterToLinkDirection[4] = {
    DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_UP
};

const uint8_t LinkDirectionToSwordAnimationState[32] = {
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,  SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,  SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,  SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END,
    SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_START,  SWORD_ANIMATION_STATE_SWING_MIDDLE, SWORD_ANIMATION_STATE_SWING_END
};

const uint8_t LinkDirectionToAbsolute[32] = {
    DIRECTION_RIGHT, DIRECTION_UP,    DIRECTION_UP,    DIRECTION_LEFT,
    DIRECTION_LEFT,  DIRECTION_DOWN,  DIRECTION_DOWN,  DIRECTION_RIGHT,
    DIRECTION_LEFT,  DIRECTION_UP,    DIRECTION_UP,    DIRECTION_RIGHT,
    DIRECTION_RIGHT, DIRECTION_DOWN,  DIRECTION_DOWN,  DIRECTION_LEFT,
    DIRECTION_UP,    DIRECTION_RIGHT, DIRECTION_RIGHT, DIRECTION_DOWN,
    DIRECTION_DOWN,  DIRECTION_LEFT,  DIRECTION_LEFT,  DIRECTION_UP,
    DIRECTION_DOWN,  DIRECTION_LEFT,  DIRECTION_LEFT,  DIRECTION_UP,
    DIRECTION_UP,    DIRECTION_RIGHT, DIRECTION_RIGHT, DIRECTION_DOWN
};

void label_002_48B0(GBState *gb) {
    if (!gb) return;

    /* xor a; ld [wC1AC], a */
    gb_write(gb, wC1AC, 0);

    /* ld a, [wIsRunningWithPegasusBoots]; and a; jr nz, .return */
    if (gb_read(gb, wIsRunningWithPegasusBoots) != 0) {
        return;
    }

    /* xor a; ld [wSwordAnimationState], a; ld [wC16A], a; ld [wIsUsingSpinAttack], a */
    gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_NONE);
    gb_write(gb, wC16A, 0);
    gb_write(gb, wIsUsingSpinAttack, 0);
}

void label_002_4827(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;

    uint8_t link_dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t sword_anim = gb_read(gb, wSwordAnimationState);
    uint8_t bc = (uint8_t)((link_dir * 6) + sword_anim);
    if (bc >= sizeof(LinkDirectionToSwordDirection)) {
        bc = 0;
    }

    /* ld a, [hl]; ld [wSwordDirection], a */
    gb_write(gb, wSwordDirection, LinkDirectionToSwordDirection[bc]);

    /* ld a, [hl]; cp LINK_ANIMATION_STATE_HIDDEN; jr z, .noUpdate; ldh [hLinkAnimationState], a */
    uint8_t anim = LinkDirectionToLinkAnimationState1[bc];
    if (anim != LINK_ANIMATION_STATE_HIDDEN) {
        gb_write_hram(gb, hLinkAnimationState, anim);
    }

    /* .noUpdate: */
    gb_write(gb, wC13A, (uint8_t)LinkDirectionTo_wC13A[bc]);
    gb_write(gb, wC139, (uint8_t)LinkDirectionTo_wC139[bc]);
    gb_write(gb, wC13C, (uint8_t)LinkDirectionTo_wC13C[bc]);
    gb_write(gb, wC13B, (uint8_t)LinkDirectionTo_wC13B[bc]);

    uint8_t check_needed = LinkDirectionToStaticSwordCollitionCheckNeeded[bc];
    if (check_needed != 0) {
        bool skip_box = false;
        if (gb_read(gb, wIsUsingShield) != 0) {
            if ((gb_read_hram(gb, hFrameCounter) & 0x01) != 0) {
                skip_box = true;
            }
        }
        if (!skip_box) {
            /* .noShieldUsed: */
            uint8_t posX = gb_read_hram(gb, hLinkPositionX);
            uint8_t c13a = gb_read(gb, wC13A);
            gb_write(gb, wC140, (uint8_t)(c13a + check_needed + posX));
            gb_write(gb, wC141, LinkDirectionTo_wC141[bc]);

            uint8_t c139 = gb_read(gb, wC139);
            uint8_t offset = LinkDirectionToOffset[bc];
            uint8_t c145 = gb_read(gb, wC145);
            gb_write(gb, wC142, (uint8_t)(c139 + offset + c145));
            gb_write(gb, wC143, LinkDirectionTo_wC143[bc]);

            gb_write(gb, wSwordCollisionEnabled, 0x01);
        }
    }

    /* .checkStaticSwordCollision: */
    if (check_collision) {
        check_collision(gb);
    } else {
        CheckStaticSwordCollision_trampoline(gb, NULL, NULL, NULL, NULL);
    }
}

void label_002_476B(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;

    /* dec a; ld [wC16D], a; jp z, label_002_48B0 */
    uint8_t c16d = gb_read(gb, wC16D);
    c16d--;
    gb_write(gb, wC16D, c16d);
    if (c16d == 0) {
        label_002_48B0(gb);
        return;
    }

    /* ld hl, wC16E; ld [hl], bash4 */
    gb_write(gb, wC16E, 0x04);

    /* ld a, [wIsRunningWithPegasusBoots]; and a; jr nz, .jr_4781 */
    if (gb_read(gb, wIsRunningWithPegasusBoots) == 0) {
        /* ld a, bash1; ldh [hLinkInteractiveMotionBlocked], a */
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    }

    /* .jr_4781: ld a, SWORD_ANIMATION_STATE_SWING_MIDDLE; ld [wSwordAnimationState], a */
    gb_write(gb, wSwordAnimationState, SWORD_ANIMATION_STATE_SWING_MIDDLE);
    label_002_4827(gb, check_collision);
}

void UpdateSpinAttackAnimation(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;

    /* dec a; ld [wIsUsingSpinAttack], a */
    uint8_t spin = gb_read(gb, wIsUsingSpinAttack);
    spin--;
    gb_write(gb, wIsUsingSpinAttack, spin);

    /* ld hl, hLinkInteractiveMotionBlocked; ld [hl], TRUE */
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);

    /* de = (hLinkDirection << 3) + (wIsUsingSpinAttack >> 2) */
    uint8_t link_dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t de = (uint8_t)((link_dir << 3) + (spin >> 2));
    if (de >= sizeof(LinkDirectionToSwordAnimationState)) {
        de = 0;
    }

    /* ld a, [hl]; ld [wSwordAnimationState], a */
    gb_write(gb, wSwordAnimationState, LinkDirectionToSwordAnimationState[de]);

    /* Backup link direction */
    uint8_t orig_dir = gb_read_hram(gb, hLinkDirection);

    /* link direction = absolute direction in spin attack */
    gb_write_hram(gb, hLinkDirection, LinkDirectionToAbsolute[de]);

    /* call label_002_4827 */
    label_002_4827(gb, check_collision);

    /* Recover link direction */
    gb_write_hram(gb, hLinkDirection, orig_dir);

    /* ldh a, [hFrameCounter]; and bash3; jr nz, .jr_4745; dec [hl] */
    if ((gb_read_hram(gb, hFrameCounter) & 0x03) == 0) {
        spin = gb_read(gb, wIsUsingSpinAttack);
        spin--;
        gb_write(gb, wIsUsingSpinAttack, spin);
    }

    /* .jr_4745: ld a, [wIsUsingSpinAttack]; and a; jp z, label_002_48B0; rla; jp c, label_002_48B0 */
    spin = gb_read(gb, wIsUsingSpinAttack);
    if (spin == 0 || (spin & 0x80) != 0) {
        label_002_48B0(gb);
        return;
    }

    /* ldh a, [hLinkPositionX]; add bash8; ld [wC140], a */
    gb_write(gb, wC140, (uint8_t)(gb_read_hram(gb, hLinkPositionX) + 0x08));
    gb_write(gb, wC141, 0x18);
    gb_write(gb, wC143, 0x18);
    uint8_t c142 = (uint8_t)(gb_read(gb, wC145) + 0x08);
    gb_write(gb, wC142, c142);
    gb_write(gb, wSwordCollisionEnabled, c142);
}

void UpdateLinkAnimation(GBState *gb, void (*check_collision)(GBState *)) {
    if (!gb) return;

    /* ld a, [wD475]; and a; jr z, .rotateEnd */
    if (gb_read(gb, wD475) != 0) {
        /* rotate Link every 4th frame clockwise */
        uint8_t frame = gb_read_hram(gb, hFrameCounter);
        uint8_t dir_idx = (uint8_t)((frame >> 2) & 0x03);
        gb_write_hram(gb, hLinkDirection, FrameCounterToLinkDirection[dir_idx]);
        return;
    }

    /* .rotateEnd: if link is in the air, jump to .jr_002_47E0 */
    if (gb_read(gb, wIsLinkInTheAir) == 1) {
        uint8_t c3cf = gb_read(gb, wC3CF);
        uint8_t sword_anim = gb_read(gb, wSwordAnimationState);
        if ((c3cf | sword_anim) == 0) {
            uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
            uint8_t bc = (uint8_t)((dir << 2) & 0x0C);
            uint8_t c152 = gb_read(gb, wC152);
            if (c152 < 3) {
                uint8_t idx = (uint8_t)(bc + c152);
                if (idx < sizeof(UnkownToLinkStateTable)) {
                    gb_write_hram(gb, hLinkAnimationState, UnkownToLinkStateTable[idx]);
                }
                uint8_t c153 = (uint8_t)(gb_read(gb, wC153) + 1);
                gb_write(gb, wC153, c153);
                if ((c153 & 0x07) == 0) {
                    gb_write(gb, wC152, (uint8_t)(c152 + 1));
                }
            }
        }
    }

    /* .jr_002_47E0: */
    if (gb_read(gb, wIsUsingSpinAttack) != 0) {
        UpdateSpinAttackAnimation(gb, check_collision);
        return;
    }

    if (gb_read(gb, wC16D) != 0) {
        label_002_476B(gb, check_collision);
        return;
    }

    /* return if wSwordAnimationState == SWORD_ANIMATION_STATE_NONE */
    uint8_t sword_anim = gb_read(gb, wSwordAnimationState);
    if (sword_anim == SWORD_ANIMATION_STATE_NONE) {
        return;
    }

    gb_write(gb, wC16E, 0x04);
    if (sword_anim >= SWORD_ANIMATION_STATE_HOLDING) {
        label_002_4827(gb, check_collision);
        return;
    }

    if (gb_read(gb, wIsRunningWithPegasusBoots) == 0) {
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    }

    uint8_t c138 = gb_read(gb, wC138);
    if (c138 == 0) {
        sword_anim++;
        gb_write(gb, wSwordAnimationState, sword_anim);
        if (sword_anim == SWORD_ANIMATION_STATE_SWING_END) {
            label_002_48B0(gb);
            return;
        }
        if (sword_anim < sizeof(SwordAnimationStateToUnknow)) {
            c138 = SwordAnimationStateToUnknow[sword_anim];
        } else {
            c138 = 1;
        }
    }

    /* .jr_002_4823: dec a; ld [wC138], a */
    c138--;
    gb_write(gb, wC138, c138);

    label_002_4827(gb, check_collision);
}

void LinkMotionUnstuckingHandler(GBState *gb, void (*bg_collision_handler)(GBState *)) {
    if (!gb) return;

    /* ld a, $02; ld [wC1C4], a */
    gb_write(gb, wC1C4, 0x02);

    /* ldh a, [hLinkPhysicsModifier]; and a; jr nz, jr_002_49AA */
    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0) {
        /* ldh a, [hLinkPositionY]; add $10; ldh [hLinkPositionY], a; ldh [hLinkFinalPositionY], a */
        uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x10);
        gb_write_hram(gb, hLinkPositionY, pos_y);
        gb_write_hram(gb, hLinkFinalPositionY, pos_y);

        /* ldh a, [hLinkPositionZ]; add $10; ldh [hLinkPositionZ], a */
        uint8_t pos_z = (uint8_t)(gb_read_hram(gb, hLinkPositionZ) + 0x10);
        gb_write_hram(gb, hLinkPositionZ, pos_z);

        /* .loop_4978 */
        while (1) {
            /* ld a, $FF; ldh [hLinkSpeedY], a */
            gb_write_hram(gb, hLinkSpeedY, 0xFF);
            /* xor a; ldh [hLinkSpeedX], a */
            gb_write_hram(gb, hLinkSpeedX, 0x00);

            /* ldh a, [hLinkPositionY]; add $08; ldh [hLinkPositionY], a; ldh [hLinkFinalPositionY], a */
            pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x08);
            gb_write_hram(gb, hLinkPositionY, pos_y);
            gb_write_hram(gb, hLinkFinalPositionY, pos_y);

            /* ldh a, [hLinkPositionZ]; add $08; ldh [hLinkPositionZ], a */
            pos_z = (uint8_t)(gb_read_hram(gb, hLinkPositionZ) + 0x08);
            gb_write_hram(gb, hLinkPositionZ, pos_z);

            /* call BackgroundCollisionHandler */
            if (bg_collision_handler) {
                bg_collision_handler(gb);
            }

            /* ldh a, [hObjectUnderEntity]; cp $E1; jr z, .loop_4978 */
            uint8_t under = gb_read_hram(gb, hObjectUnderEntity);
            if (under == 0xE1) {
                continue;
            }

            /* cp $61; jr z, .jr_49A0 */
            if (under == 0x61) {
                break;
            }

            /* ld a, [wCollisionType]; and a; jr nz, .loop_4978 */
            if (gb_read(gb, wCollisionType) != 0) {
                continue;
            }
            break;
        }

        /* .jr_49A0: ld a, $01; ldh [hLinkPhysicsModifier], a */
        gb_write_hram(gb, hLinkPhysicsModifier, 0x01);
        /* ldh a, [hLinkPositionY]; sub $03; ldh [hLinkPositionY], a */
        pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x03);
        gb_write_hram(gb, hLinkPositionY, pos_y);
    }

    /* jr_002_49AA: call ApplyLinkGroundMotion.noChecks */
    ApplyLinkGroundMotion_noChecks(gb, NULL);

    /* ldh a, [hLinkPositionZ]; and a; jr nz, .jr_49B6 */
    if (gb_read_hram(gb, hLinkPositionZ) == 0) {
        /* xor a; ld [wLinkMotionState], a */
        gb_write(gb, wLinkMotionState, 0x00);
    }

    /* .jr_49B6: ld a, $01; ld [wIsLinkInTheAir], a */
    gb_write(gb, wIsLinkInTheAir, 0x01);

    /* call CheckItemsToUse */
    CheckItemsToUse(gb, NULL, NULL, NULL);

    /* call UpdateLinkAnimation */
    UpdateLinkAnimation(gb, NULL);

    /* ld a, [wSwordAnimationState]; ld [wC16A], a */
    gb_write(gb, wC16A, gb_read(gb, wSwordAnimationState));

    /* jp ApplyLinkMotionState */
    ApplyLinkMotionState(gb, NULL, NULL, NULL);
}

void LinkPlayingOcarinaHandler(GBState *gb,
                               void (*select_music_track)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    /* ld a, [wLinkPlayingOcarinaCountdown]; and a; ret z */
    uint8_t countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    if (countdown == 0) {
        return;
    }

    /* ld hl, hLinkInteractiveMotionBlocked; ld [hl], $02 */
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x02);

    /* cp $FF; jr nz, jr_002_4A7C */
    if (countdown == 0xFF) {
        /* ld a, [wD210]; add $01; ld [wD210], a; ld a, [wD211]; adc $00; ld [wD211], a */
        uint16_t d210_val = (uint16_t)(gb_read(gb, wD210) | (gb_read(gb, wD211) << 8));
        d210_val++;
        gb_write(gb, wD210, (uint8_t)(d210_val & 0xFF));
        gb_write(gb, wD211, (uint8_t)(d210_val >> 8));

        /* cp $08; jr nz, .jr_4A53; ld a, [wD210]; cp $D0; jr nz, .jr_4A53 */
        if (d210_val == 0x08D0) {
            /* xor a; ld [wLinkPlayingOcarinaCountdown], a; ld [wC167], a */
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0x00);
            gb_write(gb, wC167, 0x00);
            /* ld a, $03; ld [wC5A3], a */
            gb_write(gb, wC5A3, 0x03);

            /* ld a, [wD465]; cp $47; ret z */
            if (gb_read(gb, wD465) == 0x47) {
                return;
            }

            /* jr jr_002_4A6C */
            uint8_t slot = gb_read(gb, wD461);
            gb_write(gb, (uint16_t)(wEntitiesStateTable + slot), 0x00);
            if (select_music_track) {
                select_music_track(gb);
            } else {
                SelectMusicTrackAfterTransition(gb);
            }
            return;
        }

        /* .jr_4A53: ld a, [wD465]; cp $47; jr z, jr_002_4A7A */
        if (gb_read(gb, wD465) != 0x47) {
            /* ldh a, [hJoypadState]; and J_A | J_B; jr z, jr_002_4A7A */
            if ((gb_read_hram(gb, hJoypadState) & (J_A | J_B)) != 0) {
                /* xor a; ld [wLinkPlayingOcarinaCountdown], a; ld [wC167], a */
                gb_write(gb, wLinkPlayingOcarinaCountdown, 0x00);
                gb_write(gb, wC167, 0x00);
                /* ld a, $03; ld [wC5A3], a */
                gb_write(gb, wC5A3, 0x03);

                /* jr_002_4A6C */
                uint8_t slot = gb_read(gb, wD461);
                gb_write(gb, (uint16_t)(wEntitiesStateTable + slot), 0x00);
                if (select_music_track) {
                    select_music_track(gb);
                } else {
                    SelectMusicTrackAfterTransition(gb);
                }
                return;
            }
        }
        /* jr_002_4A7A -> jr jr_002_4AD1 */
    } else {
        /* jr_002_4A7C */
        ClearLinkPositionIncrement(gb);
        ResetSpinAttack(gb);

        /* dec [hl]; jr nz, jr_002_4AD1 */
        countdown--;
        gb_write(gb, wLinkPlayingOcarinaCountdown, countdown);
        if (countdown == 0) {
            /* ld a, [wIsMarinFollowingLink]; and a; jr z, .jr_4AA2 */
            if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                /* ld a, [wSelectedSongIndex]; cp $01; jr z, jr_002_4AB2 */
                /* ld a, [wIsIndoor]; and a; jr nz, jr_002_4AB2 */
                if (gb_read(gb, wSelectedSongIndex) != 0x01 && gb_read(gb, wIsIndoor) == 0) {
                    /* call_open_dialog Dialog277 */
                    OpenDialogInTable2(gb, 0x77);
                }
            } else {
                /* .jr_4AA2: ld a, [wOcarinaSongFlags]; and a; jr nz, jr_002_4AB2 */
                if (gb_read(gb, wOcarinaSongFlags) == 0) {
                    /* call_open_dialog Dialog08E */
                    OpenDialogInTable0(gb, 0x8E);
                    gb_write(gb, wC167, 0x00);
                    return;
                }
            }

            /* jr_002_4AB2 */
            gb_write(gb, wC167, 0x00);
            /* ld a, [wSelectedSongIndex]; cp $01; jr nz, .ret_4AD0 */
            if (gb_read(gb, wSelectedSongIndex) == 0x01) {
                /* ld a, TRANSITION_GFX_MANBO_IN; ld [wTransitionGfx], a */
                gb_write(gb, wTransitionGfx, TRANSITION_GFX_MANBO_IN);
                /* xor a; ld [wTransitionGfxFrameCount], a; ld [wTransitionSequenceCounter], a; ld [wC16C], a */
                gb_write(gb, wTransitionGfxFrameCount, 0x00);
                gb_write(gb, wTransitionSequenceCounter, 0x00);
                gb_write(gb, wC16C, 0x00);
                /* ld a, JINGLE_MANBO_WARP; ldh [hJingle], a */
                gb_write_hram(gb, hJingle, JINGLE_MANBO_WARP);
            }
            return;
        }
    }

    /* jr_002_4AD1 */
    uint8_t c5a4 = (uint8_t)(gb_read(gb, wC5A4) + 1);
    gb_write(gb, wC5A4, c5a4);
    if (c5a4 >= 0x38) {
        gb_write(gb, wC5A4, 0x00);
        uint8_t c5a5 = (uint8_t)(gb_read(gb, wC5A5) ^ 0x01);
        gb_write(gb, wC5A5, c5a5);
    }

    /* .jr_4AE8 */
    uint8_t c5a5 = gb_read(gb, wC5A5);
    uint8_t anim_state = (c5a5 != 0) ? LINK_ANIMATION_STATE_UNKNOWN_75 : (uint8_t)(LINK_ANIMATION_STATE_UNKNOWN_75 + 1);
    gb_write_hram(gb, hLinkAnimationState, anim_state);

    /* ld a, $02; ld [wC167], a; ld [wC111], a */
    gb_write(gb, wC167, 0x02);
    gb_write(gb, wC111, 0x02);

    /* ld a, [wLinkPlayingOcarinaCountdown]; cp $10; ret c */
    countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    if (countdown < 0x10) {
        return;
    }

    /* ld a, [wC5A4]; cp $14; jr nz, .ret_4B40 */
    if (gb_read(gb, wC5A4) != 0x14) {
        return;
    }

    /* ld a, ENTITY_MUSICAL_NOTE; call SpawnNewEntity_trampoline; jr c, .ret_4B40 */
    uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_MUSICAL_NOTE, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    /* ldh a, [hLinkPositionY]; sub $08; ld [wEntitiesPosYTable + de], a */
    uint8_t note_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x08);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), note_y);

    /* ld a, [wC5A5]; ld c, a; ld b, d; ld hl, Data_002_4A12; add hl, bc */
    /* ldh a, [hLinkPositionX]; add [hl]; ld [wEntitiesPosXTable + de], a */
    uint8_t bc_idx = (uint8_t)(gb_read(gb, wC5A5) & 1);
    uint8_t note_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + (uint8_t)Data_002_4A12[bc_idx]);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), note_x);

    /* ld hl, Data_002_4A14; add hl, bc; ld a, [hl]; ld [wEntitiesSpeedXTable + de], a */
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), (uint8_t)Data_002_4A14[bc_idx]);

    /* ld [wEntitiesSpeedYTable + de], $FC */
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), 0xFC);

    /* ld [wEntitiesInertiaTable + de], $40 */
    gb_write(gb, (uint16_t)(wEntitiesInertiaTable + slot), 0x40);
}

/* Bank 2 Shovel Digging & Animation Tables */
const uint8_t LinkDirectionToLinkAnimationState2[8] = {
    0x71, 0x72, 0x6F, 0x70, 0x73, 0x74, 0x6D, 0x6E
};

const int8_t LinkDirectionToAdjacentTileIndexX[4] = {
    0x14, (int8_t)0xFC, 0x08, 0x08
};

const int8_t LinkDirectionToAdjacentTileIndexY[4] = {
    0x0A, 0x0A, (int8_t)0xFC, 0x14
};

void func_002_4BD4(GBState *gb, uint16_t hl) {
    if (!gb) return;

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, bg_low);
    gb_write(gb, hl++, 0x81);

    if (gb_read(gb, wIsIndoor) == 0) {
        gb_write(gb, hl++, 0x6A);
        gb_write(gb, hl++, 0x7A);
        gb_write(gb, hl++, bg_high);
        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, hl++, 0x81);
        gb_write(gb, hl++, 0x6B);
        gb_write(gb, hl++, 0x7B);
    } else {
        gb_write(gb, hl++, 0x04);
        gb_write(gb, hl++, 0x06);
        gb_write(gb, hl++, bg_high);
        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, hl++, 0x81);
        gb_write(gb, hl++, 0x05);
        gb_write(gb, hl++, 0x07);
    }

    gb_write(gb, hl++, 0x00);
}

void func_002_4C14(GBState *gb, uint16_t hl) {
    if (!gb) return;

    uint8_t vram1_size = gb_read(gb, wDrawCommandsVRAM1Size);
    uint16_t de = (uint16_t)(wDrawCommandVRAM1 + vram1_size);
    gb_write(gb, wDrawCommandsVRAM1Size, (uint8_t)(vram1_size + 0x0A));

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, de++, bg_high);

    gb_write(gb, hl++, bg_low);
    gb_write(gb, de++, bg_low);

    gb_write(gb, hl++, 0x81);
    gb_write(gb, de++, 0x81);

    if (gb_read(gb, wIsIndoor) == 0) {
        gb_write(gb, hl++, 0x6A);
        gb_write(gb, hl++, 0x7A);
        gb_write(gb, de++, 0x03);
        gb_write(gb, de++, 0x03);

        gb_write(gb, hl++, bg_high);
        gb_write(gb, de++, bg_high);

        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, de++, (uint8_t)(bg_low + 1));

        gb_write(gb, hl++, 0x81);
        gb_write(gb, de++, 0x81);

        gb_write(gb, hl++, 0x6B);
        gb_write(gb, de++, 0x03);
        gb_write(gb, de++, 0x03);
        gb_write(gb, hl++, 0x7B);
    } else {
        gb_write(gb, hl++, 0x04);
        gb_write(gb, hl++, 0x06);
        gb_write(gb, de++, 0x05);
        gb_write(gb, de++, 0x05);

        gb_write(gb, hl++, bg_high);
        gb_write(gb, de++, bg_high);

        gb_write(gb, hl++, (uint8_t)(bg_low + 1));
        gb_write(gb, de++, (uint8_t)(bg_low + 1));

        gb_write(gb, hl++, 0x81);
        gb_write(gb, de++, 0x81);

        gb_write(gb, hl++, 0x05);
        gb_write(gb, de++, 0x05);
        gb_write(gb, de++, 0x05);
        gb_write(gb, hl++, 0x07);
    }

    gb_write(gb, hl++, 0x00);
    gb_write(gb, de++, 0x00);
}

void label_002_4C92(GBState *gb,
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                    void (*apply_vector)(GBState *)) {
    if (!gb) return;

    uint8_t mp1 = gb_read_hram(gb, hMultiPurpose1);
    uint16_t room_obj_addr = (uint16_t)(wRoomObjects + mp1);
    gb_write(gb, room_obj_addr, OBJECT_SHOVEL_HOLE);

    BackupObjectInRAM2(gb, room_obj_addr, 0x82);
    GetIntersectedObjectBGAddress(gb);

    uint8_t draw_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + draw_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(draw_size + 0x0A));

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_002_4C14(gb, hl);
    } else {
        func_002_4BD4(gb, hl);
    }

    /* Random drops (1 in 8 chance) */
    if ((GetRandomByte(gb) & 0x07) != 0) {
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_EAGLES_TOWER) {
            return;
        }
    }

    uint8_t rand_drop = GetRandomByte(gb);
    uint8_t drop_type = (rand_drop & 1) ? ENTITY_DROPPABLE_HEART : ENTITY_DROPPABLE_RUPEE;

    uint16_t slot = SpawnNewEntity_trampoline(gb, drop_type, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    uint8_t left = (uint8_t)(gb_read_hram(gb, hIntersectedObjectLeft) + 0x08);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), left);

    uint8_t top = (uint8_t)(gb_read_hram(gb, hIntersectedObjectTop) + 0x10);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), top);

    gb_write(gb, (uint16_t)(wEntitiesSlowTransitionCountdownTable + slot), 0x80);
    gb_write(gb, (uint16_t)(wEntitiesPrivateCountdown1Table + slot), 0x18);
    gb_write(gb, (uint16_t)(wEntitiesSpeedZTable + slot), 0x20);

    /* ApplyVectorTowardsLink_trampoline with speed $0C */
    ApplyVectorTowardsLink_trampoline(gb, apply_vector);

    uint8_t spd_y = (uint8_t)(-(int8_t)gb_read_hram(gb, hMultiPurpose0));
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), spd_y);

    uint8_t spd_x = (uint8_t)(-(int8_t)gb_read_hram(gb, hMultiPurpose1));
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), spd_x);
}

bool func_002_4D20(GBState *gb) {
    if (!gb) return false;

    /* jump to .done if one or more is true:
     * - Link carry something
     * - Link is in the air
     * - Link is in motion
     * - screen scrolling is happening
     */
    uint8_t carrying = gb_read(gb, wIsCarryingLiftedObject);
    uint8_t pos_z = gb_read_hram(gb, hLinkPositionZ);
    uint8_t motion = gb_read(gb, wLinkMotionState);
    uint8_t scrolling = gb_read_hram(gb, hIsSideScrolling);
    if ((carrying | pos_z | motion | scrolling) != 0) {
        return false; /* carry set */
    }

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t posX = gb_read_hram(gb, hLinkPositionX);
    uint8_t left = (uint8_t)((posX + (uint8_t)LinkDirectionToAdjacentTileIndexX[dir] - 0x08) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectLeft, left);
    uint8_t c = (uint8_t)(left >> 4);

    uint8_t posY = gb_read_hram(gb, hLinkPositionY);
    uint8_t top = (uint8_t)((posY + (uint8_t)LinkDirectionToAdjacentTileIndexY[dir] - 0x10) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectTop, top);

    uint8_t tile_idx = (uint8_t)(top | c);
    gb_write_hram(gb, hMultiPurpose1, tile_idx);

    uint16_t room_obj_addr = (uint16_t)(wRoomObjects + tile_idx);
    if ((room_obj_addr >> 8) != 0xD7) {
        return false; /* carry set */
    }

    uint8_t obj = gb_read(gb, room_obj_addr);
    gb_write_hram(gb, hMultiPurpose0, obj);

    uint8_t is_indoor = gb_read(gb, wIsIndoor);
    uint16_t de = (uint16_t)((is_indoor << 8) | obj);
    uint8_t flags = GetObjectPhysicsFlags_trampoline(gb, de);
    if (flags != 0) {
        return false; /* carry set */
    }

    if (is_indoor != 0) {
        if (obj != 0x05) {
            return false; /* carry set */
        }
    } else {
        if (obj == 0x0C || obj == 0x0D || obj == 0xB9) {
            return false; /* carry set */
        }
    }

    return true; /* carry clear */
}

bool func_002_4BC8(GBState *gb,
                   uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                   void (*apply_vector)(GBState *)) {
    if (!gb) return false;

    if (!func_002_4D20(gb)) {
        return false;
    }

    gb_write(gb, wLinkUsingShovel, 0x02);
    label_002_4C92(gb, spawn_new_entity, apply_vector);
    return true;
}

void func_002_4B49(GBState *gb,
                   void (*func_020_4b4a)(GBState *),
                   bool (*shovel_dig_action)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wLinkUsingShovel) == 0) {
        return;
    }

    if (gb_read_hram(gb, hMapId) == MAP_WINDFISHS_EGG) {
        if (gb_read(gb, wFinalNightmareForm) == 0x02) {
            func_020_4B4A_trampoline(gb, func_020_4b4a);
            gb_write(gb, wSwordCollisionEnabled, 0x01);
        }
    }

    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    ClearLinkPositionIncrement(gb);

    gb_write(gb, wSwordAnimationState, 0x00);
    gb_write(gb, wIsUsingSpinAttack, 0x00);
    gb_write(gb, wSwordCharge, 0x00);

    uint8_t timer = (uint8_t)(gb_read(gb, wLinkUsingShovelTimer) + 1);
    gb_write(gb, wLinkUsingShovelTimer, timer);

    if (timer == 0x10) {
        if (shovel_dig_action) {
            shovel_dig_action(gb);
        } else {
            func_002_4BC8(gb, NULL, NULL);
        }
    }

    if (timer == 0x18) {
        if (gb_read(gb, wLinkUsingShovel) == 0x02) {
            if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                if (gb_read(gb, wDialogState) == 0) {
                    OpenDialogInTable2(gb, 0x79);
                }
            }
        }
        gb_write(gb, wLinkUsingShovel, 0x00);
        gb_write(gb, wC1AC, 0x00);
        return;
    }

    uint8_t e = (uint8_t)((timer >> 4) & 1);
    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 3;
    uint8_t idx = (uint8_t)((dir << 1) + e);
    gb_write_hram(gb, hLinkAnimationState, LinkDirectionToLinkAnimationState2[idx]);
}

/* Bank 2 Revolving Door & Tile/Palette Data Tables */
const uint8_t Data_002_4E1C[16] = {
    0xFF, 0x47, 0x00, 0x00, 0x0C, 0x39, 0x76, 0x5E,
    0xFF, 0x47, 0x00, 0x00, 0x67, 0x28, 0x76, 0x5E
};

const uint8_t LinkRevolvingDoorAnimation[7] = {
    LINK_ANIMATION_STATE_REVOLVING_DOOR_1,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_2,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_3,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_4,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_4,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_5,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_3
};

void label_002_4D97(GBState *gb, void (*get_bg_attr_addr)(GBState *)) {
    if (!gb) return;

    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    gb_write_hram(gb, hIntersectedObjectLeft, mp0);
    uint8_t e = (uint8_t)((mp0 >> 4) & 0x0F);

    uint8_t mp1 = gb_read_hram(gb, hMultiPurpose1);
    gb_write_hram(gb, hIntersectedObjectTop, mp1);
    uint8_t tile_idx = (uint8_t)((mp1 & 0xF0) | e);

    gb_write(gb, (uint16_t)(wRoomObjects + tile_idx), 0xAE);
    GetIntersectedObjectBGAddress(gb);

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_91D_jp_92E(gb, 0x02B8, 0x02, get_bg_attr_addr);
    }

    uint8_t draw_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + draw_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(draw_size + 0x0A));

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, bg_low);
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, 0x76);
    gb_write(gb, hl++, 0x77);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, (uint8_t)(bg_low + 1));
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, 0x76);
    gb_write(gb, hl++, 0x77);

    gb_write(gb, hl, 0x00);
}

void func_002_4DFC(GBState *gb) {
    if (!gb) return;

    for (uint8_t c = 0; c < 8; c++) {
        uint8_t b = gb->wram[1][(wObjPal1 + c) - 0xD000];
        gb->wram[2][(wObjPal1 + c) - 0xD000] = b;
    }
    gb_write_hram(gb, rSVBK, 0);
}

void func_002_4E2C(GBState *gb, uint16_t de) {
    if (!gb) return;

    for (uint8_t c = 0; c < 8; c++) {
        uint8_t val = Data_002_4E1C[(de + c) & 0x0F];
        gb_write(gb, (uint16_t)(wObjPal8 + c), val);
    }
    gb_write(gb, wPaletteDataFlags, 0x02);
}

void func_002_4E48(GBState *gb) {
    if (!gb) return;

    for (uint8_t e = 0; e < 8; e++) {
        uint8_t b = gb->wram[2][(wObjPal8 + e) - 0xD000];
        gb->wram[1][(wObjPal8 + e) - 0xD000] = b;
    }
    gb_write_hram(gb, rSVBK, 0);
    gb_write(gb, wPaletteDataFlags, 0x02);
}

void func_002_4EDD(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wLinkAnimationFrame, 0);
    gb_write(gb, wC167, 0);
    gb_write(gb, wBGPaletteTransitionEffect, 0);
    gb_write(gb, wDDD7, 0);
    gb_write(gb, wLinkMotionState, LINK_MOTION_DEFAULT);
}

void LinkMotionRevolvingDoorHandler(GBState *gb) {
    if (!gb) return;

    gb_write_hram(gb, hLinkPositionY, 0x10);
    gb_write_hram(gb, hLinkPositionX, 0x50);
    gb_write(gb, wC167, 0x50);

    uint8_t frame = gb_read(gb, wLinkAnimationFrame);
    uint8_t de_offset = 0x08;

    if (frame == 0x18) {
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4E2C(gb, de_offset);
        }
    } else if (frame == 0x28) {
        de_offset = 0x00;
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4E2C(gb, de_offset);
        }
    } else if (frame == 0) {
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4DFC(gb);
        }
    }

    uint8_t idx = (frame >> 3) & 0x07;
    gb_write_hram(gb, hLinkAnimationState, LinkRevolvingDoorAnimation[idx]);

    frame++;
    gb_write(gb, wLinkAnimationFrame, frame);

    if (frame >= 0x38) {
        gb_write_hram(gb, hLinkPositionY, 0xFB);
        gb_write_hram(gb, hLinkDirection, DIRECTION_UP);
        gb_write_hram(gb, hLinkAnimationState, LINK_ANIMATION_STATE_STANDING_UP);
    }

    if (frame != 0x48) {
        return;
    }

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_002_4E48(gb);
    }

    gb_write(gb, wRoomTransitionDirection, 0x02);
    gb_write(gb, wRoomTransitionState, 0x01);
    func_002_4EDD(gb);
}

/* Bank 2 Swimming Velocity Tables */
const int8_t Data_002_4EF0[16] = {
    0x00, 0x08, -0x08, 0x00, 0x00, 0x06, -0x06, 0x00,
    0x00, 0x06, -0x06, 0x00, 0x00, 0x00,  0x00, 0x00
};

const int8_t Data_002_4F00[16] = {
    0x00, 0x10, -0x10, 0x00, 0x00, 0x0C, -0x0C, 0x00,
    0x00, 0x0C, -0x0C, 0x00, 0x00, 0x00,  0x00, 0x00
};

const int8_t Data_002_4F10[16] = {
    0x00, 0x00, 0x00, 0x00, -0x08, -0x06, -0x06, 0x00,
    0x08, 0x06, 0x06, 0x00,  0x00,  0x00,  0x00, 0x00
};

const int8_t Data_002_4F20[16] = {
    0x00, 0x00, 0x00, 0x00, -0x10, -0x0C, -0x0C, 0x00,
    0x10, 0x0C, 0x0C, 0x00,  0x00,  0x00,  0x00, 0x00
};

void func_002_5928(GBState *gb, uint8_t y) {
    if (!gb) return;

    gb_write_hram(gb, hMultiPurpose1, y);
    gb_write_hram(gb, hMultiPurpose0, gb_read_hram(gb, hLinkPositionX));
    gb_write_hram(gb, hJingle, JINGLE_WATER_SPLASH);
    AddTranscientVfx(gb, TRANSCIENT_VFX_WATER_SPLASH);
}

void LinkMotionSwimmingHandler(GBState *gb,
                               void (*check_map_transition)(GBState *),
                               void (*func_753a)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                               void (*splash_vfx)(GBState *, uint8_t)) {
    if (!gb) return;

    if (gb_read(gb, wFreeMovementMode) != 0) {
        gb_write(gb, wLinkMotionState, LINK_MOTION_DEFAULT);
        return;
    }

    ResetSpinAttack(gb);
    gb_write_hram(gb, hLinkPositionZ, 0);
    gb_write(gb, wIsLinkInTheAir, 0);
    gb_write(gb, wLinkAttackStepAnimationCountdown, 0);
    gb_write(gb, wSwordAnimationState, 0);
    gb_write(gb, wC16A, 0);
    gb_write(gb, wC16D, 0);

    gb_write(gb, wD46B, gb_read(gb, wIndoorRoom));

    if ((gb_read_hram(gb, hLinkInteractiveMotionBlocked) | gb_read(gb, wDialogState)) != 0) {
        ClearLinkPositionIncrement(gb);
        if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) != 0) {
            gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);
        }
        UpdateLinkWalkingAnimation(gb);
        return;
    }

    /* label_002_4F6D: Check B button for diving */
    uint8_t joypad = gb_read_hram(gb, hJoypadState);
    if ((joypad & J_B) != 0) {
        uint8_t phys = (uint8_t)(gb_read_hram(gb, hLinkPhysicsModifier) ^ 0x01);
        gb_write_hram(gb, hLinkPhysicsModifier, phys);
        if (phys != 0) {
            gb_write_hram(gb, hLinkCountdown, 0xA0);
            uint8_t py = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 3);
            if (splash_vfx) {
                splash_vfx(gb, py);
            } else {
                func_002_5928(gb, py);
            }
        }
    }

    /* Check A button for swim stroke speed boost */
    uint8_t c183 = gb_read(gb, wC183);
    if (c183 != 0) {
        gb_write(gb, wC183, (uint8_t)(c183 - 1));
    } else if ((joypad & J_A) != 0) {
        gb_write_hram(gb, hJingle, JINGLE_SWIM);
        gb_write(gb, wC183, 0x20);
    }

    /* jr_002_4FA1: Velocity adjustments every even frame */
    if ((gb_read_hram(gb, hFrameCounter) & 0x01) == 0) {
        uint8_t btn = (uint8_t)(gb_read_hram(gb, hPressedButtonsMask) & 0x0F);
        uint8_t speed_idx = gb_read(gb, wC183);

        const int8_t *x_table = (speed_idx < 0x10) ? Data_002_4EF0 : Data_002_4F00;
        int8_t target_x = x_table[btn];
        int8_t cur_speed_x = (int8_t)gb_read_hram(gb, hLinkSpeedX);
        uint8_t diff_x = (uint8_t)((uint8_t)target_x - (uint8_t)cur_speed_x);

        if (diff_x != 0) {
            cur_speed_x++;
            if ((diff_x & 0x80) != 0) {
                cur_speed_x -= 2;
            }
            gb_write_hram(gb, hLinkSpeedX, (uint8_t)cur_speed_x);
        }

        const int8_t *y_table = (speed_idx < 0x10) ? Data_002_4F10 : Data_002_4F20;
        int8_t target_y = y_table[btn];
        int8_t cur_speed_y = (int8_t)gb_read_hram(gb, hLinkSpeedY);
        uint8_t diff_y = (uint8_t)((uint8_t)target_y - (uint8_t)cur_speed_y);

        if (diff_y != 0) {
            cur_speed_y++;
            if ((diff_y & 0x80) != 0) {
                cur_speed_y -= 2;
            }
            gb_write_hram(gb, hLinkSpeedY, (uint8_t)cur_speed_y);
        }

        uint8_t moving = gb_read_hram(gb, hLinkSpeedX) | gb_read_hram(gb, hLinkSpeedY) | gb_read_hram(gb, hLinkPhysicsModifier);
        if (moving != 0) {
            gb_write(gb, wConsecutiveStepsCount, (uint8_t)(gb_read(gb, wConsecutiveStepsCount) + 1));
        } else {
            gb_write(gb, wConsecutiveStepsCount, 0x03);
        }

        uint8_t dir = JoypadToLinkDirection[btn];
        if (dir != 0x0F) {
            gb_write_hram(gb, hLinkDirection, dir);
        }
    }

    UpdateLinkWalkingAnimation(gb);

    if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) != 0) {
        gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0);
    } else {
        func_002_44AD(gb, check_map_transition);
    }

    if (func_753a) {
        func_753a(gb);
    }

    /* Sunk heart piece and underwater cave warp check */
    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0) {
        return;
    }

    if (gb_read_hram(gb, hLinkCountdown) == 0) {
        gb_write_hram(gb, hLinkPhysicsModifier, 0);
    }

    uint8_t map_id = gb_read_hram(gb, hMapId);
    uint8_t map_room = gb_read_hram(gb, hMapRoom);

    if (map_id == 0) {
        if (map_room != ROOM_OW_KANALET_MOAT_HEARTPIECE) {
            return;
        }
    } else if (map_id == MAP_CAVE_WATER) {
        if (map_room == ROOM_INDOOR_A_WATER_FLOODED_GROTTO) {
            /* Flooded grotto sunk piece of heart */
        } else if (map_room == UNKNOWN_ROOM_8D) {
            /* Underwater grotto tunnel warp */
            uint8_t dy = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x50 + 0x08);
            if (dy >= 0x10) return;
            uint8_t dx = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x58 + 0x08);
            if (dx >= 0x10) return;

            gb_write(gb, wWarp0DestinationX, gb_read_hram(gb, hLinkPositionX));
            gb_write(gb, wD463, gb_read(gb, wLinkMotionState));
            ApplyMapFadeOutTransitionWithNoise(gb);
            return;
        } else {
            return;
        }
    } else {
        return;
    }

    /* Check coordinates for sunken heart piece (02:503B) */
    uint8_t dy = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x50 + 0x08);
    if (dy >= 0x10) return;
    uint8_t dx = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x58 + 0x08);
    if (dx >= 0x10) return;

    uint8_t status = gb_read_hram(gb, hRoomStatus);
    if ((status & (1 << 5)) != 0) {
        return;
    }
    gb_write_hram(gb, hRoomStatus, (uint8_t)(status | (1 << 5)));

    if (spawn_new_entity) {
        uint16_t de = spawn_new_entity(gb, ENTITY_HEART_PIECE);
        if (de != 0xFFFF) {
            gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), gb_read_hram(gb, hLinkPositionX));
            gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), gb_read_hram(gb, hLinkPositionY));
            gb_write(gb, (uint16_t)(wEntitiesPosZTable + de), 0x03);
            uint16_t phys_addr = (uint16_t)(wEntitiesPhysicsFlagsTable + de);
            gb_write(gb, phys_addr, (uint8_t)(gb_read(gb, phys_addr) & ~(1 << 3)));
        }
    }
}

void LinkMotionUnknownHandler(GBState *gb) {
    if (!gb) return;

    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x01);
    UpdateFinalLinkPosition(gb);
    func_21E1(gb);

    uint8_t px = (uint8_t)(gb_read_hram(gb, hLinkPositionX) & 0xF0);
    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);

    if (px != 0xE0 && pz < 0x78) {
        return;
    }

    ApplyMapFadeOutTransition(gb);
    ClearLinkPositionIncrement(gb);
    gb_write_hram(gb, hLinkPositionZ, 0);
    gb_write_hram(gb, hLinkVelocityZ, 0);
    gb_write(gb, wMapEntrancePositionZ, 0x70);
}

const uint8_t LinkFallingDownAnimation[10] = {
    LINK_ANIMATION_STATE_FALLING_PIT_1,
    LINK_ANIMATION_STATE_FALLING_PIT_2,
    LINK_ANIMATION_STATE_FALLING_PIT_3,
    LINK_ANIMATION_STATE_FALLING_PIT_3,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN,
    LINK_ANIMATION_STATE_HIDDEN
};

void func_002_52D6(GBState *gb) {
    if (!gb) return;

    if (gb_read_hram(gb, hStaircase) != 0) {
        gb_write_hram(gb, hStaircase, STAIRCASE_INACTIVE);
    }
}

void label_002_52B9(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wInvincibilityCounter, 0x40);

    uint8_t entry_x = gb_read(gb, wLinkMapEntryPositionX);
    gb_write_hram(gb, hLinkPositionX, entry_x);
    gb_write_hram(gb, hLinkFinalPositionX, entry_x);

    uint8_t entry_y = gb_read(gb, wLinkMapEntryPositionY);
    gb_write_hram(gb, hLinkPositionY, entry_y);
    gb_write_hram(gb, hLinkFinalPositionY, entry_y);

    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);
    gb_write(gb, wC145, (uint8_t)(entry_y - pz));

    func_002_4EDD(gb);
}

void LinkMotionFallingDownHandler(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wC167, 0x01);
    uint8_t anim_frame = (uint8_t)(gb_read(gb, wLinkAnimationFrame) + 1);
    gb_write(gb, wLinkAnimationFrame, anim_frame);

    uint8_t index = (anim_frame >> 4) & 0x0F;
    if (index != 0x06) {
        if (index < sizeof(LinkFallingDownAnimation)) {
            gb_write_hram(gb, hLinkAnimationState, LinkFallingDownAnimation[index]);
        }
        return;
    }

    /* reset sword parameter */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x00);
    gb_write(gb, wIsUsingSpinAttack, 0x00);
    gb_write(gb, wSwordCharge, 0x00);
    func_002_52D6(gb);

    if (gb_read(gb, wIsIndoor) == 0) {
        uint8_t room = gb_read_hram(gb, hMapRoom);
        if (room == ROOM_OW_TURTLE_ROCK_WARP_HOLE ||
            room == ROOM_OW_UKUKU_PRAIRIE_WARP_HOLE ||
            room == ROOM_OW_WATERFALL_WARP_HOLE ||
            room == ROOM_OW_ANIMAL_VILLAGE_WARP_HOLE) {
            gb_write(gb, wLinkMotionState, LINK_MOTION_TELEPORT);
            gb_write_hram(gb, hLinkCountdown, 0x40);
            gb_write_hram(gb, hLinkPhysicsModifier, 0x00);
            gb_write_hram(gb, hLinkAnimationState, 0xFF);
            return;
        }
    }

    uint8_t phys = gb_read(gb, wLinkFallingDownObjectPhysics);
    if (phys != OBJ_PHYSICS_PIT) {
        if (phys != OBJ_PHYSICS_TRACTOR_DEVICE) {
            if (gb_read(gb, wWarp0MapCategory) == 0x02) {
                gb_write(gb, wWarp0DestinationX, gb_read_hram(gb, hLinkPositionY));
                gb_write(gb, wMapEntrancePositionZ, 0x00);
                ClearLinkPositionIncrement(gb);
                gb_write_hram(gb, hLinkVelocityZ, 0x00);
                gb_write(gb, wIsLinkInTheAir, 0x00);
                ApplyMapFadeOutTransition(gb);
                return;
            }

            uint8_t dest_x = (uint8_t)((gb_read_hram(gb, hLinkPositionX) & 0xF0) + 0x08);
            uint8_t dest_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) & 0xF0);
            gb_write(gb, wWarp0DestinationX, dest_x);
            gb_write(gb, wWarp0DestinationY, dest_y);
        }

        gb_write(gb, wD475, 0x01);
        gb_write(gb, wMapEntrancePositionZ, 0x70);
        ClearLinkPositionIncrement(gb);
        gb_write_hram(gb, hLinkVelocityZ, 0x00);
        gb_write(gb, wIsLinkInTheAir, 0x00);
        ApplyMapFadeOutTransition(gb);
        return;
    }

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_1E) {
            gb_write(gb, wD475, 0x01);
            gb_write(gb, wMapEntrancePositionZ, 0x70);
            ClearLinkPositionIncrement(gb);
            gb_write_hram(gb, hLinkVelocityZ, 0x00);
            gb_write(gb, wIsLinkInTheAir, 0x00);
            ApplyMapFadeOutTransition(gb);
            return;
        }
    }

    if (gb_read_hram(gb, hMapId) == MAP_CAVE_B) {
        uint8_t room = gb_read_hram(gb, hMapRoom);
        if (room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_1 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_2 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_3 ||
            room == ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_4) {
            gb_write(gb, wWarp0MapCategory, 0x00);
            gb_write(gb, wWarp0Map, 0x00);
            gb_write(gb, wWarp0Room, 0x1A);
            gb_write(gb, wWarp0DestinationX, 0x68);
            gb_write(gb, wWarp0DestinationY, 0x56);
            gb_write(gb, wMapEntrancePositionZ, 0x24);
            gb_write_hram(gb, hLinkDirection, DIRECTION_DOWN);
            ApplyMapFadeOutTransition(gb);
            return;
        }
    }

    label_002_52B9(gb);
    gb_write(gb, wSubtractHealthBuffer, (uint8_t)(gb_read(gb, wSubtractHealthBuffer) + 0x04));
    gb_write(gb, wC167, 0x00);
}

uint8_t func_002_523A(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x14;
}

uint8_t func_002_523F(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x14;
}

uint8_t func_002_524A(GBState *gb, uint16_t bc, uint8_t e) {
    if (gb) gb_write(gb, bc, e);
    return 0x10;
}

void HandleGotItemB(GBState *gb,
                    void (*apply_motion_state)(GBState *),
                    void (*func_020_4ab3)(GBState *)) {
    if (!gb) return;

    ResetSpinAttack(gb);
    gb_write(gb, wC16A, 0x00);
    gb_write(gb, wSwordAnimationState, 0x00);
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x00);

    if (apply_motion_state) {
        apply_motion_state(gb);
    } else {
        ApplyLinkMotionState(gb, NULL, NULL, NULL);
    }

    func_21E1(gb);

    int8_t vz = (int8_t)(gb_read_hram(gb, hLinkVelocityZ) - 2);
    gb_write_hram(gb, hLinkVelocityZ, (uint8_t)vz);

    uint8_t pz = gb_read_hram(gb, hLinkPositionZ);
    if ((pz & 0x80) != 0) {
        gb_write_hram(gb, hLinkPositionZ, 0x00);
        gb_write(gb, wC149, 0x00);
        gb_write_hram(gb, hLinkVelocityZ, 0x00);
    }

    gb_write_hram(gb, hLinkAnimationState, LINK_ANIMATION_STATE_UNKNOWN_6B);

    uint16_t bc = (uint16_t)(wLinkOAMBuffer + 0x10);
    uint8_t py = gb_read_hram(gb, hLinkPositionY);
    pz = gb_read_hram(gb, hLinkPositionZ);
    uint8_t c13b = gb_read(gb, wC13B);
    uint8_t multi0 = (uint8_t)(py - pz + c13b - 0x10);
    gb_write_hram(gb, hMultiPurpose0, multi0);

    uint8_t got_item = gb_read(gb, wDialogGotItem);
    if (got_item == DIALOG_GOT_PIECE_OF_POWER) {
        gb_write_hram(gb, hMultiPurpose1, (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 0x08));
        uint8_t fc = gb_read_hram(gb, hFrameCounter);
        gb_write_hram(gb, hMultiPurpose3, (uint8_t)((fc << 2) & 0x10));
        gb_write_hram(gb, hMultiPurpose2, 0x06);
        func_1819(gb, func_020_4ab3);
        return;
    }

    gb_write(gb, bc++, (uint8_t)(multi0 + 0x02));
    gb_write(gb, bc++, gb_read_hram(gb, hLinkPositionX));

    uint8_t tile;
    uint8_t attr;
    if (got_item == DIALOG_GOT_GUARDIAN_ACORN) {
        tile = 0xAE;
        attr = func_002_523A(gb, bc, tile);
    } else if (got_item == DIALOG_GOT_MAGIC_POWDER) {
        tile = 0x8E;
        gb_write(gb, bc, tile);
        attr = 0x16;
    } else if (got_item == DIALOG_GOT_ROD) {
        tile = 0x8C;
        attr = func_002_524A(gb, bc, tile);
    } else {
        tile = 0x8E;
        attr = func_002_523F(gb, bc, tile);
    }

    bc++;
    gb_write(gb, bc, attr);
}

void HandleGotItemA(GBState *gb,
                    void (*apply_motion_state)(GBState *),
                    void (*func_020_4ab3)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, wDialogGotItemCountdown) == 0x2E) {
        gb_write_hram(gb, hJingle, JINGLE_GOT_POWER_UP);
    }

    HandleGotItemB(gb, apply_motion_state, func_020_4ab3);
}

void LinkMotionRecoverHandler(GBState *gb) {
    if (!gb) return;

    ResetSpinAttack(gb);
    ClearLinkPositionIncrement(gb);

    uint8_t countdown = gb_read_hram(gb, hLinkCountdown);
    if (countdown != 0) {
        uint8_t anim;
        if (countdown < 0x30) {
            anim = LINK_ANIMATION_STATE_HIDDEN;
        } else if (countdown < 0x40) {
            anim = LINK_ANIMATION_STATE_HOLD_SWIMMING_2;
        } else {
            if (countdown == 0x40) {
                gb_write_hram(gb, hWaveSfx, NOISE_SFX_SPIN_ATTACK);
            }
            anim = LINK_ANIMATION_STATE_HOLD_SWIMMING_1_DOWN;
        }
        gb_write_hram(gb, hLinkAnimationState, anim);
        return;
    }

    gb_write(gb, wC167, 0x00);

    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0x06) {
        gb_write(gb, wSubtractHealthBuffer, (uint8_t)(gb_read(gb, wSubtractHealthBuffer) + 0x04));
    }
    gb_write_hram(gb, hLinkPhysicsModifier, 0x00);

    if (gb_read(gb, wIsIndoor) == 0) {
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_ANGLERS_TUNNEL_ENTRANCE) {
            gb_write(gb, wLinkMapEntryPositionX, 0x48);
            gb_write(gb, wLinkMapEntryPositionY, 0x30);
        }
    }

    label_002_52B9(gb);
}

/* Bank 2 Magic Rod Visuals & Attack Sprites (02:52E0-02:53AF) */

const int8_t LinkDirectionToMagicRodXOffset[8] = {
    0x0D, (int8_t)0xF3, 0x00, (int8_t)0xFF,
    0x08, (int8_t)0xF8, 0x0C, (int8_t)0xF5
};

const int8_t LinkDirectionToMagicRodYOffset[8] = {
    0x00, 0x00, (int8_t)0xF3, 0x0E,
    (int8_t)0xF3, (int8_t)0xF3, (int8_t)0xFC, 0x00
};

const uint8_t LinkDirectionToMagicRodTiles[16] = {
    0x06, 0x08, 0x08, 0x06, 0x04, 0xFF, 0xFF, 0x04,
    0x04, 0xFF, 0xFF, 0x04, 0x06, 0x08, 0x08, 0x06
};

const uint8_t LinkDirectionToMagicRodOAMAttributes[16] = {
    0x02, 0x02, 0x22, 0x22, 0x22, 0x02, 0x02, 0x42,
    0x22, 0x02, 0x02, 0x22, 0x02, 0x02, 0x22, 0x22
};

const int8_t LinkDirectionToEntitiesPositionX[4] = {
    4, -4, -4, 4
};

const int8_t LinkDirectionToEntitiesPositionY[4] = {
    4, 4, -4, 4
};

static const uint8_t data_13AD[8] = {
    0x30, 0xD0, 0x00, 0x00,
    0x40, 0xC0, 0x00, 0x00
};

static const uint8_t data_13B5[8] = {
    0x00, 0x00, 0xD0, 0x30,
    0x00, 0x00, 0xC0, 0x40
};

void label_002_5310(GBState *gb) {
    if (!gb) return;

    uint8_t countdown = gb_read(gb, wLinkAttackStepAnimationCountdown) & ATTACK_STEP_DURATION_MASK;
    uint8_t dir = gb_read_hram(gb, hLinkDirection);
    if (countdown >= 0x08) {
        dir += 0x04;
    }
    uint8_t offset = dir & 0x07;

    uint8_t y_offset = (uint8_t)LinkDirectionToMagicRodYOffset[offset];
    gb_write_hram(gb, hMultiPurpose0, y_offset);

    uint8_t x_offset = (uint8_t)LinkDirectionToMagicRodXOffset[offset];
    gb_write_hram(gb, hMultiPurpose1, x_offset);

    uint8_t tile0 = LinkDirectionToMagicRodTiles[offset * 2];
    gb_write_hram(gb, hMultiPurpose2, tile0);

    uint8_t tile1 = LinkDirectionToMagicRodTiles[offset * 2 + 1];
    gb_write_hram(gb, hMultiPurpose3, tile1);

    uint8_t attr0 = LinkDirectionToMagicRodOAMAttributes[offset * 2];
    gb_write_hram(gb, hMultiPurpose4, attr0);

    uint8_t attr1 = LinkDirectionToMagicRodOAMAttributes[offset * 2 + 1];
    gb_write_hram(gb, hMultiPurpose5, attr1);

    uint16_t de = wLinkOAMBuffer + 0x10;
    uint16_t bc = wLinkOAMBuffer + 0x14;

    uint8_t final_y = (uint8_t)(gb_read(gb, wC145) + gb_read(gb, wC13B) + y_offset);
    gb_write_hram(gb, hMultiPurpose0, final_y);

    if (tile0 != 0xFF) {
        gb_write(gb, de, final_y);
    }
    if (tile1 != 0xFF) {
        gb_write(gb, bc, final_y);
    }

    de++;
    bc++;

    uint8_t pos_x = (uint8_t)(x_offset + gb_read_hram(gb, hLinkPositionX));
    gb_write(gb, de, pos_x);
    gb_write(gb, bc, (uint8_t)(pos_x + 0x08));

    de++;
    bc++;

    gb_write(gb, de, tile0);
    gb_write(gb, bc, tile1);

    de++;
    bc++;

    gb_write(gb, de, attr0);
    gb_write(gb, bc, attr1);
}

void label_002_538B_entity(GBState *gb, uint8_t de) {
    if (!gb) return;

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    uint8_t pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + (uint8_t)LinkDirectionToEntitiesPositionX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), pos_x);

    uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + (uint8_t)LinkDirectionToEntitiesPositionY[dir]);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), pos_y);

    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), 0x00);

    /* label_140F */
    uint8_t offset = dir;
    if (gb_read(gb, wActivePowerUp) == POWER_UP_PIECE_OF_POWER) {
        offset += 4;
    }
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + de), data_13AD[offset]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + de), data_13B5[offset]);
}

void label_002_538B(GBState *gb) {
    label_002_538B_entity(gb, 0);
}

/* Bank 2 Debug Warp Tables (02:5471+) */
const uint8_t DebugWarpRooms[11] = {
    0x30, /* MAP_BOTTLE_GROTTO */
    0x33, /* MAP_BOTTLE_GROTTO */
    0x81, /* MAP_CATFISHS_MAW */
    0x01, /* MAP_TAIL_CAVE */
    0x28, /* MAP_BOTTLE_GROTTO */
    0x56, /* MAP_KEY_CAVERN */
    0x68, /* MAP_ANGLERS_TUNNEL */
    0x87, /* MAP_CATFISHS_MAW */
    0xB3, /* MAP_FACE_SHRINE */
    0xE6, /* MAP_EAGLES_TOWER */
    0x0A  /* MAP_TURTLE_ROCK */
};

const uint8_t DebugWarpMaps[11] = {
    MAP_BOTTLE_GROTTO,
    MAP_BOTTLE_GROTTO,
    MAP_CATFISHS_MAW,
    MAP_TAIL_CAVE,
    MAP_BOTTLE_GROTTO,
    MAP_KEY_CAVERN,
    MAP_ANGLERS_TUNNEL,
    MAP_CATFISHS_MAW,
    MAP_FACE_SHRINE,
    MAP_EAGLES_TOWER,
    MAP_TURTLE_ROCK
};

uint16_t GetRoomStatusAddress(GBState *gb) {
    if (!gb) return 0;

    uint8_t room = gb_read_hram(gb, hMapRoom);
    if (gb_read(gb, wIsIndoor) == 0) {
        return (uint16_t)(wOverworldRoomStatus + room);
    }

    uint8_t map_id = gb_read_hram(gb, hMapId);
    if (map_id == MAP_COLOR_DUNGEON) {
        return (uint16_t)(wColorDungeonRoomStatus + room);
    }

    uint8_t d = gb_read(gb, wIsIndoor);
    if (map_id >= MAP_INDOORS_B_START && map_id < MAP_INDOORS_B_END) {
        d++;
    }

    return (uint16_t)(wOverworldRoomStatus + ((uint16_t)d << 8) + room);
}

void EnqueueDoorUnlockedSfx(GBState *gb) {
    if (!gb) return;
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_DOOR_UNLOCKED);
}

void label_002_5425(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    uint8_t map_id = gb_read_hram(gb, hMapId);
    uint8_t entity_type;
    if (map_id == MAP_COLOR_DUNGEON) {
        entity_type = ENTITY_KEY_DROP_POINT;
    } else if (map_id < MAP_CAVE_B) {
        entity_type = ENTITY_KEY_DROP_POINT;
    } else {
        entity_type = ENTITY_HIDING_SLIME_KEY;
    }

    uint16_t slot = SpawnNewEntity_trampoline(gb, entity_type, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    uint8_t pos_x = 0x28;
    if (map_id == MAP_COLOR_DUNGEON) {
        pos_x = 0x48;
        if (gb_read_hram(gb, hMapRoom) == ROOM_OW_MARIN_BRIDGE) {
            pos_x = 0x58;
        }
    }
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), pos_x);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), 0x3C);
    gb_write(gb, (uint16_t)(wEntitiesPosZTable + slot), 0x70);
}

void TryOpenKeyDoor(GBState *gb,
                    void (*sync_item_flags)(GBState *),
                    void (*reveal_object)(GBState *),
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    if (gb_read_hram(gb, hMultiPurposeG) == 0x40) {
        /* .spawnPushedBlock */
        uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_PUSHED_BLOCK, spawn_new_entity);
        if (slot == 0xFFFF) {
            return;
        }

        uint8_t status = (uint8_t)(gb_read(gb, (uint16_t)(wEntitiesStatusTable + slot)) - 1);
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + slot), status);

        uint8_t pos_x = (uint8_t)((gb_read_hram(gb, hMultiPurpose4) & 0xF0) + 0x08);
        gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), pos_x);

        uint8_t pos_y = (uint8_t)((gb_read_hram(gb, hMultiPurpose5) & 0xF0) + 0x10);
        gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), pos_y);
        return;
    }

    uint8_t keys = gb_read(gb, wSmallKeysCount);
    if (keys == 0) {
        return;
    }

    keys--;
    gb_write(gb, wSmallKeysCount, keys);
    SynchronizeDungeonsItemFlags_trampoline(gb, sync_item_flags ? sync_item_flags : SynchronizeDungeonsItemFlags);
    EnqueueDoorUnlockedSfx(gb);

    uint16_t room_status_addr = GetRoomStatusAddress(gb);
    uint8_t status = (uint8_t)(gb_read(gb, room_status_addr) | ROOM_STATUS_EVENT_3);
    gb_write(gb, room_status_addr, status);
    gb_write_hram(gb, hRoomStatus, status);

    uint8_t left = (uint8_t)(gb_read_hram(gb, hMultiPurpose4) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectLeft, left);

    uint8_t top = (uint8_t)(gb_read_hram(gb, hMultiPurpose5) & 0xF0);
    gb_write_hram(gb, hIntersectedObjectTop, top);

    RevealObjectUnderObject_trampoline(gb, reveal_object);

    gb_write_hram(gb, hMultiPurpose0, (uint8_t)(left + 0x08));
    gb_write_hram(gb, hMultiPurpose1, (uint8_t)(top + 0x10));
    AddTranscientVfx(gb, TRANSCIENT_VFX_POOF);
}

