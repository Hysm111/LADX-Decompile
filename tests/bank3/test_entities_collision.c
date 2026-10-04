#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "gb.h"
#include "bank3/entities_collision.h"
#include "bank3/entities_physics.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/inventory.h"
#include "constants/sfx.h"
#include "constants/audio.h"

/* Test EntityDamagesForGroup table values */
static void test_EntityDamagesForGroup(void) {
    printf("Testing EntityDamagesForGroup table...\n");

    assert(sizeof(EntityDamagesForGroup) == 53);
    assert(EntityDamagesForGroup[0x00] == 0x04);
    assert(EntityDamagesForGroup[0x02] == 0x08);
    assert(EntityDamagesForGroup[0x04] == 0x18);
    assert(EntityDamagesForGroup[0x15] == 0x0C);
    assert(EntityDamagesForGroup[0x16] == 0x00);
    assert(EntityDamagesForGroup[0x1F] == 0x20);
    assert(EntityDamagesForGroup[0x34] == 0x08);

    printf("  PASSED\n");
}

/* Test ApplyLinkCollisionWithEnemy: Cheep-Cheep Jumping */
static void test_ApplyLinkCollision_CheepCheep(void) {
    printf("Testing ApplyLinkCollisionWithEnemy (Cheep-Cheep)...\n");

    GBState gb;
    gb_init(&gb);
    uint16_t bc = 0x02;
    gb_write(&gb, wActiveEntityIndex, bc);

    gb_write_hram(&gb, hActiveEntityType, ENTITY_CHEEP_CHEEP_JUMPING);
    gb_write_hram(&gb, hActiveEntityPosY, 0x40);
    gb_write(&gb, wEntitiesPosYTable + bc, 0x40);
    gb_write(&gb, wEntitiesPosZTable + bc, 0x00);

    /* Case 1: Link is above Cheep-Cheep (Y = 0x20 -> DIRECTION_UP) */
    gb_write_hram(&gb, hLinkPositionY, 0x20);
    gb_write(&gb, wEntitiesStateTable + bc, 0x01);
    gb_write(&gb, wIsLinkInTheAir, 0x00);
    gb_write_hram(&gb, hLinkSpeedY, 0x00);
    gb_write_hram(&gb, hWaveSfx, 0x00);

    ApplyLinkCollisionWithEnemy(&gb, bc);

    assert(gb_read(&gb, wEntitiesStateTable + bc) == ENTITY_STATUS_ACTIVE);
    assert(gb_read(&gb, wIsLinkInTheAir) == 0x02);
    assert(gb_read_hram(&gb, hLinkSpeedY) == 0xF0);
    assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_FLOOR_SWITCH);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0); /* No damage taken */

    printf("  PASSED\n");
}

/* Test ApplyLinkCollisionWithEnemy: Goomba Stomp and Damage */
static void test_ApplyLinkCollision_Goomba(void) {
    printf("Testing ApplyLinkCollisionWithEnemy (Goomba)...\n");

    GBState gb;
    gb_init(&gb);
    uint16_t bc = 0x01;

    gb_write_hram(&gb, hActiveEntityType, ENTITY_GOOMBA);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00); /* 4 damage */

    /* Case 1: Airborne and falling in top-down mode (velocity Z negative = falling) */
    gb_write(&gb, wIsLinkInTheAir, 0x01);
    gb_write_hram(&gb, hIsSideScrolling, 0x00);
    gb_write_hram(&gb, hLinkVelocityZ, 0xF8); /* Falling */
    gb_write_hram(&gb, hLinkCountdown, 0x00);
    gb_write(&gb, wEntitiesStateTable + bc, 0x01);

    ApplyLinkCollisionWithEnemy(&gb, bc);

    assert(gb_read_hram(&gb, hLinkCountdown) == 0x02);
    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x02);
    assert(gb_read(&gb, wEntitiesTransitionCountdownTable + bc) == 0x30);
    assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_FLOOR_SWITCH);
    assert(gb_read_hram(&gb, hLinkVelocityZ) == 0x10);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0); /* No damage */

    /* Case 2: Airborne and falling in side-scrolling mode (speed Y >= 0 = falling) */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_GOOMBA);
    gb_write(&gb, wIsLinkInTheAir, 0x01);
    gb_write_hram(&gb, hIsSideScrolling, 0x01);
    gb_write_hram(&gb, hLinkSpeedY, 0x08); /* Falling */
    gb_write_hram(&gb, hLinkCountdown, 0x00);

    ApplyLinkCollisionWithEnemy(&gb, bc);

    assert(gb_read_hram(&gb, hLinkCountdown) == 0x02);
    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x02);
    assert(gb_read_hram(&gb, hLinkSpeedY) == 0xF0);

    /* Case 3: On ground (not airborne) -> Link takes damage */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_GOOMBA);
    gb_write(&gb, wIsLinkInTheAir, 0x00);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00); /* 4 damage */

    ApplyLinkCollisionWithEnemy(&gb, bc);

    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x04);
    assert(gb_read(&gb, wInvincibilityCounter) == 0x50);
    assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_LINK_HURT);

    /* Case 4: Airborne but rising in top-down mode (velocity Z positive = rising) -> takes damage */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_GOOMBA);
    gb_write(&gb, wIsLinkInTheAir, 0x01);
    gb_write_hram(&gb, hIsSideScrolling, 0x00);
    gb_write_hram(&gb, hLinkVelocityZ, 0x18); /* Rising */
    gb_write_hram(&gb, hLinkCountdown, 0x00);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00);

    ApplyLinkCollisionWithEnemy(&gb, bc);

    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x04);

    printf("  PASSED\n");
}

/* Test ApplyLinkCollisionWithEnemy: Gel, Cue Ball, Rolling Bones Bar, Moblin King */
static void test_ApplyLinkCollision_SpecialEntities(void) {
    printf("Testing ApplyLinkCollisionWithEnemy (Gel, Cue Ball, Moblin King)...\n");

    GBState gb;
    gb_init(&gb);
    uint16_t bc = 0x03;

    /* Case 1: Gel latch */
    gb_write_hram(&gb, hActiveEntityType, ENTITY_GEL);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wEntitiesTransitionCountdownTable + bc) == 0x80);
    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x04);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 2: Ignore collision countdown active blocks normal enemy damage */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0x0A);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 3: Cue Ball bypasses ignore collisions countdown */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_CUE_BALL);
    gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0x0A);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x04);

    /* Case 4: Rolling Bones Bar bypasses ignore collisions countdown */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_ROLLING_BONES_BAR);
    gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0x0A);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x04);

    /* Case 5: Moblin King in state 4 */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_MOBLIN_KING);
    gb_write_hram(&gb, hActiveEntityState, 0x04);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x08);
    assert(gb_read_hram(&gb, hWaveSfx) == WAVE_SFX_LINK_HURT);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    printf("  PASSED\n");
}

/* Test ApplyLinkCollisionWithEnemy: Immunity and Damage Calculations */
static void test_ApplyLinkCollision_DamageCalculations(void) {
    printf("Testing ApplyLinkCollisionWithEnemy (Immunity & Damage Math)...\n");

    GBState gb;
    uint16_t bc = 0x02;

    /* Case 1: Invincibility counter active */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wInvincibilityCounter, 0x20);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x04); /* 0x18 = 24 */
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 2: Ocarina playing active */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wLinkPlayingOcarinaCountdown, 0x10);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 3: Got item dialog active */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wDialogGotItem, 0x01);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 4: Collision immunity flag active */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wIsLinkImmuneToCollisionDamage, 0x01);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 5: Standard green tunic, nominal damage (group 4 = 0x18 = 24 damage) */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x04);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 24);
    assert(gb_read(&gb, wInvincibilityCounter) == 0x50);
    assert(gb_read(&gb, wGuardianAcornCounter) == 0x00);

    /* Case 6: Blue Tunic halves damage (24 -> 12) */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wTunicType, TUNIC_BLUE);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x04);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 12);

    /* Case 7: Guardian Acorn: damage == 4 -> 0 damage */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wActivePowerUp, ACTIVE_POWER_UP_GUARDIAN_ACORN);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00); /* 4 damage */
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0);

    /* Case 8: Guardian Acorn: damage != 4 (e.g. 24) -> halved to 12 */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wActivePowerUp, ACTIVE_POWER_UP_GUARDIAN_ACORN);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x04); /* 24 damage */
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 12);

    /* Case 9: Power-up hits accumulation and loss after 3 hits */
    gb_init(&gb);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write(&gb, wActivePowerUp, ACTIVE_POWER_UP_PIECE_OF_POWER);
    gb_write(&gb, wPowerUpHits, 0x01);
    gb_write_hram(&gb, hDefaultMusicTrack, MUSIC_OVERWORLD);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wPowerUpHits) == 0x02);
    assert(gb_read(&gb, wActivePowerUp) == ACTIVE_POWER_UP_PIECE_OF_POWER);

    /* 3rd hit drops power-up and restores music track */
    gb_write(&gb, wInvincibilityCounter, 0x00); /* Clear invincibility for next hit */
    gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0x00);
    ApplyLinkCollisionWithEnemy(&gb, bc);
    assert(gb_read(&gb, wPowerUpHits) == 0x03);
    assert(gb_read(&gb, wActivePowerUp) == 0x00);
    assert(gb_read(&gb, wMusicTrackToPlay) == MUSIC_OVERWORLD);
    assert(gb_read_hram(&gb, hNextDefaultMusicTrack) == MUSIC_OVERWORLD);

    printf("  PASSED\n");
}

/* Test DefaultEnemyDamageCollisionHandler alternating parity and func_003_6E2B */
static void test_DefaultEnemyDamageCollisionHandler_Parity(void) {
    printf("Testing DefaultEnemyDamageCollisionHandler parity & dispatch...\n");

    GBState gb;
    gb_init(&gb);
    uint16_t bc = 0x01;

    /* Setup entity hitbox at (0x40, 0x40) */
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write_hram(&gb, hActiveEntityPosX, 0x40);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x40);
    uint16_t hb = (uint16_t)(wEntitiesHitboxPositionTable + (bc * 4));
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x04);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x04);

    /* Link overlapping entity */
    gb_write_hram(&gb, hLinkPositionX, 0x38);
    gb_write_hram(&gb, hLinkPositionY, 0x38);

    /* Odd parity: hFrameCounter (0) ^ c (1) = 1 (bit 0 set) -> CheckLinkCollisionWithEnemy runs */
    gb_write_hram(&gb, hFrameCounter, 0x00);
    gb_write(&gb, wInvincibilityCounter, 0x00);
    gb_write(&gb, wEntitiesHealthGroup + bc, 0x00);
    DefaultEnemyDamageCollisionHandler(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x04);
    assert(gb_read(&gb, wInvincibilityCounter) == 0x50);

    /* Even parity: hFrameCounter (1) ^ c (1) = 0 (bit 0 clear) -> CheckLinkCollisionWithEnemy is skipped */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_OCTOROK);
    gb_write_hram(&gb, hActiveEntityPosX, 0x40);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x40);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x04);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x04);
    gb_write_hram(&gb, hLinkPositionX, 0x38);
    gb_write_hram(&gb, hLinkPositionY, 0x38);
    gb_write_hram(&gb, hFrameCounter, 0x01);
    gb_write(&gb, wInvincibilityCounter, 0x00);
    DefaultEnemyDamageCollisionHandler(&gb, bc);
    assert(gb_read(&gb, wSubtractHealthBuffer) == 0x00); /* Skipped */

    printf("  PASSED\n");
}

/* Test func_003_6E2B: Hitbox and Weapon Collision Branches */
static void test_func_003_6E2B_Branches(void) {
    printf("Testing func_003_6E2B branches...\n");

    GBState gb;
    uint16_t bc = 0x02;

    /* Case 1: Weapon inactive (wC140 == 0) -> early return */
    gb_init(&gb);
    gb_write(&gb, wC140, 0x00);
    func_003_6E2B(&gb, bc);
    assert(gb_read_hram(&gb, hJingle) == 0);

    /* Case 2: Flashing enemy (flash countdown < 0x18) -> early return */
    gb_init(&gb);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wEntitiesFlashCountdownTable + bc, 0x10);
    func_003_6E2B(&gb, bc);
    assert(gb_read_hram(&gb, hJingle) == 0);

    /* Case 3: Already processed hit this frame (wC1AC - 1 == bc) -> early return */
    gb_init(&gb);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC1AC, bc + 1);
    func_003_6E2B(&gb, bc);
    assert(gb_read_hram(&gb, hJingle) == 0);

    /* Case 4: Ignoring hits countdown != 0 -> early return */
    gb_init(&gb);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x05);
    func_003_6E2B(&gb, bc);
    assert(gb_read_hram(&gb, hJingle) == 0);

    /* Setup overlapping weapon and entity hitboxes for entity tests */
    /* Weapon at X = 0x40, radius = 0x08, Y = 0x40, radius = 0x08 */
    /* Entity at X = 0x42, radius = 0x06, Y = 0x42, radius = 0x06 */
    uint16_t hb = (uint16_t)(wEntitiesHitboxPositionTable + (bc * 4));

    /* Case 5: Flame Shooter + Level 2 shield + direction UP -> blocks fire */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_FLAME_SHOOTER);
    gb_write_hram(&gb, hActiveEntityPosX, 0x42);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x42);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x06);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x06);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC141, 0x08);
    gb_write(&gb, wC142, 0x40);
    gb_write(&gb, wC143, 0x08);
    gb_write(&gb, wShieldLevel, 0x02);
    gb_write_hram(&gb, hLinkDirection, DIRECTION_UP);

    func_003_6E2B(&gb, bc);

    assert(gb_read_hram(&gb, hLinkSpeedY) == 0x04);
    assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 0x08);
    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x01);

    /* Case 6: Bouncing Bombite in state 2 -> reverses speed */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_BOUNCING_BOMBITE);
    gb_write_hram(&gb, hActiveEntityState, 0x02);
    gb_write_hram(&gb, hActiveEntityPosX, 0x42);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x42);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x06);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x06);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC141, 0x08);
    gb_write(&gb, wC142, 0x40);
    gb_write(&gb, wC143, 0x08);
    gb_write(&gb, wEntitiesSpeedXTable + bc, 0x10);
    gb_write(&gb, wEntitiesSpeedYTable + bc, (uint8_t)-8);

    func_003_6E2B(&gb, bc);

    assert(gb_read(&gb, wEntitiesSpeedXTable + bc) == (uint8_t)-16);
    assert(gb_read(&gb, wEntitiesSpeedYTable + bc) == 0x08);
    assert(gb_read(&gb, wEntitiesTransitionCountdownTable + bc) == 0x40);
    assert(gb_read(&gb, wEntitiesPrivateCountdown1Table + bc) == 0x08);

    /* Case 7: Knight with SWORD_CLINK_OFF -> clink spark */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_KNIGHT);
    gb_write(&gb, wEntitiesOptions1Table + bc, ENTITY_OPT1_SWORD_CLINK_OFF);
    gb_write_hram(&gb, hActiveEntityPosX, 0x42);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x42);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x06);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x06);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC141, 0x08);
    gb_write(&gb, wC142, 0x40);
    gb_write(&gb, wC143, 0x08);
    gb_write(&gb, wEntitiesPrivateState1Table + bc, 0x05);
    gb_write(&gb, wSwordCharge, 0x20);

    func_003_6E2B(&gb, bc);

    assert(gb_read(&gb, wEntitiesPrivateState1Table + bc) == (uint8_t)-5);
    assert(gb_read(&gb, wEntitiesPrivateCountdown1Table + bc) == 0x0C);
    assert(gb_read(&gb, wC160) == 0x01);
    assert(gb_read(&gb, wSwordCharge) == 0x00);
    assert(gb_read_hram(&gb, hMultiPurpose0) == 0x42);
    assert(gb_read_hram(&gb, hMultiPurpose1) == 0x42);

    /* Case 8: Spiked Beetle flipped on hit */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_SPIKED_BEETLE);
    gb_write_hram(&gb, hActiveEntityPosX, 0x42);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x42);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x06);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x06);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC141, 0x08);
    gb_write(&gb, wC142, 0x40);
    gb_write(&gb, wC143, 0x08);
    gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);

    func_003_6E2B(&gb, bc);

    assert(gb_read(&gb, wEntitiesStateTable + bc) == 0x03);
    assert(gb_read(&gb, wEntitiesSpeedZTable + bc) == 0x20);
    assert(gb_read(&gb, wEntitiesTransitionCountdownTable + bc) == 0xFF);
    assert(gb_read(&gb, wEntitiesSpeedXTable + bc) == 0x10);
    assert(gb_read(&gb, wEntitiesSpeedYTable + bc) == 0x00);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + bc) == 0x00);

    /* Case 9: Pairodd projectile sets collisions table to 0xFF */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, bc);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_PAIRODD_PROJECTILE);
    gb_write_hram(&gb, hActiveEntityPosX, 0x42);
    gb_write_hram(&gb, hActiveEntityVisualPosY, 0x42);
    gb_write(&gb, hb + 0, 0x00);
    gb_write(&gb, hb + 1, 0x06);
    gb_write(&gb, hb + 2, 0x00);
    gb_write(&gb, hb + 3, 0x06);
    gb_write(&gb, wC140, 0x40);
    gb_write(&gb, wC141, 0x08);
    gb_write(&gb, wC142, 0x40);
    gb_write(&gb, wC143, 0x08);

    func_003_6E2B(&gb, bc);

    assert(gb_read(&gb, wEntitiesCollisionsTable + bc) == 0xFF);

    printf("  PASSED\n");
}

void test_bank3_entities_collision(void) {
    test_EntityDamagesForGroup();
    test_ApplyLinkCollision_CheepCheep();
    test_ApplyLinkCollision_Goomba();
    test_ApplyLinkCollision_SpecialEntities();
    test_ApplyLinkCollision_DamageCalculations();
    test_DefaultEnemyDamageCollisionHandler_Parity();
    test_func_003_6E2B_Branches();
}
