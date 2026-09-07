#include "bank2/bank2.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "home/check_items_to_use.h"
#include "home/entities.h"
#include "home/link.h"

const int8_t HookshotChainSpeedX[4] = {
    0x30,  /* DIRECTION_RIGHT:  HOOKSHOT_CHAIN_SPEED ($30) */
    -0x30, /* DIRECTION_LEFT:  -HOOKSHOT_CHAIN_SPEED ($D0) */
    0x00,  /* DIRECTION_UP */
    0x00   /* DIRECTION_DOWN */
};

const int8_t HookshotChainSpeedY[4] = {
    0x00,  /* DIRECTION_RIGHT */
    0x00,  /* DIRECTION_LEFT */
    -0x30, /* DIRECTION_UP:    -HOOKSHOT_CHAIN_SPEED ($D0) */
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

bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return false;

    uint16_t de = 0;
    if (spawn_new_entity) {
        de = SpawnNewEntity_trampoline(gb, ENTITY_CHEST_WITH_ITEM, spawn_new_entity);
        if (de >= MAX_ENTITIES) {
            return false;
        }
    } else {
        int found_slot = -1;
        for (int i = MAX_ENTITIES - 1; i >= 0; i--) {
            if (gb_read(gb, (uint16_t)(wEntitiesStatusTable + i)) == ENTITY_STATUS_DISABLED) {
                found_slot = i;
                break;
            }
        }
        if (found_slot < 0) {
            return false;
        }
        de = (uint16_t)found_slot;
        gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), ENTITY_STATUS_ACTIVE);
        gb_write(gb, (uint16_t)(wEntitiesTypeTable + de), ENTITY_CHEST_WITH_ITEM);
    }

    /* ld hl, wEntitiesStatusTable; add hl, de; dec [hl] */
    uint8_t status = gb_read(gb, (uint16_t)(wEntitiesStatusTable + de));
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + de), (uint8_t)(status - 1));

    /* ldh a, [hIntersectedObjectLeft]; and $F0; add $08; ld [wEntitiesPosXTable + de], a */
    uint8_t left = gb_read_hram(gb, hIntersectedObjectLeft);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + de), (uint8_t)((left & 0xF0) + 0x08));

    /* ldh a, [hIntersectedObjectTop]; and $F0; add $10; ld [wEntitiesPosYTable + de], a */
    uint8_t top = gb_read_hram(gb, hIntersectedObjectTop);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + de), (uint8_t)((top & 0xF0) + 0x10));

    /* ldh a, [hMultiPurpose8]; ld [wEntitiesSpriteVariantTable + de], a */
    uint8_t variant = gb_read_hram(gb, hMultiPurpose8);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), variant);

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

    uint8_t song_index = gb_read(gb, wSelectedSongIndex);
    if (song_index == 1) {
        /* Manbo's Mambo */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_MAMBO);
    } else if (song_index == 2) {
        /* Frog's Song of the Soul */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xBB);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_FROG);
    } else {
        /* Ballad of the Wind Fish */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xDC);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_BALLAD);
    }
}

bool FireHookshot(GBState *gb) {
    if (!gb) return false;

    if (gb_read(gb, wIsLinkInTheAir) != 0) {
        return false;
    }

    uint8_t de = 0;
    if (!SpawnPlayerProjectile(gb, ENTITY_HOOKSHOT_CHAIN, &de)) {
        return false;
    }

    gb_write(gb, (uint16_t)(wEntitiesTransitionCountdownTable + de), 0x2A);
    gb_write(gb, (uint16_t)(wEntitiesSpriteVariantTable + de), 0x00);

    uint8_t dir = gb_read_hram(gb, hLinkDirection) & 0x03;
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + de), (uint8_t)HookshotChainSpeedX[dir]);
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + de), (uint8_t)HookshotChainSpeedY[dir]);

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
