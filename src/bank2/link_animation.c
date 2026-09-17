#include "bank2/link_animation.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/inventory.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "home/link.h"

const uint8_t DirectionToLinkAnimationState[4] = {
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_RIGHT, /* 0: 0x11 */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_LEFT,  /* 1: 0x10 */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_UP,    /* 2: 0x0F */
    LINK_ANIMATION_STATE_HOOKSHOT_CHAIN_DOWN   /* 3: 0x0E */
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
