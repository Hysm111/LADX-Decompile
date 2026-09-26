#include "bank3/entities_moblin.h"
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

/* ===== Data Tables (03:581B-03:59D6) ===== */

/* RoamingEnemySpeedXPerDirection (03:581B) */
static const int8_t RoamingEnemySpeedXPerDirection[4] = { 8, -8, 0, 0 };

/* RoamingEnemySpeedYPerDirection (03:581F) */
static const int8_t RoamingEnemySpeedYPerDirection[4] = { 0, 0, -8, 8 };

/* EntityVariantForDirection_03 (03:5823) */
static const uint8_t EntityVariantForDirection_03[4] = { 6, 4, 2, 0 };

/* MoblinSpriteVariants (03:5917) - 8 variants * 4 bytes each = 32 bytes */
static const uint8_t MoblinSpriteVariants[32] = {
    /* variant 0: down */
    0x60, OAM_GBC_PAL_3, 0x62, OAM_GBC_PAL_3,
    /* variant 1: down flipped */
    0x62, OAM_GBC_PAL_3 | OAMF_XFLIP, 0x60, OAM_GBC_PAL_3 | OAMF_XFLIP,
    /* variant 2: up */
    0x64, OAM_GBC_PAL_3, 0x66, OAM_GBC_PAL_3,
    /* variant 3: up flipped */
    0x66, OAM_GBC_PAL_3 | OAMF_XFLIP, 0x64, OAM_GBC_PAL_3 | OAMF_XFLIP,
    /* variant 4: left */
    0x68, OAM_GBC_PAL_3, 0x6A, OAM_GBC_PAL_3,
    /* variant 5: left */
    0x6C, OAM_GBC_PAL_3, 0x6E, OAM_GBC_PAL_3,
    /* variant 6: right */
    0x6A, OAM_GBC_PAL_3 | OAMF_XFLIP, 0x68, OAM_GBC_PAL_3 | OAMF_XFLIP,
    /* variant 7: right */
    0x6E, OAM_GBC_PAL_3 | OAMF_XFLIP, 0x6C, OAM_GBC_PAL_3 | OAMF_XFLIP
};

/* Moblin Arrow data (03:5937-03:5946) */
static const int8_t MoblinArrowOffsetXPerDirection[4] = { 8, -8, 4, -4 };
static const int8_t MoblinArrowOffsetYPerDirection[4] = { -4, -4, -8, 0 };
static const int8_t MoblinArrowSpeedXPerDirection[4] = { 32, -32, 0, 0 };
static const int8_t MoblinArrowSpeedYPerDirection[4] = { 0, 0, -32, 32 };

/* Octorok Rock data (03:598C-03:5997) */
static const int8_t OctorokRockOffsetXPerDirection[2] = { 8, -8 };
static const int8_t OctorokRockOffsetYPerDirection[4] = { 0, 0, -8, 8 };
static const int8_t OctorokRockSpeedXPerDirection[2] = { 32, -32 };
static const int8_t OctorokRockSpeedYPerDirection[4] = { 0, 0, -32, 32 };

/* ===== MoblinEntityHandler (03:5827) ===== */
void MoblinEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hMapId]; cp MAP_BOWWOW_HIDEOUT; jr nz, .hideoutEnd */
    if (gb_read_hram(gb, hMapId) == MAP_BOWWOW_HIDEOUT) {
        /* ld a, [wIsBowWowFollowingLink]; cp $80; jp nz, UnloadEntityAndReturn */
        if (gb_read(gb, wIsBowWowFollowingLink) != 0x80) {
            UnloadEntityAndReturn(gb, bc);
            return;
        }
    }

    /* .hideoutEnd: ld a, c; ld [wD153], a */
    gb_write(gb, wD153, bc & 0xFF);

    /* ld de, MoblinSpriteVariants; fallthrough to AnimateRoamingEnemy */
    AnimateRoamingEnemy(gb, bc);
}

/* ===== AnimateRoamingEnemy (03:583C) ===== */
void AnimateRoamingEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call RenderActiveEntitySpritesPair */
    RenderActiveEntitySpritesPair(gb, MoblinSpriteVariants, NULL);

    /* call ReturnIfNonInteractive_03 */
    if (ReturnIfNonInteractive_03(gb, false)) {
        return;
    }

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld a, [hl]; and a; jr z, .recoilEnd */
    if (gb_read(gb, wEntitiesIgnoreHitsCountdownTable + bc) == 0) {
        goto recoilEnd;
    }

    /* ld hl, wEntitiesStateTable; add hl, bc; ld a, $01; ld [hl], a */
    gb_write(gb, wEntitiesStateTable + bc, 0x01);
    /* ldh [hActiveEntityState], a */
    gb_write_hram(gb, hActiveEntityState, 0x01);
    /* call GetEntityTransitionCountdown; ld [hl], $40 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x40);

recoilEnd:
    /* call ApplyRecoilIfNeeded_03 */
    ApplyRecoilIfNeeded_03(gb, bc);

    /* call DefaultEnemyDamageCollisionHandler */
    DefaultEnemyDamageCollisionHandler(gb, bc);

    /* ldh a, [hActiveEntityState]; and a; jr z, RoamingEnemyState0Handler */
    if (gb_read_hram(gb, hActiveEntityState) == 0) {
        RoamingEnemyState0Handler(gb, bc);
        return;
    }

    /* call GetEntityTransitionCountdown; jr z, jr_003_5896 */
    uint8_t countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown == 0) {
        goto jr_003_5896;
    }

    /* cp $0A; jr nz, .projectileEnd */
    if (countdown != 0x0A) {
        goto projectileEnd;
    }

    /* call GetEntityPrivateCountdown1; jr nz, .projectileEnd */
    if (GetEntityPrivateCountdown1(gb, bc) != 0) {
        goto projectileEnd;
    }

    /* call GetEntityDirectionToLink_03 */
    uint8_t dir_to_link = GetEntityDirectionToLink_03(gb);
    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld a, e; cp [hl]; jr nz, .projectileEnd */
    if (dir_to_link != gb_read(gb, wEntitiesDirectionTable + bc)) {
        goto projectileEnd;
    }

    /* ldh a, [hActiveEntityType]; cp ENTITY_IRON_MASK; jr z, .projectileEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_IRON_MASK) {
        goto projectileEnd;
    }

    /* cp ENTITY_OCTOROK; jr z, jr_003_588D */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_OCTOROK) {
        goto jr_003_588D;
    }

    /* call SpawnMoblinArrow */
    SpawnMoblinArrow(gb, bc);

projectileEnd:
    /* call ApplyEntityInteractionWithBackground; ret */
    ApplyEntityInteractionWithBackground(gb, bc);
    return;

jr_003_588D:
    /* ld a, [wGameplayType]; cp GAMEPLAY_CREDITS; ret z */
    if (gb_read(gb, wGameplayType) == GAMEPLAY_CREDITS) {
        return;
    }

    /* jp SpawnOctorokRock */
    SpawnOctorokRock(gb, bc);
    return;

jr_003_5896: ;
    /* call GetRandomByte; and $1F; or $20; ld [hl], a */
    uint8_t random = (GetRandomByte(gb) & 0x1F) | 0x20;
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, random);

    /* ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $00 */
    gb_write(gb, wEntitiesStateTable + bc, 0x00);

    /* ld hl, wEntitiesPrivateState1Table; add hl, bc; ld a, [hl]; inc a */
    uint8_t private_state1 = gb_read(gb, wEntitiesPrivateState1Table + bc) + 1;

    /* and $03; ld [hl], a */
    private_state1 &= 0x03;
    gb_write(gb, wEntitiesPrivateState1Table + bc, private_state1);

    /* cp $00; jr nz, .jr_58B6 */
    if (private_state1 != 0) {
        goto jr_58B6;
    }

    /* call GetEntityDirectionToLink_03; jr jr_003_58B9 */
    dir_to_link = GetEntityDirectionToLink_03(gb);
    goto jr_003_58B9;

jr_58B6: ;
    /* call GetRandomByte */
    dir_to_link = GetRandomByte(gb);

jr_003_58B9: ;
    /* and $03; ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], a */
    dir_to_link &= 0x03;
    gb_write(gb, wEntitiesDirectionTable + bc, dir_to_link);

    /* ld e, a; ld d, b; ld hl, RoamingEnemySpeedXPerDirection; add hl, de; ld a, [hl] */
    int8_t speed_x = RoamingEnemySpeedXPerDirection[dir_to_link];
    int8_t speed_y = RoamingEnemySpeedYPerDirection[dir_to_link];

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)speed_x);
    /* ld hl, RoamingEnemySpeedYPerDirection; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesSpeedYTable; add hl, bc; ld [hl], a; ret */
    gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)speed_y);
}

/* ===== RoamingEnemyState0Handler (03:58D7) ===== */
void RoamingEnemyState0Handler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; and $0F; jr nz, .collided */
    if ((gb_read(gb, wEntitiesCollisionsTable + bc) & 0x0F) != 0) {
        goto collided;
    }

    /* call GetEntityTransitionCountdown; jr nz, StopWalkingEnd */
    if (GetEntityTransitionCountdown(gb, bc) != 0) {
        goto StopWalkingEnd;
    }

collided: ;
    /* call GetRandomByte; and $0F; or $10; ld [hl], a */
    uint8_t random = (GetRandomByte(gb) & 0x0F) | 0x10;
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, random);

    /* ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $01 */
    gb_write(gb, wEntitiesStateTable + bc, 0x01);
    /* call ClearEntitySpeed */
    ClearEntitySpeed(gb, bc);

StopWalkingEnd:
    /* call UpdateEntityPosWithSpeed_03; call ApplyEntityInteractionWithBackground; ret */
    UpdateEntityPosWithSpeed_03(gb, bc);
    ApplyEntityInteractionWithBackground(gb, bc);
}

/* ===== SetEntityVariantForDirection_03 (03:58FC) ===== */
void SetEntityVariantForDirection_03(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld e, [hl]; ld d, b */
    uint8_t direction = gb_read(gb, wEntitiesDirectionTable + bc);

    /* ld hl, EntityVariantForDirection_03; add hl, de; push hl */
    uint8_t variant = EntityVariantForDirection_03[direction & 0x03];

    /* ld hl, wEntitiesInertiaTable; add hl, bc; inc [hl] */
    uint8_t inertia = gb_read(gb, wEntitiesInertiaTable + bc) + 1;
    gb_write(gb, wEntitiesInertiaTable + bc, inertia);

    /* ld a, [hl]; rra x3; pop hl; and $01; or [hl] */
    uint8_t inertia_bit = (inertia >> 3) & 0x01;
    variant |= inertia_bit;

    /* jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, variant);
}

/* ===== SpawnMoblinArrow (03:5947) ===== */
void SpawnMoblinArrow(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, ENTITY_MOBLIN_ARROW; call SpawnNewEntity; jr c, ret_003_598B */
    uint16_t de = SpawnNewEntity_trampoline(gb, ENTITY_MOBLIN_ARROW, NULL);
    if (de == 0xFFFF) {
        return;
    }

    /* push bc; ldh a, [hMultiPurpose2]; ld c, a */
    uint8_t multi_purpose2 = gb_read_hram(gb, hMultiPurpose2);

    /* ld hl, MoblinArrowOffsetXPerDirection; add hl, bc; ldh a, [hMultiPurpose0]; add [hl] */
    int8_t offset_x = MoblinArrowOffsetXPerDirection[multi_purpose2 & 0x03];
    uint8_t pos_x = gb_read_hram(gb, hMultiPurpose0);
    gb_write(gb, wEntitiesPosXTable + de, (uint8_t)(pos_x + offset_x));

    /* ld hl, MoblinArrowOffsetYPerDirection; add hl, bc; ldh a, [hMultiPurpose1]; add [hl] */
    int8_t offset_y = MoblinArrowOffsetYPerDirection[multi_purpose2 & 0x03];
    uint8_t pos_y = gb_read_hram(gb, hMultiPurpose1);
    gb_write(gb, wEntitiesPosYTable + de, (uint8_t)(pos_y + offset_y));

    /* ld hl, MoblinArrowSpeedXPerDirection; add hl, bc; ld a, [hl] */
    int8_t speed_x = MoblinArrowSpeedXPerDirection[multi_purpose2 & 0x03];
    gb_write(gb, wEntitiesSpeedXTable + de, (uint8_t)speed_x);

    /* ld hl, MoblinArrowSpeedYPerDirection; add hl, bc; ld a, [hl] */
    int8_t speed_y = MoblinArrowSpeedYPerDirection[multi_purpose2 & 0x03];
    gb_write(gb, wEntitiesSpeedYTable + de, (uint8_t)speed_y);

    /* ldh a, [hMultiPurpose2]; ld hl, wEntitiesSpriteVariantTable; add hl, de; ld [hl], a */
    gb_write(gb, wEntitiesSpriteVariantTable + de, multi_purpose2);
    /* ld hl, wEntitiesDirectionTable; add hl, de; ld [hl], a */
    gb_write(gb, wEntitiesDirectionTable + de, multi_purpose2);

    /* pop bc; ret */
}

/* ===== SpawnOctorokRock (03:5998) ===== */
void SpawnOctorokRock(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, ENTITY_OCTOROK_ROCK; call SpawnNewEntity; jr c, .return */
    uint16_t de = SpawnNewEntity_trampoline(gb, ENTITY_OCTOROK_ROCK, NULL);
    if (de == 0xFFFF) {
        return;
    }

    /* push bc; ldh a, [hMultiPurpose2]; ld hl, wEntitiesDirectionTable; add hl, de; ld [hl], a */
    uint8_t multi_purpose2 = gb_read_hram(gb, hMultiPurpose2);
    gb_write(gb, wEntitiesDirectionTable + de, multi_purpose2);

    /* ld c, a; ld hl, OctorokRockOffsetXPerDirection; add hl, bc */
    int8_t offset_x = OctorokRockOffsetXPerDirection[multi_purpose2 & 0x01];
    uint8_t pos_x = gb_read_hram(gb, hMultiPurpose0);
    gb_write(gb, wEntitiesPosXTable + de, (uint8_t)(pos_x + offset_x));

    /* ldh a, [hMultiPurpose0]; add [hl]; ld hl, wEntitiesPosXTable; add hl, de; ld [hl], a */
    int8_t offset_y = OctorokRockOffsetYPerDirection[multi_purpose2 & 0x03];
    uint8_t pos_y = gb_read_hram(gb, hMultiPurpose1);
    gb_write(gb, wEntitiesPosYTable + de, (uint8_t)(pos_y + offset_y));

    /* ld hl, OctorokRockSpeedXPerDirection; add hl, bc; ld a, [hl] */
    int8_t speed_x = OctorokRockSpeedXPerDirection[multi_purpose2 & 0x01];
    gb_write(gb, wEntitiesSpeedXTable + de, (uint8_t)speed_x);

    /* ld hl, OctorokRockSpeedYPerDirection; add hl, bc; ld a, [hl] */
    int8_t speed_y = OctorokRockSpeedYPerDirection[multi_purpose2 & 0x03];
    gb_write(gb, wEntitiesSpeedYTable + de, (uint8_t)speed_y);

    /* pop bc; and a; .return: ret */
}