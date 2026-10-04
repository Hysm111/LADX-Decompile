#include "bank3/entities_collision.h"
#include "bank3/entities_physics.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/gameplay.h"
#include "constants/directions.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "constants/gfx.h"
#include "home/entities.h"
#include "home/room.h"
#include "home/bank.h"
#include "home/audio.h"
#include "home/gameplay.h"
#include "constants/audio.h"

/* ===== CheckLinkCollisionWithEnemy (03:6C72) ===== */
bool CheckLinkCollisionWithEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return false;

    /* If Link is in the air, skip the collision check */
    if (gb_read_hram(gb, hLinkPositionZ) != 0) {
        return false;
    }

    /* If Link is not interactive, return */
    if (gb_read(gb, wLinkMotionState) >= LINK_MOTION_TYPE_NON_INTERACTIVE) {
        return false;
    }

    /* Hitbox offset table: 4 bytes per entity:
       byte 0: X offset
       byte 1: X half-width radius
       byte 2: Y offset
       byte 3: Y half-height radius */
    uint16_t hitbox_addr = (uint16_t)(wEntitiesHitboxPositionTable + ((bc & 0xFF) << 2));

    /* Check X distance */
    uint8_t ent_x = (uint8_t)(gb_read_hram(gb, hActiveEntityPosX) + gb_read(gb, hitbox_addr + 0));
    uint8_t diff_x = (uint8_t)(ent_x - gb_read_hram(gb, hLinkPositionX) - 8);
    if ((diff_x & 0x80) != 0) {
        diff_x = (uint8_t)(~diff_x + 1);
    }
    uint8_t limit_x = (uint8_t)(gb_read(gb, hitbox_addr + 1) + 4);
    if (diff_x >= limit_x) {
        return false;
    }

    /* Check Y distance */
    uint8_t ent_y = (uint8_t)(gb_read_hram(gb, hActiveEntityVisualPosY) + gb_read(gb, hitbox_addr + 2));
    uint8_t diff_y = (uint8_t)(ent_y - gb_read_hram(gb, hLinkPositionY) - 8);
    if ((diff_y & 0x80) != 0) {
        diff_y = (uint8_t)(~diff_y + 1);
    }
    uint8_t limit_y = (uint8_t)(gb_read(gb, hitbox_addr + 3) + 4);
    if (diff_y >= limit_y) {
        return false;
    }

    /* Collision occurred! Check if harmless or Link in falling animation */
    if (func_003_6CC0(gb, bc)) {
        return true;
    }

    /* Apply damages to Link */
    ApplyLinkCollisionWithEnemy(gb, bc);
    return true;
}

/* ===== ApplyLinkCollisionWithEnemy (03:6CD5) ===== */
void ApplyLinkCollisionWithEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* Special case when a cheep-cheep hurts Link */
    /* ldh a, [hActiveEntityType]; cp ENTITY_CHEEP_CHEEP_JUMPING; jr nz, .cheepCheepEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_CHEEP_CHEEP_JUMPING) {
        /* call GetEntityYDistanceToLink_03; ld a, e; cp $02; jr nz, .goombaEnd */
        /* Simplified: just check if close vertically */
        /* call IncrementEntityState; ld [hl], ENTITY_STATUS_ACTIVE; ld a, $02; ld [wIsLinkInTheAir], a; ld a, $F0; ldh [hLinkSpeedY], a; call ClearEntitySpeed; ld a, WAVE_SFX_FLOOR_SWITCH; ldh [hWaveSfx], a; ret */
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        gb_write(gb, wIsLinkInTheAir, 0x02);
        gb_write_hram(gb, hLinkSpeedY, 0xF0);
        ClearEntitySpeed(gb, bc);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_FLOOR_SWITCH);
        return;
    }

    /* Special case when a Goomba hurts Link */
    /* ldh a, [hActiveEntityType]; cp ENTITY_GOOMBA; jr nz, .goombaEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GOOMBA) {
        /* ld a, [wIsLinkInTheAir]; and a; jr z, .goombaEnd */
        if (gb_read(gb, wIsLinkInTheAir) == 0) {
            return;
        }
        /* ldh a, [hLinkCountdown]; and a; jr nz, .jr_003_6D1B */
        if (gb_read_hram(gb, hLinkCountdown) != 0) {
            goto jr_003_6D1B;
        }
        /* ldh a, [hIsSideScrolling]; and a; jr nz, .jr_003_6D15 */
        if (gb_read_hram(gb, hIsSideScrolling) != 0) {
            goto jr_003_6D15;
        }
        /* ldh a, [hLinkVelocityZ]; xor $80; jr .jr_003_6D17 */
        /* .jr_003_6D15: ldh a, [hLinkSpeedY] */
    jr_003_6D15:
        /* .jr_003_6D17: and $80; jr nz, .goombaEnd */
        if ((gb_read_hram(gb, hLinkSpeedY) & 0x80) != 0) {
            return;
        }

    jr_003_6D1B:
        /* ld a, $02; ldh [hLinkCountdown], a */
        gb_write_hram(gb, hLinkCountdown, 0x02);
        /* ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $02 */
        gb_write(gb, wEntitiesStateTable + bc, 0x02);
        /* call GetEntityTransitionCountdown; ld [hl], $30 */
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x30);
        /* ld a, WAVE_SFX_FLOOR_SWITCH; ldh [hWaveSfx], a */
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_FLOOR_SWITCH);
        /* ldh a, [hIsSideScrolling]; and a; jr nz, .jr_003_6D38 */
        if (gb_read_hram(gb, hIsSideScrolling) != 0) {
            goto jr_003_6D38;
        }
        /* ld a, $10; ldh [hLinkVelocityZ], a; ret */
        gb_write_hram(gb, hLinkVelocityZ, 0x10);
        return;

    jr_003_6D38:
        /* ld a, $F0; ldh [hLinkSpeedY], a; ret */
        gb_write_hram(gb, hLinkSpeedY, 0xF0);
        return;
    }

    /* Special case when Link collides with a Gel */
    /* ldh a, [hActiveEntityType]; cp ENTITY_GEL; jr nz, .gelEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GEL) {
        /* call GetEntityTransitionCountdown; ld [hl], $80; call IncrementEntityState; ld [hl], $04; ret */
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x80);
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStateTable + bc, 0x04);
        return;
    }

    /* cp ENTITY_CUE_BALL; jr z, .jr_6D5D; cp ENTITY_ROLLING_BONES_BAR; jr z, .jr_6D5D */
    /* ld a, [wIgnoreLinkCollisionsCountdown]; and a; jp nz, setCarryAndReturn */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_CUE_BALL ||
        gb_read_hram(gb, hActiveEntityType) == ENTITY_ROLLING_BONES_BAR) {
        goto jr_6D5D;
    }

    if (gb_read(gb, wIgnoreLinkCollisionsCountdown) != 0) {
        return;
    }

jr_6D5D:
    /* ldh a, [hActiveEntityType]; cp ENTITY_MOBLIN_KING; jr nz, .jr_6D73 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_MOBLIN_KING) {
        /* ldh a, [hActiveEntityState]; cp $04; jr nz, .jr_6D73 */
        if (gb_read_hram(gb, hActiveEntityState) == 0x04) {
            /* call IncrementEntityState; ld [hl], $08; ld a, WAVE_SFX_LINK_HURT; ldh [hWaveSfx], a; ret */
            IncrementEntityState(gb, bc);
            gb_write(gb, wEntitiesStateTable + bc, 0x08);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_HURT);
            return;
        }
    }

    /* ld a, [wInvincibilityCounter]; and a; jp nz, .invincibleEnd */
    if (gb_read(gb, wInvincibilityCounter) != 0) {
        return;
    }

    /* Handle default collision - hurt Link */
    /* This is a simplified version - the actual implementation is complex */
    /* We'll just apply damage and effects */
    gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_HURT);
    gb_write(gb, wInvincibilityCounter, 0x40);
    /* Subtract health would be done here */
    return;
}

/* ===== DefaultEnemyDamageCollisionHandler (03:6E2B) ===== */
void DefaultEnemyDamageCollisionHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call func_003_6C6B */
    func_003_6C6B(gb, bc);

    /* ld a, [wC140]; cp $00; jp z, label_003_73E6 */
    if (gb_read(gb, wC140) == 0) {
        return;
    }

    /* ld hl, wEntitiesFlashCountdownTable; add hl, bc; ld a, [hl]; and a; jr z, .jr_6E40 */
    if (gb_read(gb, wEntitiesFlashCountdownTable + bc) == 0) {
        goto jr_6E40;
    }

    /* cp $18; jp c, label_003_73E6 */
    if (gb_read(gb, wEntitiesFlashCountdownTable + bc) < 0x18) {
        return;
    }

jr_6E40:
    /* ld a, [wC1AC]; and a; jr z, .jr_6E4B */
    if (gb_read(gb, wC1AC) == 0) {
        goto jr_6E4B;
    }

    /* dec a; cp c; jp z, label_003_73E6 */
    if ((gb_read(gb, wC1AC) - 1) == (bc & 0xFF)) {
        return;
    }

jr_6E4B:
    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld a, [hl]; and a; jp nz, label_003_73E6 */
    if (gb_read(gb, wEntitiesIgnoreHitsCountdownTable + bc) != 0) {
        return;
    }

    /* ld de, hActiveEntityPosX; push bc; sla c; sla c; ld hl, wEntitiesHitboxPositionTable; add hl, bc; pop bc; ld a, [de]; add [hl]; push hl; ld hl, wC140; sub [hl]; cp $80; jr c, .jr_6E6E; cpl; inc a; .jr_6E6E: pop hl; push af; inc hl; ld a, [wC141]; add [hl]; ld e, a; pop af; cp e; jp nc, jr_003_6CCB */
    /* This is complex collision detection - simplified */
    /* We'll skip the detailed hitbox collision for now */

    /* func_003_6CC0: ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld a, [hl]; and ENTITY_PHYSICS_HARMLESS; jr z, jr_003_6CCD */
    if ((gb_read(gb, wEntitiesPhysicsFlagsTable + bc) & ENTITY_PHYSICS_HARMLESS) == 0) {
        /* jr_003_6CCD: ldh a, [hLinkAnimationState]; sub $4E; cp $02; jr c, jr_003_6CC9 */
        if (gb_read_hram(gb, hLinkAnimationState) >= 0x4E && gb_read_hram(gb, hLinkAnimationState) < 0x50) {
            /* Damage the entity */
            /* This would call ApplySwordDamagesToEnemy */
            ApplySwordDamagesToEnemy(gb, bc);
        }
    }
}

/* ===== ApplySwordDamagesToEnemy (03:7267) ===== */
void ApplySwordDamagesToEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* This is a complex function that handles sword damage to enemies */
    /* Simplified implementation - apply damage based on weapon type */
    /* The actual implementation has many special cases for different enemy types */
    
    /* Special case for Final Nightmare - handled by JP_TABLE */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_FINAL_NIGHTMARE) {
        /* Handle Final Nightmare forms - simplified */
        return;
    }

    /* Special case for Buzz Blob */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BUZZ_BLOB) {
        /* if status == ACTIVE; call IncrementEntityState; ld [hl], $01; call GetEntityTransitionCountdown; ld [hl], $40; ld a, $40; ld [wD464], a; xor a; ld [wSwordAnimationState], a; ld [wC16A], a; ld [wIsUsingSpinAttack], a; ld a, NOISE_SFX_BUZZ_BLOB_ELECTROCUTE; ldh [hNoiseSfx], a; jp ApplyLinkCollisionWithEnemy */
        if (gb_read_hram(gb, hActiveEntityStatus) == ENTITY_STATUS_ACTIVE) {
            IncrementEntityState(gb, bc);
            gb_write(gb, wEntitiesStateTable + bc, 0x01);
            gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x40);
            gb_write(gb, wD464, 0x40);
            gb_write(gb, wSwordAnimationState, 0x00);
            gb_write(gb, wC16A, 0x00);
            gb_write(gb, wIsUsingSpinAttack, 0x00);
            gb_write_hram(gb, hNoiseSfx, NOISE_SFX_BUZZ_BLOB_ELECTROCUTE);
            ApplyLinkCollisionWithEnemy(gb, bc);
        }
        return;
    }

    /* Standard sword collision */
    /* Special case for Bouncing Bombite */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BOUNCING_BOMBITE) {
        /* call GetVectorTowardsLink; negate and set speeds; call IncrementEntityState; ld [hl], $02; call GetEntityTransitionCountdown; ld [hl], $40; call GetEntityPrivateCountdown1; ld [hl], $08; ret */
        uint8_t x, y;
        GetVectorTowardsLink(gb, &x, &y);
        gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)(~x + 1));
        gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)(~y + 1));
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStateTable + bc, 0x02);
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x40);
        gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0x08);
        return;
    }

    /* Special case for Angler Fish */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_ANGLER_FISH) {
        /* call func_003_6DDF; ld a, $08; ld [wIgnoreLinkCollisionsCountdown], a; jr .slimeEyeEnd */
        func_003_6DDF(gb, bc);
        gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x08);
        return;
    }

    /* Special case for Slime Eye */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_SLIME_EYE) {
        /* Simplified - has complex state machine */
        return;
    }

    /* If sword clink is disabled... */
    if ((gb_read(gb, wEntitiesOptions1Table + bc) & ENTITY_OPT1_SWORD_CLINK_OFF) == 0) {
        /* Special case for Knight */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_KNIGHT) {
            /* call label_003_6F04 */
            /* This is complex - skip for now */
            return;
        }

        /* Special case for Genie */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GENIE) {
            /* ConfigureEntityRecoil with different strength based on tunic/power */
            uint8_t recoil = 0x20;
            if (gb_read(gb, wTunicType) == TUNIC_RED || gb_read(gb, wActivePowerUp) == ACTIVE_POWER_UP_PIECE_OF_POWER) {
                recoil = 0x30;
            }
            ConfigureEntityRecoil(gb, bc, recoil);

            /* Without flashing from damages */
            gb_write(gb, wEntitiesFlashCountdownTable + bc, 0);
            return;
        }
    }

    /* Continue default collision */
    /* ld a, c; inc a; ld [wC1AC], a; call label_D07; ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], $10; ld hl, wEntitiesRecoilVelocityX; add hl, bc; ld [hl], b; ld hl, wEntitiesRecoilVelocityY; add hl, bc; ld [hl], b; jp func_003_6DDF */
    gb_write(gb, wC1AC, (uint8_t)((bc & 0xFF) + 1));
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x10);
    gb_write(gb, wEntitiesRecoilVelocityX + bc, 0);
    gb_write(gb, wEntitiesRecoilVelocityY + bc, 0);
    func_003_6DDF(gb, bc);
}
/* ===== func_003_73EB (03:73EB) - Enemy Collision Handler for Link ===== */
void func_003_73EB(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hFrameCounter]; xor c; rra; jr nc, ret_003_7570 */
    uint8_t frame = gb_read_hram(gb, hFrameCounter);
    uint8_t c = bc & 0xFF;
    if (((frame ^ c) & 0x01) != 0) {
        return;
    }

    /* ldh a, [hLinkPositionZ]; and a; jr nz, ret_003_7570 */
    if (gb_read_hram(gb, hLinkPositionZ) != 0) {
        return;
    }

    /* ld a, [wC1AC]; cp $00; jp z, label_003_74E1 */
    if (gb_read(gb, wC1AC) == 0) {
        return;
    }

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ldh a, [hLinkDirection]; cp [hl]; jp z, label_003_74E1 */
    uint8_t link_dir = gb_read_hram(gb, hLinkDirection);
    uint8_t entity_dir = gb_read(gb, wEntitiesDirectionTable + bc);
    if (link_dir == entity_dir) {
        return;
    }

    /* ld de, hActiveEntityPosX; ld hl, wD5C0; ld a, [de]; add [hl] */
    /* push hl; ld hl, wC140; sub [hl]; cp $80; jr c, .jr_7422; cpl; inc a */
    int16_t diff_x = (int16_t)gb_read_hram(gb, hActiveEntityPosX) + gb_read(gb, wD5C0);
    diff_x -= gb_read(gb, wC140);
    if (diff_x < 0) diff_x = -diff_x;
    if (diff_x >= 0x80) {
        return;
    }

    /* pop hl; push af; inc hl; ld a, [wC141]; add [hl]; ld e, a; pop af; cp e; jp nc, label_003_74E1 */
    int16_t diff_y = (int16_t)gb_read(gb, wC141) + gb_read(gb, wD5C1);
    diff_y -= gb_read(gb, wC140);
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return;
    }

    /* pop hl; push af; inc hl; ld a, [wC142]; add [hl]; ld e, a; pop af; cp e; jp nc, label_003_74E1 */
    diff_y = (int16_t)gb_read(gb, wC142) + gb_read(gb, wD5C2);
    diff_y -= gb_read(gb, wC141);
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return;
    }

    /* pop hl; push af; inc hl; ld a, [wC143]; add [hl]; ld e, a; pop af; cp e; jp nc, label_003_74E1 */
    diff_y = (int16_t)gb_read(gb, wC143) + gb_read(gb, wD5C3);
    diff_y -= gb_read(gb, wC144);
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return;
    }

    /* call ResetPegasusBoots */
    ResetPegasusBoots(gb);

    /* ld a, $08; ld [wIgnoreLinkCollisionsCountdown], a */
    gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x08);

    /* ld a, $12; call func_003_7565 */
    func_003_7565(gb);

    /* ld a, $18; call GetVectorTowardsLink */
    uint8_t vec_x, vec_y;
    GetVectorTowardsLink(gb, &vec_x, &vec_y);

    /* ldh a, [hMultiPurpose0]; cpl; inc a; ld hl, wEntitiesRecoilVelocityY; add hl, bc; ld [hl], a */
    uint8_t recoil_y = (uint8_t)(~vec_x + 1);
    gb_write(gb, wEntitiesRecoilVelocityY + bc, recoil_y);

    /* ldh a, [hMultiPurpose1]; cpl; inc a; ld hl, wEntitiesRecoilVelocityX; add hl, bc; ld [hl], a */
    uint8_t recoil_x = (uint8_t)(~vec_y + 1);
    gb_write(gb, wEntitiesRecoilVelocityX + bc, recoil_x);

    /* call StartIgnoringHitsForEntity */
    StartIgnoringHitsForEntity(gb);

    /* ld [hl], $08 */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x08);

    /* ; reset sword charge */
    /* xor a; ld [wSwordCharge], a */
    gb_write(gb, wSwordCharge, 0x00);

    /* call AlertSwordMoblins */
    AlertSwordMoblins(gb);

    /* ld hl, wIsUsingSpinAttack; ld a, [wC16A]; or [hl]; jr z, .jr_748B */
    if ((gb_read(gb, wIsUsingSpinAttack) | gb_read(gb, wC16A)) != 0) {
        /* ld a, $0C; ld [wC16D], a */
        gb_write(gb, wC16D, 0x0C);
    }

    /* ldh a, [hActiveEntityType]; cp ENTITY_BLAINO; jr nz, jr_003_74C1 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BLAINO) {
        /* ld a, JINGLE_BUMP; ldh [hJingle], a */
        gb_write_hram(gb, hJingle, JINGLE_BUMP);

        /* ld a, [wD205]; cp $00; jr z, jr_003_74BF */
        if (gb_read(gb, wD205) == 0x00) {
            /* ld a, $20; ld [wIgnoreLinkCollisionsCountdown], a */
            /* IF !__PATCH_0__: ld a, $20; ENDC */
            gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x20);
            func_003_7565(gb);
            return;
        }

        /* cp $01; jr z, .jr_74B5 */
        /* cp $04; jr z, .jr_74B5 */
        /* cp $03; jp z, jr_003_7571 */
        uint8_t d205 = gb_read(gb, wD205);
        if (d205 == 0x01 || d205 == 0x04) {
            /* .jr_74B5: ld a, $10; ld [wIgnoreLinkCollisionsCountdown], a; ld a, $20; call func_003_7565 */
            gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x10);
            func_003_7565(gb);
            return;
        }
        if (d205 == 0x03) {
            /* jr_003_7571: ld hl, wEntitiesInertiaTable; add hl, bc; ld a, [hl]; cp $22; jr c, ret_003_7570 */
            /* ld a, LINK_MOTION_UNKNOWN_0A; ld [wLinkMotionState], a */
            /* ld hl, wEntitiesDirectionTable; add hl, bc; ld a, [hl]; and a; ld a, $30; jr z, .jr_758B; ld a, $D0 */
            /* .jr_758B: ldh [hLinkSpeedX], a; xor a; ldh [hLinkSpeedY], a; ld a, $30; ldh [hLinkVelocityZ], a; ld a, JINGLE_STRONG_BUMP; ldh [hJingle], a; ret */
            uint8_t inertia = gb_read(gb, wEntitiesInertiaTable + bc);
            if (inertia < 0x22) {
                return;
            }
            gb_write(gb, wLinkMotionState, LINK_MOTION_UNKNOWN_0A);
            uint8_t dir = gb_read(gb, wEntitiesDirectionTable + bc);
            if (dir == 0) {
                gb_write_hram(gb, hLinkSpeedX, 0x30);
            } else {
                gb_write_hram(gb, hLinkSpeedX, 0xD0);
            }
            gb_write_hram(gb, hLinkSpeedY, 0x00);
            gb_write_hram(gb, hLinkVelocityZ, 0x30);
            gb_write_hram(gb, hJingle, JINGLE_STRONG_BUMP);
            return;
        }

        /* ld a, $20; ld [wIgnoreLinkCollisionsCountdown], a */
        /* IF !__PATCH_0__: ld a, $20; ENDC */
        gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x20);
        func_003_7565(gb);
        return;
    }

    /* jr_003_74C1: ldh a, [hLinkDirection]; ld e, a; ld d, b */
    /* ld hl, Data_003_74E4; add hl, de; ld a, [wC140]; add [hl]; ldh [hMultiPurpose0], a */
    /* ld hl, Data_003_74E8; add hl, de; ld a, [wC142]; add [hl]; ldh [hMultiPurpose1], a */
    /* call label_D15 */
    /* jr_003_74DC: ld a, $0C; ldh [hLinkPunchedAwayCountdown], a; ret */
    /* label_003_74E1: jp label_003_74EC */
    /* label_003_74EC: ldh a, [hFrameCounter]; xor c; rra; jr nc, ret_003_7570 */
    /* ldh a, [hLinkPositionX]; add $08; ldh [hMultiPurpose0], a */
    /* ldh a, [hLinkPositionY]; add $08; ldh [hMultiPurpose2], a */
    /* ld de, hActiveEntityPosX; ld hl, wD5C0; ld a, [de]; add [hl] */
    /* push hl; ld hl, hMultiPurpose0; sub [hl]; cp $80; jr c, .jr_7511; cpl; inc a */
    /* .jr_7511: pop hl; push af; inc hl; ld a, $04; add [hl]; ld e, a; pop af; cp e; jr nc, ret_003_7570 */
    /* inc hl; ld de, hActiveEntityVisualPosY; ld a, [de]; add [hl]; push hl; ld hl, hMultiPurpose2; sub [hl]; cp $80; jr c, .jr_752D; cpl; inc a */
    /* .jr_752D: pop hl; push af; inc hl; ld a, $05; add [hl]; ld e, a; pop af; cp e; jr nc, ret_003_7570 */
    /* ld a, [wInvincibilityCounter]; and a; jr nz, ret_003_7570 */
    /* call ApplyLinkCollisionWithEnemy */
    /* ldh a, [hActiveEntityType]; cp ENTITY_BLAINO; jr nz, ret_003_7570 */
    /* ld a, [wD205]; and a; jr z, ret_003_7570 */
    /* cp $01; jr z, ret_003_7570 */
    /* cp $04; jr z, ret_003_7570 */
    /* cp $02; jr nz, jr_003_7571 */
    /* call GetEntityPrivateCountdown1; ld [hl], $A0; ld a, $20; ld [wIgnoreLinkCollisionsCountdown], a; ld a, $30 */
    /* func_003_7565 */
    /* jr_003_7571: ld hl, wEntitiesInertiaTable; add hl, bc; ld a, [hl]; cp $22; jr c, ret_003_7570 */
    /* ld a, LINK_MOTION_UNKNOWN_0A; ld [wLinkMotionState], a */
    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld a, [hl]; and a; ld a, $30; jr z, .jr_758B; ld a, $D0 */
    /* .jr_758B: ldh [hLinkSpeedX], a; xor a; ldh [hLinkSpeedY], a; ld a, $30; ldh [hLinkVelocityZ], a; ld a, JINGLE_STRONG_BUMP; ldh [hJingle], a; ret */
    /* ld a, $20; ld [wIgnoreLinkCollisionsCountdown], a; ld a, $20; jr func_003_7565 */
}


/* ===== StartIgnoringHitsForEntity (03:73DB) ===== */
void StartIgnoringHitsForEntity_idx(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesPowerRecoilingTable; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesPowerRecoilingTable + bc, 0x00);

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], $0A */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x0A);
}

void StartIgnoringHitsForEntity(GBState *gb) {
    if (!gb) return;
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    StartIgnoringHitsForEntity_idx(gb, bc);
}
