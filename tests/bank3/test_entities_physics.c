#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "gb.h"
#include "bank3/entities_physics.h"
#include "bank3/entities_collision.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/directions.h"

/* Test GetEntityXDistanceToLink_03 */
static void test_GetEntityXDistanceToLink(void) {
    printf("Testing GetEntityXDistanceToLink_03...\n");

    GBState gb;
    gb_init(&gb);

    /* Entity 2 at X = 0x40 */
    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write(&gb, wEntitiesPosXTable + 0x02, 0x40);

    uint8_t dir = 0xFF;
    uint8_t diff = 0xFF;

    /* Case 1: Link is to the right (X = 0x60) */
    gb_write_hram(&gb, hLinkPositionX, 0x60);
    GetEntityXDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_RIGHT);
    assert(diff == 0x20);

    /* Case 2: Link is to the left (X = 0x20) */
    gb_write_hram(&gb, hLinkPositionX, 0x20);
    GetEntityXDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_LEFT);
    assert(diff == 0xE0); /* (uint8_t)(0x20 - 0x40) = 0xE0 */

    /* Case 3: Link is at same position (X = 0x40) */
    gb_write_hram(&gb, hLinkPositionX, 0x40);
    GetEntityXDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_RIGHT);
    assert(diff == 0x00);

    /* Case 4: Test _idx variant with entity 5 at X = 0x80 */
    gb_write(&gb, wEntitiesPosXTable + 0x05, 0x80);
    gb_write_hram(&gb, hLinkPositionX, 0x90);
    GetEntityXDistanceToLink_03_idx(&gb, 0x05, &dir, &diff);
    assert(dir == DIRECTION_RIGHT);
    assert(diff == 0x10);

    printf("  PASSED\n");
}

/* Test GetEntityYDistanceToLink_03 */
static void test_GetEntityYDistanceToLink(void) {
    printf("Testing GetEntityYDistanceToLink_03...\n");

    GBState gb;
    gb_init(&gb);

    /* Entity 3 at Y = 0x50, Z = 0 */
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write(&gb, wEntitiesPosYTable + 0x03, 0x50);
    gb_write(&gb, wEntitiesPosZTable + 0x03, 0x00);

    uint8_t dir = 0xFF;
    uint8_t diff = 0xFF;

    /* Case 1: Link is below (Y = 0x70) */
    gb_write_hram(&gb, hLinkPositionY, 0x70);
    GetEntityYDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_DOWN);
    assert(diff == 0x20);

    /* Case 2: Link is above (Y = 0x30) */
    gb_write_hram(&gb, hLinkPositionY, 0x30);
    GetEntityYDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_UP);
    assert(diff == 0xE0); /* (uint8_t)(0x30 - 0x50) = 0xE0 */

    /* Case 3: Link at same Y (Y = 0x50) */
    gb_write_hram(&gb, hLinkPositionY, 0x50);
    GetEntityYDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_DOWN);
    assert(diff == 0x00);

    /* Case 4: With entity altitude Z = 0x08 */
    /* link_y = 0x4A, ent_y = 0x50, ent_z = 0x08 -> diff = (0x4A - 0x50) + 0x08 = 0xFA + 0x08 = 0x02 */
    gb_write_hram(&gb, hLinkPositionY, 0x4A);
    gb_write(&gb, wEntitiesPosZTable + 0x03, 0x08);
    GetEntityYDistanceToLink_03(&gb, &dir, &diff);
    assert(dir == DIRECTION_DOWN);
    assert(diff == 0x02);

    printf("  PASSED\n");
}

/* Test GetEntityDirectionToLink_03 */
static void test_GetEntityDirectionToLink(void) {
    printf("Testing GetEntityDirectionToLink_03...\n");

    GBState gb;
    gb_init(&gb);

    /* Entity 1 at (0x40, 0x40) */
    gb_write(&gb, wActiveEntityIndex, 0x01);
    gb_write(&gb, wEntitiesPosXTable + 0x01, 0x40);
    gb_write(&gb, wEntitiesPosYTable + 0x01, 0x40);
    gb_write(&gb, wEntitiesPosZTable + 0x01, 0x00);

    /* Case 1: Link is to the right (dx = +0x30, dy = +0x10) */
    gb_write_hram(&gb, hLinkPositionX, 0x70);
    gb_write_hram(&gb, hLinkPositionY, 0x50);
    uint8_t dir = GetEntityDirectionToLink_03(&gb);
    assert(dir == DIRECTION_RIGHT);
    assert(gb_read_hram(&gb, hMultiPurpose0) == DIRECTION_RIGHT);
    assert(gb_read_hram(&gb, hMultiPurpose1) == DIRECTION_DOWN);

    /* Case 2: Link is to the left (dx = -0x30, dy = -0x10) */
    gb_write_hram(&gb, hLinkPositionX, 0x10);
    gb_write_hram(&gb, hLinkPositionY, 0x30);
    dir = GetEntityDirectionToLink_03(&gb);
    assert(dir == DIRECTION_LEFT);

    /* Case 3: Link is predominantly down (dx = +0x10, dy = +0x30) */
    gb_write_hram(&gb, hLinkPositionX, 0x50);
    gb_write_hram(&gb, hLinkPositionY, 0x70);
    dir = GetEntityDirectionToLink_03(&gb);
    assert(dir == DIRECTION_DOWN);

    /* Case 4: Link is predominantly up (dx = -0x10, dy = -0x30) */
    gb_write_hram(&gb, hLinkPositionX, 0x30);
    gb_write_hram(&gb, hLinkPositionY, 0x10);
    dir = GetEntityDirectionToLink_03(&gb);
    assert(dir == DIRECTION_UP);

    /* Case 5: 45-degree diagonal (dx = 0x20, dy = 0x20) -> vertical priority per assembly */
    gb_write_hram(&gb, hLinkPositionX, 0x60);
    gb_write_hram(&gb, hLinkPositionY, 0x60);
    dir = GetEntityDirectionToLink_03(&gb);
    assert(dir == DIRECTION_DOWN);

    printf("  PASSED\n");
}

/* Test GetVectorTowardsLink and ApplyVectorTowardsLink */
static void test_GetVectorTowardsLink(void) {
    printf("Testing GetVectorTowardsLink...\n");

    GBState gb;
    gb_init(&gb);

    /* Entity 0 at (0x40, 0x40) */
    gb_write(&gb, wActiveEntityIndex, 0x00);
    gb_write(&gb, wEntitiesPosXTable, 0x40);
    gb_write(&gb, wEntitiesPosYTable, 0x40);
    gb_write(&gb, wEntitiesPosZTable, 0x00);

    /* Case 1: Zero length cancel */
    uint8_t vx = 0xFF, vy = 0xFF;
    GetVectorTowardsLink_with_length(&gb, 0x00, &vy, &vx);
    assert(vy == 0x00);
    assert(vx == 0x00);
    assert(gb_read_hram(&gb, hMultiPurpose0) == 0x00);

    /* Case 2: Pure horizontal right (dx = 0x20, dy = 0) */
    gb_write_hram(&gb, hLinkPositionX, 0x60);
    gb_write_hram(&gb, hLinkPositionY, 0x40);
    GetVectorTowardsLink_with_length(&gb, 0x18, &vy, &vx);
    assert(vx == 0x18);
    assert(vy == 0x00);

    /* Case 3: Pure horizontal left (dx = -0x20, dy = 0) */
    gb_write_hram(&gb, hLinkPositionX, 0x20);
    gb_write_hram(&gb, hLinkPositionY, 0x40);
    GetVectorTowardsLink_with_length(&gb, 0x18, &vy, &vx);
    assert(vx == (uint8_t)(-0x18));
    assert(vy == 0x00);

    /* Case 4: Pure vertical down (dx = 0, dy = 0x20) */
    gb_write_hram(&gb, hLinkPositionX, 0x40);
    gb_write_hram(&gb, hLinkPositionY, 0x60);
    GetVectorTowardsLink_with_length(&gb, 0x10, &vy, &vx);
    assert(vx == 0x00);
    assert(vy == 0x10);

    /* Case 5: Pure vertical up (dx = 0, dy = -0x20) */
    gb_write_hram(&gb, hLinkPositionX, 0x40);
    gb_write_hram(&gb, hLinkPositionY, 0x20);
    GetVectorTowardsLink_with_length(&gb, 0x10, &vy, &vx);
    assert(vx == 0x00);
    assert(vy == (uint8_t)(-0x10));

    /* Case 6: 45-degree angle (dx = 0x20, dy = 0x20, length = 0x10) */
    gb_write_hram(&gb, hLinkPositionX, 0x60);
    gb_write_hram(&gb, hLinkPositionY, 0x60);
    GetVectorTowardsLink_with_length(&gb, 0x10, &vy, &vx);
    assert(vx == 0x10);
    assert(vy == 0x10);

    /* Case 7: ApplyVectorTowardsLink */
    ApplyVectorTowardsLink(&gb, 0x00);
    assert(gb_read(&gb, wEntitiesSpeedXTable) == 0x10);
    assert(gb_read(&gb, wEntitiesSpeedYTable) == 0x10);

    printf("  PASSED\n");
}

/* Test AddEntitySpeedToPos_03 */
static void test_AddEntitySpeedToPos(void) {
    printf("Testing AddEntitySpeedToPos_03...\n");

    GBState gb;
    gb_init(&gb);

    /* Setup entity 1 */
    uint16_t bc = 0x01;
    gb_write(&gb, wEntitiesPosXTable + bc, 0x40);
    gb_write(&gb, wEntitiesSpeedXAccTable + bc, 0x00);

    /* Case 1: Speed = 0 does nothing */
    gb_write(&gb, wEntitiesSpeedXTable + bc, 0x00);
    AddEntitySpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesPosXTable + bc) == 0x40);
    assert(gb_read(&gb, wEntitiesSpeedXAccTable + bc) == 0x00);

    /* Case 2: Speed = 0x08 (+0.5 px/frame) */
    /* Frame 1: frac = 0x80, acc becomes 0x80, carry = 0, int_part = 0 -> pos = 0x40 */
    gb_write(&gb, wEntitiesSpeedXTable + bc, 0x08);
    AddEntitySpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesSpeedXAccTable + bc) == 0x80);
    assert(gb_read(&gb, wEntitiesPosXTable + bc) == 0x40);

    /* Frame 2: frac = 0x80, acc overflows to 0x00, carry = 1, int_part = 0 -> pos = 0x41 */
    AddEntitySpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesSpeedXAccTable + bc) == 0x00);
    assert(gb_read(&gb, wEntitiesPosXTable + bc) == 0x41);

    /* Case 3: Speed = 0xF0 (-1.0 px/frame = -16 sixteenths) */
    gb_write(&gb, wEntitiesSpeedXTable + bc, 0xF0);
    AddEntitySpeedToPos_03(&gb, bc);
    /* frac = 0x00, acc remains 0x00, carry = 0, int_part = 0xFF (-1) -> pos = 0x40 */
    assert(gb_read(&gb, wEntitiesSpeedXAccTable + bc) == 0x00);
    assert(gb_read(&gb, wEntitiesPosXTable + bc) == 0x40);

    printf("  PASSED\n");
}

/* Test AddEntityZSpeedToPos_03 */
static void test_AddEntityZSpeedToPos(void) {
    printf("Testing AddEntityZSpeedToPos_03...\n");

    GBState gb;
    gb_init(&gb);

    uint16_t bc = 0x02;
    gb_write(&gb, wEntitiesPosZTable + bc, 0x10);
    gb_write(&gb, wEntitiesSpeedZAccTable + bc, 0x00);

    /* Case 1: Speed = 0 does nothing */
    gb_write(&gb, wEntitiesSpeedZTable + bc, 0x00);
    AddEntityZSpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesPosZTable + bc) == 0x10);

    /* Case 2: Speed = 0x10 (+1.0 px/frame) */
    gb_write(&gb, wEntitiesSpeedZTable + bc, 0x10);
    AddEntityZSpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesPosZTable + bc) == 0x11);

    /* Case 3: Speed = 0xF0 (-1.0 px/frame) */
    gb_write(&gb, wEntitiesSpeedZTable + bc, 0xF0);
    AddEntityZSpeedToPos_03(&gb, bc);
    assert(gb_read(&gb, wEntitiesPosZTable + bc) == 0x10);

    printf("  PASSED\n");
}

/* Test UpdateEntityPosWithSpeed_03 */
static void test_UpdateEntityPosWithSpeed(void) {
    printf("Testing UpdateEntityPosWithSpeed_03...\n");

    GBState gb;
    gb_init(&gb);

    uint16_t bc = 0x03;
    gb_write(&gb, wEntitiesPosXTable + bc, 0x20);
    gb_write(&gb, wEntitiesPosYTable + bc, 0x30);
    gb_write(&gb, wEntitiesSpeedXAccTable + bc, 0x00);
    gb_write(&gb, wEntitiesSpeedYAccTable + bc, 0x00);

    /* Set speed X = +1 px (0x10), speed Y = -1 px (0xF0) */
    gb_write(&gb, wEntitiesSpeedXTable + bc, 0x10);
    gb_write(&gb, wEntitiesSpeedYTable + bc, 0xF0);

    UpdateEntityPosWithSpeed_03(&gb, bc);

    assert(gb_read(&gb, wEntitiesPosXTable + bc) == 0x21);
    assert(gb_read(&gb, wEntitiesPosYTable + bc) == 0x2F);

    printf("  PASSED\n");
}

/* Test StartIgnoringHitsForEntity and ConfigureEntityRecoil */
static void test_RecoilAndIgnoringHits(void) {
    printf("Testing ConfigureEntityRecoil & StartIgnoringHitsForEntity...\n");

    GBState gb;
    gb_init(&gb);

    uint16_t bc = 0x04;
    gb_write(&gb, wActiveEntityIndex, bc);

    /* Test StartIgnoringHitsForEntity */
    gb_write(&gb, wEntitiesPowerRecoilingTable + bc, 0xFF);
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x00);

    StartIgnoringHitsForEntity(&gb);

    assert(gb_read(&gb, wEntitiesPowerRecoilingTable + bc) == 0x00);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + bc) == 0x0A);

    /* Test ConfigureEntityRecoil */
    /* Position Link right and below entity: (dx = 0x20, dy = 0x20) */
    gb_write(&gb, wEntitiesPosXTable + bc, 0x30);
    gb_write(&gb, wEntitiesPosYTable + bc, 0x30);
    gb_write(&gb, wEntitiesPosZTable + bc, 0x00);
    gb_write_hram(&gb, hLinkPositionX, 0x50);
    gb_write_hram(&gb, hLinkPositionY, 0x50);

    ConfigureEntityRecoil(&gb, bc, 0x20);

    /* Vector towards link is (+0x20, +0x20). Recoil should be negated: (-0x20, -0x20) = (0xE0, 0xE0) */
    assert(gb_read(&gb, wEntitiesRecoilVelocityX + bc) == 0xE0);
    assert(gb_read(&gb, wEntitiesRecoilVelocityY + bc) == 0xE0);
    assert(gb_read(&gb, wEntitiesPowerRecoilingTable + bc) == 0x00);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + bc) == 0x0A);

    printf("  PASSED\n");
}

void test_bank3_entities_physics(void) {
    test_GetEntityXDistanceToLink();
    test_GetEntityYDistanceToLink();
    test_GetEntityDirectionToLink();
    test_GetVectorTowardsLink();
    test_AddEntitySpeedToPos();
    test_AddEntityZSpeedToPos();
    test_UpdateEntityPosWithSpeed();
    test_RecoilAndIgnoringHits();
}
