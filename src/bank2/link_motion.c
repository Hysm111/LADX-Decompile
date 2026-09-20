#include "bank2/link_motion.h"
#include "bank2/link_animation.h"
#include "bank2/ocarina.h"
#include "bank2/shovel.h"
#include "bank2/vfx.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/physics.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "home/check_items_to_use.h"
#include "home/link.h"

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

void func_002_7587(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hLinkRoomPosition] */
    /* ldh [hLinkFinalRoomPosition], a */
    uint8_t room_pos = gb_read_hram(gb, hLinkRoomPosition);
    gb_write_hram(gb, hLinkFinalRoomPosition, room_pos);

    /* ld a, [wFreeMovementMode]; and a; ret nz */
    if (gb_read(gb, wFreeMovementMode) != 0) {
        return;
    }

    /* ldh a, [hLinkPositionZ]; and a; jr z, jr_002_75B2 */
    uint8_t pos_z = gb_read_hram(gb, hLinkPositionZ);
    if (pos_z == 0) {
        return;
    }

    /* ldh a, [hFrameCounter]; and $01; jr nz, ret_002_75B1 */
    uint8_t frame = gb_read_hram(gb, hFrameCounter);
    if ((frame & 0x01) != 0) {
        return;
    }

    /* ld hl, wLinkOAMBuffer */
    /* ldh a, [hLinkPositionY]; add $0B; cp $88; jr nc, ret_002_75B1 */
    uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x0B);
    if (pos_y >= 0x88) {
        return;
    }

    /* ld [hl+], a */
    gb_write(gb, wLinkOAMBuffer + 0, pos_y);

    /* ldh a, [hLinkPositionX]; add $04; ld [hl+], a */
    uint8_t pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + 0x04);
    gb_write(gb, wLinkOAMBuffer + 1, pos_x);

    /* ld a, $26; ld [hl+], a; ld [hl], $00 */
    gb_write(gb, wLinkOAMBuffer + 2, 0x26);
    gb_write(gb, wLinkOAMBuffer + 3, 0x00);
}

void func_002_75B2(GBState *gb) {
    if (!gb) return;

    /* xor a; ld [wD475], a */
    gb_write(gb, wD475, 0);

    /* ld a, [wLinkMotionState]; cp LINK_MOTION_UNSTUCKING; jr z, ret_002_75B1 */
    if (gb_read(gb, wLinkMotionState) == LINK_MOTION_UNSTUCKING) {
        return;
    }

    /* Fall through to ApplyLinkGroundPhysics */
    ApplyLinkGroundPhysics(gb);
}

void HurtBySpikes(GBState *gb) {
    if (!gb) return;

    /* ld a, [wInvincibilityCounter]; and a; jr nz, .return */
    if (gb_read(gb, wInvincibilityCounter) != 0) {
        return;
    }

    /* call ResetSpinAttack */
    ResetSpinAttack(gb);

    /* Invert Link speed X */
    uint8_t speed_x = gb_read_hram(gb, hLinkSpeedX);
    speed_x = (uint8_t)(~speed_x + 1);
    gb_write_hram(gb, hLinkSpeedX, speed_x);

    /* Invert Link speed Y */
    uint8_t speed_y = gb_read_hram(gb, hLinkSpeedY);
    speed_y = (uint8_t)(~speed_y + 1);
    gb_write_hram(gb, hLinkSpeedY, speed_y);

    /* Mark Link as in the air */
    gb_write(gb, wIsLinkInTheAir, 0x02);

    /* If in top-view... */
    if (gb_read_hram(gb, hIsSideScrolling) == 0) {
        /* ... move Link slightly above the ground */
        gb_write_hram(gb, hLinkVelocityZ, 0x10);
        uint8_t pos_z = (uint8_t)(gb_read_hram(gb, hLinkPositionZ) + 0x02);
        gb_write_hram(gb, hLinkPositionZ, pos_z);
    }

    /* Make Link invincible for 48 frames */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x10);
    gb_write(gb, wInvincibilityCounter, 0x30);

    /* Lose one full heart (4 half-hearts) */
    uint8_t health_sub = (uint8_t)(gb_read(gb, wSubtractHealthBuffer) + 0x04);
    gb_write(gb, wSubtractHealthBuffer, health_sub);

    /* Play the "hurt" sfx */
    gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_HURT);
}

void ApplyLinkGroundPhysics_part2(GBState *gb) {
    if (!gb) return;

    /* ld a, [wLinkObjectPhysics]; cp OBJ_PHYSICS_TRACTOR_DEVICE; jp z, ApplyLinkGroundPhysics_Default */
    uint8_t obj_physics = gb_read(gb, wLinkObjectPhysics);
    if (obj_physics == OBJ_PHYSICS_TRACTOR_DEVICE) {
        ApplyLinkGroundPhysics_Default(gb);
        return;
    }

    /* cp OBJ_PHYSICS_CONVEYOR; jr c, .jr_002_7644; jp label_002_7C14 */
    if (obj_physics >= OBJ_PHYSICS_CONVEYOR) {
        /* label_002_7C14 is not yet implemented - callback modeled */
        return;
    }

    /* cp OBJ_PHYSICS_PIT_WARP; jr z, .slipIntoPit */
    if (obj_physics == OBJ_PHYSICS_PIT_WARP) {
        goto slipIntoPit;
    }

    /* cp OBJ_PHYSICS_PIT; jr nz, label_002_76C0 */
    if (obj_physics == OBJ_PHYSICS_PIT) {
        goto slipIntoPit;
    }

    /* label_002_76C0 is handled separately */
    label_002_76C0(gb);
    return;

slipIntoPit:
    /* call ResetSpinAttack */
    ResetSpinAttack(gb);

    /* ld a, GROUND_STATUS_PIT; ld [wLinkGroundStatus], a */
    gb_write(gb, wLinkGroundStatus, GROUND_STATUS_PIT);

    /* ld hl, wPitSlippingCounter; inc [hl] */
    uint8_t pit_slip = (uint8_t)(gb_read(gb, wPitSlippingCounter) + 1);
    gb_write(gb, wPitSlippingCounter, pit_slip);

    /* If in free-movement debug mode, or not every 4th frame, return. */
    uint8_t frame = gb_read_hram(gb, hFrameCounter);
    if (gb_read(gb, wFreeMovementMode) != 0 || (frame & 0x03) != 0) {
        return;
    }

    /* Adjust Link X position (using content of hMultiPurpose1) */
    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    uint8_t pos_x = gb_read_hram(gb, hLinkPositionX);
    uint8_t target_x = (uint8_t)(pos_x - 0x08);
    uint8_t diff_x = (uint8_t)(target_x - mp0);
    if ((diff_x & 0x80) == 0) {
        target_x = (uint8_t)(target_x - 1);
    } else {
        target_x = (uint8_t)(target_x + 1);
    }
    gb_write_hram(gb, hLinkPositionX, target_x);

    /* Adjust Link Y position (using content of hMultiPurpose1) */
    uint8_t mp1 = gb_read_hram(gb, hMultiPurpose1);
    uint8_t pos_y = gb_read_hram(gb, hLinkPositionY);
    uint8_t target_y = (uint8_t)(pos_y - (mp1 + 0x10));
    if ((target_y & 0x80) != 0) {
        target_y = (uint8_t)(target_y + 1);
    } else {
        target_y = (uint8_t)(target_y - 1);
    }
    gb_write_hram(gb, hLinkPositionY, target_y);

    /* If Link is close from pit center (?), make it fall down */
    pos_x = gb_read_hram(gb, hLinkPositionX);
    pos_y = gb_read_hram(gb, hLinkPositionY);

    uint8_t rel_x = (uint8_t)((pos_x - 0x08) + 0x02) & 0x0F;
    if (rel_x >= 0x04) {
        return;
    }

    uint8_t rel_y = (uint8_t)((pos_y - 0x10) + 0x02) & 0x0F;
    if (rel_y >= 0x04) {
        return;
    }

    /* Make Link fall down the pit */
    gb_write_hram(gb, hLinkPositionY, (uint8_t)(pos_y + 0x03));
    gb_write(gb, wLinkMotionState, LINK_MOTION_FALLING_DOWN);
    ResetSpinAttack(gb);
    gb_write(gb, wLinkAnimationFrame, 0);
    gb_write(gb, wLinkFallingDownObjectPhysics, obj_physics);
    gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_FALL);
}

void ApplyLinkGroundPhysics(GBState *gb) {
    if (!gb) return;

    /* ld a, [wRoomTransitionState]; ld hl, wDialogState; or [hl]; jp nz, label_002_76C0 */
    if (gb_read(gb, wRoomTransitionState) != 0 || gb_read(gb, wDialogState) != 0) {
        label_002_76C0(gb);
        return;
    }

    /* Store id of the object under Link's feet into a */
    uint8_t obj_under = GetObjectUnderLink(gb);

    /* If over an overworld well, fall immediately into it */
    uint8_t is_indoor = gb_read(gb, wIsIndoor);
    if (is_indoor == 0) {
        if (obj_under == OBJECT_WELL) {
            /* ApplyLinkGroundPhysics_part2.makeLinkFallInPit */
            ApplyLinkGroundPhysics_part2(gb);
            return;
        }
    } else {
        /* If over side-view spikes, hurt the player immediately */
        if (obj_under == OBJECT_SIDE_VIEW_SPIKES) {
            uint8_t pos_y = gb_read_hram(gb, hLinkPositionY);
            pos_y = (uint8_t)((pos_y - 1) & 0x0F);
            if (pos_y < 0x0C) {
                HurtBySpikes(gb);
            }
        }
    }

    /* call GetObjectPhysicsFlags_trampoline; ld [wLinkObjectPhysics], a */
    /* GetObjectPhysicsFlags_trampoline is not yet implemented - using callback model */
    /* For now, we read the already-set wLinkObjectPhysics */
    uint8_t obj_physics = gb_read(gb, wLinkObjectPhysics);
    if (obj_physics == 0) {
        ApplyLinkGroundPhysics_Default(gb);
        return;
    }

    /* cp OBJ_PHYSICS_SPIKES; jr nz, ApplyLinkGroundPhysics_part2 */
    if (obj_physics == OBJ_PHYSICS_SPIKES) {
        HurtBySpikes(gb);
        return;
    }

    /* Fall through to part2 */
    ApplyLinkGroundPhysics_part2(gb);
}

void label_002_76C0(GBState *gb) {
    if (!gb) return;

    uint8_t obj_physics = gb_read(gb, wLinkObjectPhysics);

    /* cp OBJ_PHYSICS_RAISED; jr nz, .jr_76D5 */
    if (obj_physics == OBJ_PHYSICS_RAISED) {
        /* ld a, [wC13B]; add $FD; ld [wC13B], a; jp ApplyLinkGroundPhysics_Default */
        uint8_t c13b = (uint8_t)(gb_read(gb, wC13B) + 0xFD);
        gb_write(gb, wC13B, c13b);
        ApplyLinkGroundPhysics_Default(gb);
        return;
    }

    /* .jr_76D5: cp OBJ_PHYSICS_LOWERED; jr nz, .jr_76E4 */
    if (obj_physics == OBJ_PHYSICS_LOWERED) {
        /* ld a, [wC13B]; add $02; ld [wC13B], a; jp ApplyLinkGroundPhysics_Default */
        uint8_t c13b = (uint8_t)(gb_read(gb, wC13B) + 0x02);
        gb_write(gb, wC13B, c13b);
        ApplyLinkGroundPhysics_Default(gb);
        return;
    }

    /* .jr_76E4: cp OBJ_PHYSICS_LAVA; jr z, .jr_76EC */
    /* cp OBJ_PHYSICS_DEEP_WATER; jr nz, jr_002_7750 */
    if (obj_physics == OBJ_PHYSICS_LAVA || obj_physics == OBJ_PHYSICS_DEEP_WATER) {
        /* .jr_76EC: ldh a, [hLinkSlowWalkingSpeed]; and a; jr z, .jr_76F4; jp label_002_7C50 */
        if (gb_read_hram(gb, hLinkSlowWalkingSpeed) != 0) {
            /* label_002_7C50 not implemented */
            return;
        }

        /* .jr_76F4: ld a, [wItemUsageContext]; cp ITEM_USAGE_ON_RAFT; jr z, jr_002_7750 */
        if (gb_read(gb, wItemUsageContext) == ITEM_USAGE_ON_RAFT) {
            goto jr_002_7750;
        }

        /* ld a, [wLinkMotionState]; cp LINK_MOTION_RECOVER; jr z, ret_002_774F */
        /* cp LINK_MOTION_SWIMMING; jr z, ret_002_774F */
        uint8_t motion = gb_read(gb, wLinkMotionState);
        if (motion == LINK_MOTION_RECOVER || motion == LINK_MOTION_SWIMMING) {
            return;
        }

        /* ldh a, [hLinkPositionY]; add $FE; call func_002_5928 */
        uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0xFE);
        /* func_002_5928 generates water splash VFX - stub for now */

        /* ldh a, [hObjectUnderEntity]; cp $06; jr z, label_002_7719 */
        if (gb_read_hram(gb, hObjectUnderEntity) == 0x06) {
            goto label_002_7719;
        }

        /* ld a, [wHasFlippers]; and a; jr nz, jr_002_7732 */
        if (gb_read(gb, wHasFlippers) != 0) {
            goto jr_002_7732;
        }

label_002_7719:
        /* ld a, $50; ldh [hLinkCountdown], a */
        gb_write_hram(gb, hLinkCountdown, 0x50);

        /* ld a, LINK_MOTION_RECOVER; ld [wLinkMotionState], a */
        gb_write(gb, wLinkMotionState, LINK_MOTION_RECOVER);

        /* ldh a, [hObjectUnderEntity]; ldh [hLinkPhysicsModifier], a */
        gb_write_hram(gb, hLinkPhysicsModifier, gb_read_hram(gb, hObjectUnderEntity));
        /* ldh a, [hLinkPositionY]; add $02; ldh [hLinkPositionY], a */
        pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x02);
        gb_write_hram(gb, hLinkPositionY, pos_y);
        /* ld a, $01; ld [wC167], a; ret */
        gb_write(gb, wC167, 0x01);
        return;

jr_002_7732:
        /* ld a, $01; ld [wLinkMotionState], a */
        gb_write(gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        /* xor a; ldh [hLinkPhysicsModifier], a */
        gb_write_hram(gb, hLinkPhysicsModifier, 0);
        /* call ClearLinkPositionIncrement */
        ClearLinkPositionIncrement(gb);
        /* ldh a, [hLinkDirection]; ld e, a; ld d, b; ld hl, Data_002_750A; add hl, de; ld a, [hl]; ldh [hLinkSpeedX], a */
        uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
        /* Data_002_750A and Data_002_750E are direction-based speed tables */
        static const int8_t Data_002_750A[4] = { 0x00, 0x10, (int8_t)0xF0, 0x00 };
        static const int8_t Data_002_750E[4] = { (int8_t)0xF0, 0x00, 0x00, 0x10 };
        gb_write_hram(gb, hLinkSpeedX, (uint8_t)Data_002_750A[dir]);
        gb_write_hram(gb, hLinkSpeedY, (uint8_t)Data_002_750E[dir]);
        return;

ret_002_774F:
        return;

jr_002_7750:
        /* Falls through to ApplyLinkGroundPhysics_Default for GRASS and SHALLOW_WATER */
        ;
    }

    /* cp OBJ_PHYSICS_GRASS; jp z, label_002_787D */
    if (obj_physics == OBJ_PHYSICS_GRASS) {
        label_002_787D(gb);
        return;
    }

    /* cp OBJ_PHYSICS_SHALLOW_WATER; jr nz, ApplyLinkGroundPhysics_Default */
    if (obj_physics == OBJ_PHYSICS_SHALLOW_WATER) {
        /* Write transient vfx position to wLinkOAMBuffer */
        uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x0C);
        gb_write(gb, wLinkOAMBuffer + 0, pos_y);
        uint8_t pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + 0x00);
        gb_write(gb, wLinkOAMBuffer + 1, pos_x);
        gb_write(gb, wLinkOAMBuffer + 2, 0x1C);

        /* GBC palette handling - simplified */
        if (gb_read_hram(gb, hIsGBC) != 0) {
            uint8_t frame = gb_read_hram(gb, hFrameCounter);
            uint8_t attr = (uint8_t)((frame & 0x02) << 3) | (OAMF_PAL0 | OAM_GBC_PAL_3);
            gb_write(gb, wLinkOAMBuffer + 3, attr);
        } else {
            uint8_t frame = gb_read_hram(gb, hFrameCounter);
            uint8_t attr = (uint8_t)(((frame << 1) & 0x10) | OAMF_PAL1);
            gb_write(gb, wLinkOAMBuffer + 3, attr);
        }

        /* Second sprite */
        pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x0C);
        gb_write(gb, wLinkOAMBuffer + 4, pos_y);
        pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + 0x08);
        gb_write(gb, wLinkOAMBuffer + 5, pos_x);
        gb_write(gb, wLinkOAMBuffer + 6, 0x1C);
        uint8_t attr2 = gb_read(gb, wLinkOAMBuffer + 3);
        gb_write(gb, wLinkOAMBuffer + 7, (uint8_t)(attr2 | 0x20));

        gb_write(gb, wLinkGroundStatus, GROUND_STATUS_SLOW);

        /* Play water splash jingle every 16 frames when moving */
        if ((gb_read_hram(gb, hFrameCounter) & 0x0F) == 0) {
            if (gb_read_hram(gb, hPressedButtonsMask) != 0) {
                if (gb_read(gb, wDialogState) == 0) {
                    gb_write_hram(gb, hJingle, JINGLE_WATER_SPLASH);
                }
            }
        }

        /* ld a, [wC13B]; add $02; ld [wC13B], a */
        uint8_t c13b = (uint8_t)(gb_read(gb, wC13B) + 0x02);
        gb_write(gb, wC13B, c13b);
        return;
    }

    /* Default case */
    ApplyLinkGroundPhysics_Default(gb);
}

void ApplyLinkGroundPhysics_Default(GBState *gb) {
    if (!gb) return;

    /* Reset wPitSlippingCounter */
    gb_write(gb, wPitSlippingCounter, 0);

    /* If Link was swimming, mark it as no longer swimming */
    if (gb_read(gb, wLinkMotionState) == LINK_MOTION_SWIMMING) {
        gb_write(gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    }

    uint8_t obj_physics = gb_read(gb, wLinkObjectPhysics);
    uint8_t under = gb_read_hram(gb, hObjectUnderEntity);

    /* cp OBJ_PHYSICS_OCEAN_SWITCH_BLOCK; jr nz, .grassVfxEnd */
    if (obj_physics == OBJ_PHYSICS_OCEAN_SWITCH_BLOCK) {
        /* ldh a, [hObjectUnderEntity]; cp $DB; jr c, .grassVfxEnd; cp $DD; jr nc, .grassVfxEnd */
        if (under >= 0xDB && under < 0xDD) {
            /* sub $DB; ld e, a; ld d, $00; ld hl, SwitchBlocksStateTable; add hl, de */
            uint8_t idx = under - 0xDB;
            static const uint8_t SwitchBlocksStateTable[2] = { 0x00, 0x02 };
            uint8_t table_val = SwitchBlocksStateTable[idx];
            uint8_t switch_state = gb_read(gb, wSwitchBlocksState);
            if ((switch_state ^ table_val) != 0) {
                /* ld a, [wSwitchableObjectAnimationStage]; ld e, a; ld d, $00; ld hl, Data_002_786F; add hl, de */
                uint8_t anim_stage = gb_read(gb, wSwitchableObjectAnimationStage);
                static const int8_t Data_002_786F[12] = { -4, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4, -4 };
                int8_t adj = Data_002_786F[anim_stage & 0x0F];
                uint8_t c13b = (uint8_t)(gb_read(gb, wC13B) + adj);
                gb_write(gb, wC13B, c13b);
                gb_write(gb, wLinkStandingOnSwitchBlock, 0x01);
                return;
            }
        }
    }

grassVfxEnd:
    /* ld a, [wLinkStandingOnSwitchBlock]; and a; jr z, .jr_002_77F7 */
    if (gb_read(gb, wLinkStandingOnSwitchBlock) != 0) {
        /* ld a, NOISE_SFX_FOOTSTEP; ldh [hNoiseSfx], a */
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_FOOTSTEP);
        /* xor a; ld [wLinkStandingOnSwitchBlock], a */
        gb_write(gb, wLinkStandingOnSwitchBlock, 0);
    }

    /* .jr_002_77F7: ld a, [wIsIndoor]; and a; jp z, .return */
    if (gb_read(gb, wIsIndoor) == 0) {
        return;
    }

    /* ld a, [wRoomTransitionState]; and a; jr nz, .return */
    if (gb_read(gb, wRoomTransitionState) != 0) {
        return;
    }

    /* When stepping over a switch button, activate it */
    if (under == OBJECT_SWITCH_BUTTON) {
        if (gb_read(gb, wSwitchButtonPressed) == 0) {
            uint8_t c1ca = (uint8_t)(gb_read(gb, wC1CA) + 1);
            gb_write(gb, wC1CA, c1ca);
            if (c1ca == 0x18) {
                /* Kanalet gate switch */
                gb_write(gb, wSwitchButtonPressed, 0x60);
                gb_write_hram(gb, hWaveSfx, WAVE_SFX_FLOOR_SWITCH);
                gb_write_hram(gb, hReplaceTiles, REPLACE_TILES_BUTTON_PRESSED);

                if (gb_read_hram(gb, hMapRoom) == ROOM_INDOOR_B_KANALET_GATE_SWITCH) {
                    /* set OW_ROOM_STATUS_FLAG_CHANGED, [hl] at wOverworldRoomStatus + ROOM_OW_KANALET_GATE */
                    uint16_t addr = wOverworldRoomStatus + 0x79;
                    uint8_t val = gb_read(gb, addr) | 0x10;
                    gb_write(gb, addr, val);
                }
            }

            /* ld a, [wC13B]; add $FD; ld [wC13B], a; ret */
            uint8_t c13b = (uint8_t)(gb_read(gb, wC13B) + 0xFD);
            gb_write(gb, wC13B, c13b);
            return;
        }
    }

    /* .switchButtonEnd: xor a; ld [wC1CA], a */
    gb_write(gb, wC1CA, 0);

    /* ldh a, [hLinkRoomPosition]; ld hl, hLinkFinalRoomPosition; cp [hl] */
    uint8_t room_pos = gb_read_hram(gb, hLinkRoomPosition);
    uint8_t final_room_pos = gb_read_hram(gb, hLinkFinalRoomPosition);
    uint8_t c1c9 = gb_read(gb, wC1C9);

    if (room_pos == final_room_pos) {
        /* ldh a, [hObjectUnderEntity]; cp $DF; jr nz, .jr_002_786C */
        if (under == 0xDF) {
            /* ldh a, [hLinkInteractiveMotionBlocked]; ld e, a */
            /* ld a, [wDialogGotItem]; ld d, a */
            /* ld a, [wDialogState]; or e; or d; jr nz, .jr_002_786C */
            if (gb_read_hram(gb, hLinkInteractiveMotionBlocked) == 0 &&
                gb_read(gb, wDialogGotItem) == 0 &&
                gb_read(gb, wDialogState) == 0) {
                /* inc [hl]; ld a, [hl]; cp $28; jr c, .return */
                c1c9++;
                gb_write(gb, wC1C9, c1c9);
                if (c1c9 < 0x28) {
                    return;
                }
                /* ld a, NOISE_SFX_RUMBLE2; ldh [hNoiseSfx], a; jp label_002_4D97 */
                gb_write_hram(gb, hNoiseSfx, NOISE_SFX_RUMBLE2);
                /* label_002_4D97 not implemented - callback modeled */
                return;
            }
        }
    }

    /* .jr_002_786C: ld [hl], $00 */
    gb_write(gb, wC1C9, 0);

    return;
}

void label_002_787D(GBState *gb) {
    if (!gb) return;

    /* Write transient vfx position to wLinkOAMBuffer */
    uint8_t pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x08);
    gb_write(gb, wLinkOAMBuffer + 0, pos_y);

    uint8_t pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - 1);
    gb_write(gb, wLinkOAMBuffer + 1, pos_x);

    gb_write(gb, wLinkOAMBuffer + 2, 0x1A);

    /* GBC palette handling */
    uint8_t attr = 0;
    if (gb_read_hram(gb, hIsGBC) != 0) {
        if (gb_read(gb, wIsIndoor) == 0) {
            if (gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_32) {
                attr = OAMF_PAL0 | OAM_GBC_PAL_6;
            }
        }
    }
    gb_write(gb, wLinkOAMBuffer + 3, attr);

    /* Second sprite */
    pos_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) + 0x08);
    gb_write(gb, wLinkOAMBuffer + 4, pos_y);

    pos_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + 0x07);
    gb_write(gb, wLinkOAMBuffer + 5, pos_x);

    gb_write(gb, wLinkOAMBuffer + 6, 0x1A);

    /* GBC palette for second sprite */
    uint8_t attr2 = 0;
    if (gb_read_hram(gb, hIsGBC) != 0) {
        if (gb_read(gb, wIsIndoor) == 0) {
            if (gb_read_hram(gb, hMapRoom) == UNKNOWN_ROOM_32) {
                attr2 = OAMF_PAL0 | OAM_GBC_PAL_6;
            }
        }
    }
    /* pop af; xor $20; ld [hl], a */
    attr2 ^= 0x20;
    gb_write(gb, wLinkOAMBuffer + 7, attr2);

    /* ld a, GROUND_STATUS_SLOW; ld [wLinkGroundStatus], a; ret */
    gb_write(gb, wLinkGroundStatus, GROUND_STATUS_SLOW);
}
