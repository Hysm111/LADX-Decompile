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

void run_entities_tests(void) {
    test_is_zero();
    test_entity_countdowns();
    test_create_trading_item_entity();
    test_spawn_entity_trampolines();
    test_animate_entities_trampolines();
    printf("  [PASS] All entities.asm functions verified successfully!\n\n");
}
