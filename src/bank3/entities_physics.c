#include "bank3/entities_physics.h"
#include "bank3/entities_collision.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/gameplay.h"
#include "constants/directions.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "constants/gfx.h"
#include "constants/vfx.h"
#include "constants/audio.h"
#include "home/entities.h"
#include "home/vfx.h"
#include "home/room.h"
#include "home/bank.h"
#include "home/audio.h"
#include "home/gameplay.h"

/* ===== BouncingEntityPhysics (03:60B3) ===== */
void BouncingEntityPhysics(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call UpdateEntityPosWithSpeed_03 */
    UpdateEntityPosWithSpeed_03(gb, bc);
    /* call func_003_6B7B */
    func_003_6B7B(gb, bc);
    /* call ApplyEntityInteractionWithBackground */
    ApplyEntityInteractionWithBackground(gb, bc);

    /* ldh a, [hIsSideScrolling]; and a; jr z, .sidescrollingEnd */
    if (gb_read_hram(gb, hIsSideScrolling) == 0) {
        goto sidescrollingEnd;
    }

    /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; and $08; jp z, .return */
    if ((gb_read(gb, wEntitiesCollisionsTable + bc) & 0x08) == 0) {
        return;
    }

    /* ld hl, wEntitiesPosYTable; add hl, bc; ld a, [hl]; and $F0; add $05; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    pos_y = (pos_y & 0xF0) + 0x05;
    gb_write(gb, wEntitiesPosYTable + bc, pos_y);

    /* ld hl, wEntitiesSpeedYTable; add hl, bc; ld a, [hl]; cpl; sra a; cp $F8; jr c, .makeBouncingNoise */
    int8_t speed_y = (int8_t)gb_read(gb, wEntitiesSpeedYTable + bc);
    speed_y = (int8_t)(~speed_y);  /* cpl */
    speed_y >>= 1;  /* sra */
    if ((uint8_t)speed_y < 0xF8) {
        goto makeBouncingNoise;
    }

    goto shallowWaterEnd;

sidescrollingEnd:
    /* ld hl, wEntitiesPosZTable; add hl, bc; ld a, [hl]; and $80; jr z, .return */
    if ((gb_read(gb, wEntitiesPosZTable + bc) & 0x80) == 0) {
        return;
    }

    /* xor a; ld [hl], a */
    gb_write(gb, wEntitiesPosZTable + bc, 0x00);

    /* ld hl, wEntitiesGroundStatusTable; add hl, bc; ld a, [hl] */
    uint8_t ground_status = gb_read(gb, wEntitiesGroundStatusTable + bc);

    /* ld hl, wEntitiesSpeedZTable; add hl, bc */
    /* cp ENTITY_GROUND_STATUS_SHALLOW_WATER; jr z, .shallowWaterEnd */
    if (ground_status == ENTITY_GROUND_STATUS_SHALLOW_WATER) {
        goto shallowWaterEnd;
    }

    /* ld a, [hl]; sra a; cpl; cp $07; jr nc, .makeBouncingNoise */
    int8_t speed_z = (int8_t)gb_read(gb, wEntitiesSpeedZTable + bc);
    speed_z >>= 1;
    speed_z = (int8_t)(~speed_z);
    if (speed_z >= 7) {
        goto makeBouncingNoise;
    }

shallowWaterEnd:
    /* xor a; push hl; ld hl, wEntitiesSpeedXTable; add hl, bc; ld [hl], a; ld hl, wEntitiesSpeedYTable; add hl, bc; ld [hl], a; pop hl; jr .makeBouncingNoiseEnd */
    gb_write(gb, wEntitiesSpeedXTable + bc, 0x00);
    gb_write(gb, wEntitiesSpeedYTable + bc, 0x00);
    goto makeBouncingNoiseEnd;

makeBouncingNoise:
    /* push af; push hl */
    /* ldh a, [hActiveEntityType]; cp ENTITY_KEY_DROP_POINT; jr nz, .keyEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_KEY_DROP_POINT) {
        /* ld a, NOISE_SFX_CLINK; ldh [hNoiseSfx], a; jr .bombEnd */
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_CLINK);
        goto bombEnd;
    }

    /* cp ENTITY_BOMB; jr nz, .bombEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BOMB) {
        /* ld hl, wEntitiesStatusTable; add hl, bc; ld a, [hl]; and a; jr z, .bombEnd */
        if (gb_read(gb, wEntitiesStatusTable + bc) == 0x00) {
            goto bombEnd;
        }
        /* cp ENTITY_STATUS_FALLING; jr z, .bombEnd */
        if (gb_read(gb, wEntitiesStatusTable + bc) == ENTITY_STATUS_FALLING) {
            goto bombEnd;
        }
        /* ld a, JINGLE_BUMP; ldh [hJingle], a */
        gb_write_hram(gb, hJingle, JINGLE_BUMP);
    }

bombEnd: ;
    /* pop hl; pop af */
    /* fallthrough to makeBouncingNoiseEnd */

makeBouncingNoiseEnd: ;
    /* ld [hl], a; ld hl, wEntitiesSpeedXTable; add hl, bc; ld a, [hl]; sra a; cp $FF; jr nz, .clearXSpeedEnd; xor a; .clearXSpeedEnd: ld [hl], a */
    int8_t speed_x = (int8_t)gb_read(gb, wEntitiesSpeedXTable + bc);
    speed_x >>= 1;  /* sra */
    if (speed_x == -1) {  /* $FF */
        speed_x = 0;
    }
    gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)speed_x);

    /* ldh a, [hIsSideScrolling]; and a; jr nz, .return */
    if (gb_read_hram(gb, hIsSideScrolling) != 0) {
        return;
    }

    /* ld hl, wEntitiesSpeedYTable; add hl, bc; ld a, [hl]; sra a; cp $FF; jr nz, .clearYSpeedEnd; xor a; .clearYSpeedEnd: ld [hl], a; .return: ret */
    int8_t speed_y2 = (int8_t)gb_read(gb, wEntitiesSpeedYTable + bc);
    speed_y2 >>= 1;  /* sra */
    if (speed_y2 == -1) {  /* $FF */
        speed_y2 = 0;
    }
    gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)speed_y2);
}

/* ===== func_003_6B7B (03:6B7B) ===== */
void func_003_6B7B(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hIsSideScrolling]; and a; jr nz, .sideScrolling */
    if (gb_read_hram(gb, hIsSideScrolling) != 0) {
        goto sideScrolling;
    }

    /* call AddEntityZSpeedToPos_03 */
    AddEntityZSpeedToPos_03(gb, bc);

    /* ld hl, wEntitiesSpeedZTable; add hl, bc; ld a, [hl]; sub $02; ld [hl], a */
    uint8_t speed_z = gb_read(gb, wEntitiesSpeedZTable + bc);
    speed_z = (uint8_t)(speed_z - 0x02);
    gb_write(gb, wEntitiesSpeedZTable + bc, speed_z);
    return;

sideScrolling: ;
    /* ld hl, wEntitiesGroundStatusTable; add hl, bc; ld a, [hl]; ld e, a; ld d, b; and a; jr z, .updateXSpeedEnd */
    uint8_t ground_status = gb_read(gb, wEntitiesGroundStatusTable + bc);
    if (ground_status == 0) {
        goto updateXSpeedEnd;
    }

    /* ldh a, [hFrameCounter]; and $07; jr nz, .updateXSpeedEnd */
    if ((gb_read_hram(gb, hFrameCounter) & 0x07) != 0) {
        goto updateXSpeedEnd;
    }

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld a, [hl]; and a; jr z, .updateXSpeedEnd */
    uint8_t speed_x = gb_read(gb, wEntitiesSpeedXTable + bc);
    if (speed_x == 0) {
        goto updateXSpeedEnd;
    }

    /* and $80; jr z, .positiveDifferenceX */
    if ((speed_x & 0x80) == 0) {
        goto positiveDifferenceX;
    }

    /* get here every 8 frames, if underwater and X speed is not 0 */
    /* inc [hl]; inc [hl] */
    speed_x += 2;
    gb_write(gb, wEntitiesSpeedXTable + bc, speed_x);
    goto updateXSpeedEnd;

positiveDifferenceX: ;
    /* dec [hl] */
    speed_x -= 1;
    gb_write(gb, wEntitiesSpeedXTable + bc, speed_x);

updateXSpeedEnd: ;
    /* ld hl, Data_003_6B73; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesSpeedYTable; add hl, bc; add [hl]; ld [hl], a */
    static const uint8_t Data_003_6B73[4] = { 0x02, 0x01, 0x02, 0x02 };
    static const uint8_t Data_003_6B77[4] = { 0x40, 0x08, 0x40, 0x40 };
    
    uint8_t entity_type = gb_read_hram(gb, hActiveEntityType);
    if (entity_type < 4) {
        uint8_t y_add = Data_003_6B73[entity_type];
        uint8_t speed_y = gb_read(gb, wEntitiesSpeedYTable + bc);
        speed_y += y_add;
        gb_write(gb, wEntitiesSpeedYTable + bc, speed_y);

        uint8_t y_sub = Data_003_6B77[entity_type];
        if (speed_y >= y_sub) {
            speed_y = y_sub;
            gb_write(gb, wEntitiesSpeedYTable + bc, speed_y);
        }
    }
    return;
}

/* ===== func_003_6C6B (03:6C6B) ===== */
bool func_003_6C6B(GBState *gb, uint16_t bc) {
    if (!gb) return false;

    /* ldh a, [hFrameCounter]; xor c; rra; jp nc, jr_003_6CCB */
    uint8_t frame = gb_read_hram(gb, hFrameCounter);
    uint8_t c = bc & 0xFF;
    if (((frame ^ c) & 0x01) != 0) {  /* rra + nc = bit 0 was 1 */
        return false;
    }
    return true;
}

/* ===== func_003_6CC0 (03:6CC0) ===== */
bool func_003_6CC0(GBState *gb, uint16_t bc) {
    if (!gb) return false;

    /* ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld a, [hl]; and ENTITY_PHYSICS_HARMLESS; jr z, jr_003_6CCD */
    if ((gb_read(gb, wEntitiesPhysicsFlagsTable + bc) & ENTITY_PHYSICS_HARMLESS) == 0) {
        return true;  /* Not harmless - collision possible */
    }
    return false;  /* Harmless - no collision */
}

/* ===== func_003_6DDF (03:6DDF) ===== */
void func_003_6DDF(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call ResetPegasusBoots */
    ResetPegasusBoots(gb);

    /* ld a, $10; ld [wIgnoreLinkCollisionsCountdown], a */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x10);

    /* ldh a, [hActiveEntityType]; ld e, $18; cp ENTITY_ROLLING_BONES_BAR; jp z, label_003_6FA7 */
    uint8_t entity_type = gb_read_hram(gb, hActiveEntityType);
    if (entity_type == ENTITY_ROLLING_BONES_BAR) {
        /* This jumps to label_003_6FA7 which is part of ConfigureEntityRecoil */
        /* We'll handle this in ConfigureEntityRecoil */
        return;
    }

    /* cp ENTITY_FACADE; jr nz, .facadeEnd */
    if (entity_type == ENTITY_FACADE) {
        /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld [hl], $01 */
        gb_write(gb, wEntitiesCollisionsTable + bc, 0x01);
    }

    /* cp ENTITY_MOLDORM; ld a, $14; jr nz, .moldormEnd; ld a, $18; .moldormEnd: */
    uint8_t moldorm_val = 0x14;
    if (entity_type == ENTITY_MOLDORM) {
        moldorm_val = 0x18;
    }

    /* call func_003_7565 */
    (void)moldorm_val;  /* Suppress unused warning */
    func_003_7565(gb);

    /* ldh a, [hIsSideScrolling]; and a; jr nz, jr_003_6E0E */
    if (gb_read_hram(gb, hIsSideScrolling) != 0) {
        goto jr_003_6E0E;
    }

    /* setCarryAndReturn: scf; ret */
    return;

jr_003_6E0E: ;
    /* ldh a, [hLinkPhysicsModifier]; cp $02; jr z, setCarryAndReturn */
    if (gb_read_hram(gb, hLinkPhysicsModifier) == 0x02) {
        return;
    }

    /* call GetEntityXDistanceToLink_03; ld d, b; ld hl, Data_003_6E0C; add hl, de; ld a, [hl]; ldh [hLinkSpeedX], a */
    uint8_t x_dist, y_dist;
    GetEntityXDistanceToLink_03(gb, &x_dist, &y_dist);
    
    static const int8_t Data_003_6E0C[2] = { 12, -12 };  /* right, left */
    int8_t link_speed_x = Data_003_6E0C[x_dist & 0x01];
    gb_write_hram(gb, hLinkSpeedX, (uint8_t)link_speed_x);

    /* ld a, $F4; ldh [hLinkSpeedY], a */
    gb_write_hram(gb, hLinkSpeedY, 0xF4);

    /* xor a; ldh [hLinkPhysicsModifier], a; scf; ret */
    gb_write_hram(gb, hLinkPhysicsModifier, 0x00);
}

/* ===== func_003_6F5C (03:6F5C) ===== */
void func_003_6F5C(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call func_003_6F93 */
    func_003_6F93(gb);

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x00);
}

/* ===== func_003_6F93 (03:6F93) ===== */
void func_003_6F93(GBState *gb) {
    if (!gb) return;

    /* ld a, JINGLE_BUMP; ldh [hJingle], a */
    gb_write_hram(gb, hJingle, JINGLE_BUMP);

    /* call ResetPegasusBoots */
    ResetPegasusBoots(gb);

    /* ld a, $0C; ld [wIgnoreLinkCollisionsCountdown], a */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x0C);

    /* ldh a, [hActiveEntityType]; cp ENTITY_ROLLING_BONES_BAR; jr nz, jr_003_6FB9 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_ROLLING_BONES_BAR) {
        /* ld e, $10; push de; call GetEntityXDistanceToLink_03; ld a, e; and a; pop de; ld a, e; jr z, .jr_6FB3; cpl; inc a; .jr_6FB3: ldh [hLinkSpeedX], a; xor a; ldh [hLinkSpeedY], a; ret */
        uint8_t x_dist, y_dist;
        GetEntityXDistanceToLink_03(gb, &x_dist, &y_dist);
        int8_t link_speed_x = (x_dist == 0) ? 0x10 : (int8_t)(~0x10 + 1);
        gb_write_hram(gb, hLinkSpeedX, (uint8_t)link_speed_x);
        gb_write_hram(gb, hLinkSpeedY, 0x00);
        return;
    }

    /* jr_003_6FB9: ld a, $12; call func_003_7565 */
    func_003_7565(gb);

    /* The rest handles facade and other special cases - simplified */
}

/* ===== func_003_7565 (03:7565) ===== */
void func_003_7565(GBState *gb) {
    if (!gb) return;

    /* call GetVectorTowardsLink */
    GetVectorTowardsLink(gb, NULL, NULL);

    /* ldh a, [hMultiPurpose0]; ldh [hLinkSpeedY], a */
    gb_write_hram(gb, hLinkSpeedY, gb_read_hram(gb, hMultiPurpose0));

    /* ldh a, [hMultiPurpose1]; ldh [hLinkSpeedX], a */
    gb_write_hram(gb, hLinkSpeedX, gb_read_hram(gb, hMultiPurpose1));
}

/* ===== func_003_75A2 (03:75A2) ===== */
void func_003_75A2(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld e, $0F; ld d, $00 */
    for (uint8_t e = 0x0F; e != 0xFF; e--) {
        (void)e;  /* de is used implicitly via e */

        /* If we are checking collision against ourselves, move to the next entity */
        if (e == (bc & 0xFF)) {
            continue;
        }

        /* If we are on an even frame, move to next (check every other frame) */
        if ((gb_read_hram(gb, hFrameCounter) ^ e) & 0x01) {
            continue;
        }

        /* If the entity is not interactive, move to next */
        if (gb_read(gb, wEntitiesStatusTable + e) < ENTITY_STATUS_ACTIVE) {
            continue;
        }

        /* If wEntitiesPhysicsFlagsTable[de] has PROJECTILE_NOCLIP, move to next */
        if (gb_read(gb, wEntitiesPhysicsFlagsTable + e) & ENTITY_PHYSICS_PROJECTILE_NOCLIP) {
            continue;
        }

        /* If the entities X are far apart, move to next */
        int16_t diff_x = (int16_t)gb_read_hram(gb, hActiveEntityPosX) - gb_read(gb, wEntitiesPosXTable + e) + 0x0C;
        if ((uint8_t)diff_x >= 0x18) {
            continue;
        }

        /* If the entities Y are far apart, move to next */
        int16_t diff_y = (int16_t)gb_read(gb, wEntitiesPosYTable + e) - gb_read(gb, wEntitiesPosZTable + e) - gb_read_hram(gb, hActiveEntityVisualPosY) + 0x0C;
        if ((uint8_t)diff_y >= 0x18) {
            continue;
        }

        /* If the entity sprite variant is $FF, move to next */
        if (gb_read(gb, wEntitiesSpriteVariantTable + e) == 0xFF) {
            continue;
        }

        /* If the active entity is a Bouncing Bombite... */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BOUNCING_BOMBITE) {
            /* wEntitiesSpriteVariantTable[de] = GetEntityTransitionCountdown */
            gb_write(gb, wEntitiesSpriteVariantTable + e, gb_read(gb, wEntitiesTransitionCountdownTable + bc));
        }

        /* If the collisioned entity is a Bouncing Bombite... */
        if (gb_read(gb, wEntitiesTypeTable + e) == ENTITY_BOUNCING_BOMBITE) {
            /* Copy speed and set state */
            gb_write(gb, wEntitiesSpeedXTable + e, gb_read(gb, wEntitiesSpeedXTable + bc));
            gb_write(gb, wEntitiesSpeedYTable + e, gb_read(gb, wEntitiesSpeedYTable + bc));
            gb_write(gb, wEntitiesTransitionCountdownTable + e, 0x40);
            gb_write(gb, wEntitiesStateTable + e, 0x02);
            gb_write(gb, wEntitiesPrivateCountdown1Table + e, 0x08);
            continue;
        }

        /* If the entity is grabbable, jump to label_003_7715 */
        if (gb_read(gb, wEntitiesPhysicsFlagsTable + e) & ENTITY_PHYSICS_GRABBABLE) {
            continue;  /* label_003_7715 not implemented */
        }

        /* If active entity is Magic Powder Sprinkle, force collision end */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_MAGIC_POWDER_SPRINKLE) {
            continue;
        }

        /* Final Nightmare special cases - simplified */

        /* If bit 7 of entity hitbox flag is set, force collision */
        if (gb_read(gb, wEntitiesHitboxFlagsTable + e) & 0x80) {
            gb_write(gb, wEntitiesCollisionsTable + bc, 0x01);
            continue;
        }

        /* Magic Powder Sprinkle special cases - simplified */

        /* If bit 7 of hitbox flags is set, skip */
        if (gb_read(gb, wEntitiesHitboxFlagsTable + e) & 0x80) {
            continue;
        }

        /* Force collision */
        gb_write(gb, wEntitiesCollisionsTable + bc, 0x01);

        /* continue loop */
    }
}

/* ===== ApplyEntityInteractionWithBackground (03:7386) ===== */
void ApplyEntityInteractionWithBackground(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - applies entity interaction with background tiles */
    (void)bc;
}

/* ===== ApplySwordIntersectionWithObjects (03:8194) ===== */
void ApplySwordIntersectionWithObjects(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - applies sword intersection with objects */
    (void)bc;
}

/* ===== GetEntityXDistanceToLink_03 (03:7ED9) ===== */
void GetEntityXDistanceToLink_03_idx(GBState *gb, uint16_t bc, uint8_t *e, uint8_t *d) {
    if (!gb) return;
    uint8_t dir = DIRECTION_RIGHT;
    uint8_t link_x = gb_read_hram(gb, hLinkPositionX);
    uint8_t ent_x = gb_read(gb, wEntitiesPosXTable + bc);
    uint8_t diff = (uint8_t)(link_x - ent_x);
    if ((diff & 0x80) != 0) {
        dir = (uint8_t)(dir + 1); /* DIRECTION_LEFT */
    }
    if (e) *e = dir;
    if (d) *d = diff;
}

void GetEntityXDistanceToLink_03(GBState *gb, uint8_t *e, uint8_t *d) {
    if (!gb) return;
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    GetEntityXDistanceToLink_03_idx(gb, bc, e, d);
}

/* ===== GetEntityYDistanceToLink_03 (03:7EE9) ===== */
void GetEntityYDistanceToLink_03_idx(GBState *gb, uint16_t bc, uint8_t *e, uint8_t *d) {
    if (!gb) return;
    uint8_t dir = DIRECTION_UP;
    uint8_t link_y = gb_read_hram(gb, hLinkPositionY);
    uint8_t ent_y = gb_read(gb, wEntitiesPosYTable + bc);
    uint8_t diff = (uint8_t)(link_y - ent_y);
    uint8_t ent_z = gb_read(gb, wEntitiesPosZTable + bc);
    diff = (uint8_t)(diff + ent_z);
    if ((diff & 0x80) == 0) {
        dir = (uint8_t)(dir + 1); /* DIRECTION_DOWN */
    }
    if (e) *e = dir;
    if (d) *d = diff;
}

void GetEntityYDistanceToLink_03(GBState *gb, uint8_t *e, uint8_t *d) {
    if (!gb) return;
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    GetEntityYDistanceToLink_03_idx(gb, bc, e, d);
}

/* ===== GetEntityDirectionToLink_03 (03:7EFE) ===== */
uint8_t GetEntityDirectionToLink_03(GBState *gb) {
    if (!gb) return 0;
    uint8_t dir_x = 0;
    uint8_t dist_x = 0;
    GetEntityXDistanceToLink_03(gb, &dir_x, &dist_x);
    gb_write_hram(gb, hMultiPurpose0, dir_x);

    uint8_t abs_x = dist_x;
    if ((abs_x & 0x80) != 0) {
        abs_x = (uint8_t)(~abs_x + 1);
    }

    uint8_t dir_y = 0;
    uint8_t dist_y = 0;
    GetEntityYDistanceToLink_03(gb, &dir_y, &dist_y);
    gb_write_hram(gb, hMultiPurpose1, dir_y);

    uint8_t abs_y = dist_y;
    if ((abs_y & 0x80) != 0) {
        abs_y = (uint8_t)(~abs_y + 1);
    }

    uint8_t result_dir;
    if (abs_y >= abs_x) {
        result_dir = gb_read_hram(gb, hMultiPurpose1);
    } else {
        result_dir = gb_read_hram(gb, hMultiPurpose0);
    }
    return result_dir;
}

/* ===== GetVectorTowardsLink (03:7E45) ===== */
void GetVectorTowardsLink_with_length(GBState *gb, uint8_t length, uint8_t *val0, uint8_t *val1) {
    if (!gb) return;

    gb_write_hram(gb, hMultiPurpose1, length);
    if (length == 0) {
        gb_write_hram(gb, hMultiPurpose0, 0x00);
        if (val0) *val0 = 0x00;
        if (val1) *val1 = 0x00;
        return;
    }

    uint8_t dir_y = 0;
    uint8_t dist_y = 0;
    GetEntityYDistanceToLink_03(gb, &dir_y, &dist_y);
    /* dec e; dec e; ld a, e; ldh [hMultiPurpose2], a */
    uint8_t dy_flag = (uint8_t)(dir_y - 2); /* 0 if UP (dy < 0), 1 if DOWN (dy >= 0) */
    gb_write_hram(gb, hMultiPurpose2, dy_flag);

    uint8_t abs_y = dist_y;
    if ((abs_y & 0x80) != 0) {
        abs_y = (uint8_t)(~abs_y + 1);
    }
    gb_write_hram(gb, hMultiPurposeC, abs_y);

    uint8_t dir_x = 0;
    uint8_t dist_x = 0;
    GetEntityXDistanceToLink_03(gb, &dir_x, &dist_x);
    /* ldh [hMultiPurpose3], a (where a is e: 1 if LEFT, 0 if RIGHT) */
    gb_write_hram(gb, hMultiPurpose3, dir_x);

    uint8_t abs_x = dist_x;
    if ((abs_x & 0x80) != 0) {
        abs_x = (uint8_t)(~abs_x + 1);
    }
    gb_write_hram(gb, hMultiPurposeD, abs_x);

    uint8_t swapped = 0;
    /* cp [hl] where a = [hMultiPurposeD] (abs_x) and [hl] = [hMultiPurposeC] (abs_y) */
    /* if abs_x < abs_y, swap them and swapped = 1 */
    if (gb_read_hram(gb, hMultiPurposeD) < gb_read_hram(gb, hMultiPurposeC)) {
        swapped = 1;
        uint8_t tmp_c = gb_read_hram(gb, hMultiPurposeC);
        uint8_t tmp_d = gb_read_hram(gb, hMultiPurposeD);
        gb_write_hram(gb, hMultiPurposeD, tmp_c);
        gb_write_hram(gb, hMultiPurposeC, tmp_d);
    }

    uint8_t acc = 0;
    uint8_t res0 = 0;
    uint8_t counter = gb_read_hram(gb, hMultiPurpose1);
    uint8_t small = gb_read_hram(gb, hMultiPurposeC);
    uint8_t large = gb_read_hram(gb, hMultiPurposeD);

    while (counter > 0) {
        uint16_t sum = (uint16_t)acc + small;
        if (sum > 0xFF) {
            acc = (uint8_t)(sum - large);
            res0++;
        } else {
            uint8_t s = (uint8_t)sum;
            if (s >= large) {
                s = (uint8_t)(s - large);
                res0++;
            }
            acc = s;
        }
        counter--;
    }
    gb_write_hram(gb, hMultiPurposeB, acc);
    gb_write_hram(gb, hMultiPurpose0, res0);

    /* If X and Y were swapped before, swap back */
    if (swapped != 0) {
        uint8_t val_0 = gb_read_hram(gb, hMultiPurpose0);
        uint8_t val_1 = gb_read_hram(gb, hMultiPurpose1);
        gb_write_hram(gb, hMultiPurpose0, val_1);
        gb_write_hram(gb, hMultiPurpose1, val_0);
    }

    /* If dy < 0 (hMultiPurpose2 == 0), negate Y */
    if (gb_read_hram(gb, hMultiPurpose2) == 0) {
        uint8_t y_val = gb_read_hram(gb, hMultiPurpose0);
        gb_write_hram(gb, hMultiPurpose0, (uint8_t)(~y_val + 1));
    }

    /* If dx < 0 (hMultiPurpose3 != 0), negate X */
    if (gb_read_hram(gb, hMultiPurpose3) != 0) {
        uint8_t x_val = gb_read_hram(gb, hMultiPurpose1);
        gb_write_hram(gb, hMultiPurpose1, (uint8_t)(~x_val + 1));
    }

    if (val0) *val0 = gb_read_hram(gb, hMultiPurpose0);
    if (val1) *val1 = gb_read_hram(gb, hMultiPurpose1);
}

void GetVectorTowardsLink(GBState *gb, uint8_t *x, uint8_t *y) {
    if (!gb) return;
    uint8_t len = gb_read_hram(gb, hMultiPurpose1);
    if (len == 0) {
        len = 0x12; /* Default length */
    }
    GetVectorTowardsLink_with_length(gb, len, x, y);
}

/* ===== ApplyVectorTowardsLink (03:7EC7) ===== */
void ApplyVectorTowardsLink(GBState *gb, uint16_t bc) {
    if (!gb) return;
    GetVectorTowardsLink(gb, NULL, NULL);
    gb_write(gb, wEntitiesSpeedYTable + bc, gb_read_hram(gb, hMultiPurpose0));
    gb_write(gb, wEntitiesSpeedXTable + bc, gb_read_hram(gb, hMultiPurpose1));
}

/* ===== AddEntitySpeedToPos_03 (03:7F32) ===== */
void AddEntitySpeedToPos_03(GBState *gb, uint16_t bc) {
    if (!gb) return;
    uint8_t speed = gb_read(gb, wEntitiesSpeedXTable + bc);
    if (speed == 0) return;

    /* swap a; and $F0 */
    uint8_t speed_frac = (uint8_t)(((speed << 4) | (speed >> 4)) & 0xF0);
    uint8_t acc = gb_read(gb, wEntitiesSpeedXAccTable + bc);
    uint16_t sum = (uint16_t)acc + speed_frac;
    gb_write(gb, wEntitiesSpeedXAccTable + bc, (uint8_t)sum);
    uint8_t carry = (sum > 0xFF) ? 1 : 0;

    /* Sign extension for high nibble */
    uint8_t e = (speed & 0x80) ? 0xF0 : 0x00;
    uint8_t int_part = (uint8_t)((((speed << 4) | (speed >> 4)) & 0x0F) | e);

    uint8_t pos = gb_read(gb, wEntitiesPosXTable + bc);
    pos = (uint8_t)(pos + int_part + carry);
    gb_write(gb, wEntitiesPosXTable + bc, pos);
}

/* ===== AddEntityZSpeedToPos_03 (03:8790) ===== */
void AddEntityZSpeedToPos_03(GBState *gb, uint16_t bc) {
    if (!gb) return;
    uint8_t speed = gb_read(gb, wEntitiesSpeedZTable + bc);
    if (speed == 0) return;

    uint8_t speed_frac = (uint8_t)(((speed << 4) | (speed >> 4)) & 0xF0);
    uint8_t acc = gb_read(gb, wEntitiesSpeedZAccTable + bc);
    uint16_t sum = (uint16_t)acc + speed_frac;
    gb_write(gb, wEntitiesSpeedZAccTable + bc, (uint8_t)sum);
    uint8_t carry = (sum > 0xFF) ? 1 : 0;

    uint8_t e = (speed & 0x80) ? 0xF0 : 0x00;
    uint8_t int_part = (uint8_t)((((speed << 4) | (speed >> 4)) & 0x0F) | e);

    uint8_t pos = gb_read(gb, wEntitiesPosZTable + bc);
    pos = (uint8_t)(pos + int_part + carry);
    gb_write(gb, wEntitiesPosZTable + bc, pos);
}

/* ===== UpdateEntityPosWithSpeed_03 (03:8729) ===== */
void UpdateEntityPosWithSpeed_03(GBState *gb, uint16_t bc) {
    if (!gb) return;
    AddEntitySpeedToPos_03(gb, bc);
    AddEntitySpeedToPos_03(gb, (uint16_t)(bc + 0x10));
}

/* ===== ConfigureEntityRecoil (03:6FCC) ===== */
void ConfigureEntityRecoil(GBState *gb, uint16_t bc, uint8_t recoil_amount) {
    if (!gb) return;

    GetVectorTowardsLink_with_length(gb, recoil_amount, NULL, NULL);

    /* ldh a, [hMultiPurpose0]; cpl; inc a; ld hl, wEntitiesRecoilVelocityY; add hl, bc; ld [hl], a */
    uint8_t vec_y = gb_read_hram(gb, hMultiPurpose0);
    uint8_t recoil_y = (uint8_t)(~vec_y + 1);
    gb_write(gb, wEntitiesRecoilVelocityY + bc, recoil_y);

    /* ldh a, [hMultiPurpose1]; cpl; inc a; ld hl, wEntitiesRecoilVelocityX; add hl, bc; ld [hl], a */
    uint8_t vec_x = gb_read_hram(gb, hMultiPurpose1);
    uint8_t recoil_x = (uint8_t)(~vec_x + 1);
    gb_write(gb, wEntitiesRecoilVelocityX + bc, recoil_x);

    /* jp StartIgnoringHitsForEntity */
    StartIgnoringHitsForEntity_idx(gb, bc);
}
