#include "home/entities.h"
#include "constants/memory.h"
#include "constants/entities.h"
#include <stdio.h>
#include <assert.h>

void test_is_zero(void) {
    printf("[*] Running IsZero tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, 0xC500, 0x00);
    gb_write(&gb, 0xC505, 0x42);

    assert(IsZero(&gb, 0xC500, 0) == 0x00);
    assert(IsZero(&gb, 0xC500, 5) == 0x42);
}

void test_entity_countdowns(void) {
    printf("[*] Running Entity Countdown tests...\n");

    GBState gb;
    gb_init(&gb);

    /* Test GetEntitySlowTransitionCountdown */
    gb_write(&gb, wEntitiesSlowTransitionCountdownTable + 3, 0x12);
    gb_write(&gb, wEntitiesSlowTransitionCountdownTable + 7, 0x00);
    assert(GetEntitySlowTransitionCountdown(&gb, 3) == 0x12);
    assert(GetEntitySlowTransitionCountdown(&gb, 7) == 0x00);

    /* Test GetEntityPrivateCountdown1 */
    gb_write(&gb, wEntitiesPrivateCountdown1Table + 2, 0x34);
    gb_write(&gb, wEntitiesPrivateCountdown1Table + 5, 0x00);
    assert(GetEntityPrivateCountdown1(&gb, 2) == 0x34);
    assert(GetEntityPrivateCountdown1(&gb, 5) == 0x00);

    /* Test GetEntityTransitionCountdown */
    gb_write(&gb, wEntitiesTransitionCountdownTable + 0, 0x56);
    gb_write(&gb, wEntitiesTransitionCountdownTable + 9, 0x00);
    assert(GetEntityTransitionCountdown(&gb, 0) == 0x56);
    assert(GetEntityTransitionCountdown(&gb, 9) == 0x00);

    /* Test DecrementEntityIgnoreHitsCountdown */
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + 4, 5);
    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 4);

    /* Test that 0 does not underflow */
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + 8, 0);
    DecrementEntityIgnoreHitsCountdown(&gb, 8);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 8) == 0);
}

static uint8_t spawn_called = 0;
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

void run_entities_tests(void) {
    test_is_zero();
    test_entity_countdowns();
    test_create_trading_item_entity();
    test_spawn_entity_trampolines();
    printf("  [PASS] All entities.asm functions verified successfully!\n\n");
}
