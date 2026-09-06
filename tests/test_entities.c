#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "gb.h"
#include "home/entities.h"
#include "constants/memory.h"
#include "constants/entities.h"

void test_is_zero(void) {
    printf("[*] Running IsZero tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, 0xC200, 0x00);
    gb_write(&gb, 0xC205, 0x42);

    assert(IsZero(&gb, 0xC200, 0) == 0);
    assert(IsZero(&gb, 0xC200, 5) == 0x42);
}

void test_entity_countdowns(void) {
    printf("[*] Running Entity Countdown tests...\n");

    GBState gb;
    gb_init(&gb);

    /* Test slow transition countdown */
    gb_write(&gb, wEntitiesSlowTransitionCountdownTable + 3, 0x05);
    assert(GetEntitySlowTransitionCountdown(&gb, 3) == 0x05);

    /* Test private countdown 1 */
    gb_write(&gb, wEntitiesPrivateCountdown1Table + 2, 0x09);
    assert(GetEntityPrivateCountdown1(&gb, 2) == 0x09);

    /* Test transition countdown */
    gb_write(&gb, wEntitiesTransitionCountdownTable + 1, 0x07);
    assert(GetEntityTransitionCountdown(&gb, 1) == 0x07);

    /* Test ignore hits countdown decrement */
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + 4, 0x03);
    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 0x02);

    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 0x01);

    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 0x00);

    /* Verify doesn't decrement below 0 */
    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 0x00);
}

static int spawn_called = 0;
static uint16_t mock_spawn_entity(GBState *gb, uint8_t entity_type) {
    spawn_called++;
    assert(gb->rom_bank == 0x03);
    assert(entity_type == ENTITY_TRADING_ITEM);
    return 0x05;
}

static uint16_t mock_spawn_in_range(GBState *gb, uint8_t entity_type) {
    spawn_called++;
    assert(gb->rom_bank == 0x03);
    assert(entity_type == 0x10);
    return 0x08;
}

static uint16_t mock_spawn_trampoline_wrapper(GBState *gb, uint8_t entity_type) {
    return SpawnNewEntity_trampoline(gb, entity_type, mock_spawn_entity);
}

void test_create_trading_item_entity(void) {
    printf("[*] Running CreateTradingItemEntity tests...\n");

    GBState gb;
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x01);
    gb.rom_bank = 0x01;
    gb_write(&gb, hLinkPositionX, 0x48);
    gb_write(&gb, hLinkPositionY, 0x60);

    spawn_called = 0;
    CreateTradingItemEntity(&gb, mock_spawn_trampoline_wrapper);

    assert(spawn_called == 1);
    assert(gb.rom_bank == 0x01); /* Trampoline restored saved bank */
    assert(gb_read(&gb, wEntitiesPosXTable + 5) == 0x48);
    assert(gb_read(&gb, wEntitiesPosYTable + 5) == 0x60);
}

void test_spawn_entity_trampolines(void) {
    printf("[*] Running SpawnNewEntity trampolines tests...\n");

    GBState gb;
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x02);
    gb.rom_bank = 0x02;

    /* 1. SpawnNewEntity_trampoline */
    spawn_called = 0;
    uint16_t de = SpawnNewEntity_trampoline(&gb, ENTITY_TRADING_ITEM, mock_spawn_entity);
    assert(spawn_called == 1);
    assert(de == 0x05);
    assert(gb.rom_bank == 0x02); /* ReloadSavedBank restored wCurrentBank */

    /* 2. SpawnNewEntityInRange_trampoline */
    spawn_called = 0;
    gb_write(&gb, wCurrentBank, 0x04);
    gb.rom_bank = 0x04;
    de = SpawnNewEntityInRange_trampoline(&gb, 0x10, mock_spawn_in_range);
    assert(spawn_called == 1);
    assert(de == 0x08);
    assert(gb.rom_bank == 0x04); /* ReloadSavedBank restored wCurrentBank */
}

static int animate_called = 0;
static void mock_animate_entities(GBState *gb) {
    animate_called++;
    assert(gb->rom_bank == 0x03);
}

void test_animate_entities_trampolines(void) {
    printf("[*] Running AnimateEntities trampolines tests...\n");

    GBState gb;
    gb_init(&gb);

    /* 1. AnimateEntitiesAndRestoreBank17 */
    animate_called = 0;
    AnimateEntitiesAndRestoreBank17(&gb, mock_animate_entities);
    assert(animate_called == 1);
    assert(gb.rom_bank == 0x17);
    assert(gb_read(&gb, wCurrentBank) == 0x17);

    /* 2. AnimateEntitiesAndRestoreBank01 */
    animate_called = 0;
    AnimateEntitiesAndRestoreBank01(&gb, mock_animate_entities);
    assert(animate_called == 1);
    assert(gb.rom_bank == 0x01);
    assert(gb_read(&gb, wCurrentBank) == 0x01);

    /* 3. AnimateEntitiesAndRestoreBank02 */
    animate_called = 0;
    AnimateEntitiesAndRestoreBank02(&gb, mock_animate_entities);
    assert(animate_called == 1);
    assert(gb.rom_bank == 0x02);
    assert(gb_read(&gb, wCurrentBank) == 0x02);
}


static uint8_t mock_entities_rom[0x4000 * 0x20];
#define ROM_OFFSET(bank, addr) (((size_t)(bank) * 0x4000) + ((size_t)(addr) - 0x4000))

static bool mock_cb_called = false;
static void mock_check_bank_cb(GBState *gb, uint8_t expected_bank) {
    mock_cb_called = true;
    assert(gb->rom_bank == expected_bank);
}

static void mock_cb_19(GBState *gb) {
    mock_check_bank_cb(gb, 0x19);
    assert(gb_read(gb, wCurrentBank) == 0x19);
}
static void mock_cb_03(GBState *gb) { mock_check_bank_cb(gb, 0x03); }
static void mock_cb_14(GBState *gb) { mock_check_bank_cb(gb, 0x14); }
static void mock_cb_01(GBState *gb) {
    mock_check_bank_cb(gb, 0x01);
    assert(gb_read(gb, wCurrentBank) == 0x01);
}

static void test_entities_batch_trampolines(void) {
    printf("[*] Running Bank 0 entities helper & trampoline tests (00:3925-00:3988)...\n");

    GBState gb;

    /* 1. CanBowWowEatEntity */
    gb_init(&gb);
    memset(mock_entities_rom, 0, sizeof(mock_entities_rom));
    mock_entities_rom[ROM_OFFSET(0x14, BowWowEatableEntitiesTable + 0x09)] = 0x01;
    mock_entities_rom[ROM_OFFSET(0x14, BowWowEatableEntitiesTable + 0x0A)] = 0x00;
    mock_entities_rom[ROM_OFFSET(0x14, BowWowEatableEntitiesTable + 0x1B)] = 0x01;
    gb_attach_rom(&gb, mock_entities_rom, sizeof(mock_entities_rom));

    assert(CanBowWowEatEntity(&gb, 0x09) == 0x01);
    assert(gb.rom_bank == 0x05);

    assert(CanBowWowEatEntity(&gb, 0x0A) == 0x00);
    assert(gb.rom_bank == 0x05);

    assert(CanBowWowEatEntity(&gb, 0x1B) == 0x01);
    assert(gb.rom_bank == 0x05);

    /* 2. label_3935 */
    gb_init(&gb);
    mock_cb_called = false;
    label_3935(&gb, mock_cb_19);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x03);
    assert(gb_read(&gb, wCurrentBank) == 0x03);

    /* 3. LiftableRockStartSmashingAnimation_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x07);
    gb.rom_bank = 0x07;
    mock_cb_called = false;
    LiftableRockStartSmashingAnimation_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x07);

    /* 4. label_394D */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x06);
    gb.rom_bank = 0x06;
    mock_cb_called = false;
    label_394D(&gb, mock_cb_14);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x06);

    /* 5. CreateFollowingNpcEntity_trampoline */
    gb_init(&gb);
    mock_cb_called = false;
    CreateFollowingNpcEntity_trampoline(&gb, mock_cb_01);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x02);
    assert(gb_read(&gb, wCurrentBank) == 0x02);

    /* 6. ConfigureNewEntity_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x08);
    gb.rom_bank = 0x08;
    mock_cb_called = false;
    ConfigureNewEntity_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x08);

    /* 7. GetEntityDirectionToLink_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x09);
    gb.rom_bank = 0x09;
    mock_cb_called = false;
    GetEntityDirectionToLink_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x09);

    /* 8. label_397B */
    gb_init(&gb);
    mock_cb_called = false;
    label_397B(&gb, mock_cb_14);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x03);
}


static void mock_cb_02(GBState *gb) {
    mock_check_bank_cb(gb, 0x02);
}

static void test_entities_batch_hitbox_and_collision_trampolines(void) {
    printf("[*] Running Bank 0 entities hitbox, state, and collision trampolines (00:3AEA-00:3BBF)...\n");

    GBState gb;

    /* 1. ConfigureEntityHitbox - unmapped ROM (fallback table) */
    gb_init(&gb);
    gb_write(&gb, wEntitiesHitboxFlagsTable + 2, 0x14 | 0x03); /* offset 0x14 */
    ConfigureEntityHitbox(&gb, 2);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 8) == 0x08);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 9) == 0x13);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 10) == 0x08);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 11) == 0x13);

    /* 2. ConfigureEntityHitbox - mapped ROM */
    gb_init(&gb);
    memset(mock_entities_rom, 0, sizeof(mock_entities_rom));
    mock_entities_rom[HitboxPositions + 0x28 + 0] = 0x0C;
    mock_entities_rom[HitboxPositions + 0x28 + 1] = 0x07;
    mock_entities_rom[HitboxPositions + 0x28 + 2] = 0xFC;
    mock_entities_rom[HitboxPositions + 0x28 + 3] = 0x04;
    gb_attach_rom(&gb, mock_entities_rom, sizeof(mock_entities_rom));

    gb_write(&gb, wEntitiesHitboxFlagsTable + 5, 0x28 | 0x80); /* offset 0x28, ignore hits bit set */
    ConfigureEntityHitbox(&gb, 5);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 20) == 0x0C);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 21) == 0x07);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 22) == 0xFC);
    assert(gb_read(&gb, wEntitiesHitboxPositionTable + 23) == 0x04);

    /* 3. SetEntitySpriteVariant */
    gb_init(&gb);
    SetEntitySpriteVariant(&gb, 4, 0x33);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 4) == 0x33);

    /* 4. IncrementEntityState */
    gb_init(&gb);
    gb_write(&gb, wEntitiesStateTable + 3, 0x05);
    IncrementEntityState(&gb, 3);
    assert(gb_read(&gb, wEntitiesStateTable + 3) == 0x06);

    /* 5. HurtBySpikes_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x08);
    gb.rom_bank = 0x08;
    mock_cb_called = false;
    HurtBySpikes_trampoline(&gb, mock_cb_02);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x08);

    /* 6. ApplyEntityInteractionWithBackground_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0A);
    gb.rom_bank = 0x0A;
    mock_cb_called = false;
    ApplyEntityInteractionWithBackground_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0A);

    /* 7. label_3B2E */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0B);
    gb.rom_bank = 0x0B;
    mock_cb_called = false;
    label_3B2E(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0B);

    /* 8. DefaultEnemyDamageCollisionHandler_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0C);
    gb.rom_bank = 0x0C;
    mock_cb_called = false;
    DefaultEnemyDamageCollisionHandler_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0C);

    /* 9. label_3B44 */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0D);
    gb.rom_bank = 0x0D;
    mock_cb_called = false;
    label_3B44(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0D);

    /* 10. CheckLinkCollisionWithProjectile_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0E);
    gb.rom_bank = 0x0E;
    mock_cb_called = false;
    CheckLinkCollisionWithProjectile_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0E);

    /* 11. CheckLinkCollisionWithEnemy_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0F);
    gb.rom_bank = 0x0F;
    mock_cb_called = false;
    CheckLinkCollisionWithEnemy_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x0F);

    /* 12. label_3B65 */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x10);
    gb.rom_bank = 0x10;
    mock_cb_called = false;
    label_3B65(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x10);

    /* 13. label_3B70 */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x11);
    gb.rom_bank = 0x11;
    mock_cb_called = false;
    label_3B70(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x11);

    /* 14. label_3B7B */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x12);
    gb.rom_bank = 0x12;
    mock_cb_called = false;
    label_3B7B(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x12);

    /* 15. ApplyVectorTowardsLink_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x13);
    gb.rom_bank = 0x13;
    mock_cb_called = false;
    ApplyVectorTowardsLink_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x13);

    /* 16. GetVectorTowardsLink_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x14);
    gb.rom_bank = 0x14;
    mock_cb_called = false;
    GetVectorTowardsLink_trampoline(&gb, mock_cb_03);
    assert(mock_cb_called);
    assert(gb.rom_bank == 0x14);
}

void run_entities_tests(void) {
    test_is_zero();
    test_entity_countdowns();
    test_create_trading_item_entity();
    test_spawn_entity_trampolines();
    test_animate_entities_trampolines();
    test_entities_batch_trampolines();
    test_entities_batch_hitbox_and_collision_trampolines();
    printf("  [PASS] All entities.asm functions verified successfully!\n\n");
}
