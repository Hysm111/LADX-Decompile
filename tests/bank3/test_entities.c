#include "../bank2/test_bank2.h"
#include "../bank2/test_support.h"

#include "gb.h"
#include "bank3/entities.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/memory.h"
#include "constants/rooms.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Mock callbacks - declared in test_support.h */

/* Mock callbacks - provided by test_support.c */

/* Test ConfigureNewEntity (03:485B-03:4891) - SKIPPED: Requires ROM tables */
/* Test ConfigureEntityHealth (03:4895-03:48AC) - SKIPPED: Requires ROM tables */

/* Test MasterStalfosDefeated (03:48AD-03:48BE) */
void test_MasterStalfosDefeated(void) {
    printf("Testing MasterStalfosDefeated...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x07);

    MasterStalfosDefeated(&gb);

    /* Verify room event executed flag set */
    assert(gb_read(&gb, wRoomEventEffectExecuted) == 0x01);

    /* Verify entity unloaded - entity status should be cleared */
    assert(gb_read(&gb, wEntitiesStatusTable + 0x07) == ENTITY_STATUS_DISABLED);

    printf("  PASSED\n");
}

/* Test EntityInitHorsePiece (03:4926-03:4931) */
void test_EntityInitHorsePiece(void) {
    printf("Testing EntityInitHorsePiece...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write(&gb, wEntitiesLoadOrderTable + 0x02, 0x00); /* load order 0 -> variant 0x01 */

    EntityInitHorsePiece(&gb);

    /* Verify variant set from Data_003_4924[0] = 0x01 */
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x02) == 0x01);

    /* Test load order 1 -> variant 0x04 */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x04);
    gb_write(&gb, wEntitiesLoadOrderTable + 0x04, 0x01);

    EntityInitHorsePiece(&gb);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x04) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitMarinAtTalTalHeights (03:4934-03:493C) */
void test_EntityInitMarinAtTalTalHeights(void) {
    printf("Testing EntityInitMarinAtTalTalHeights...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x06);
    gb_write(&gb, wEntitiesPosYTable + 0x06, 0x50);

    EntityInitMarinAtTalTalHeights(&gb);

    /* Verify Y position decreased by 3 */
    assert(gb_read(&gb, wEntitiesPosYTable + 0x06) == 0x4D);

    printf("  PASSED\n");
}

/* Test EntityInitHandler (03:48B5-03:4923) - Boss defeated check */
void test_EntityInitHandler_BossDefeated(void) {
    printf("Testing EntityInitHandler (boss defeated)...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x01);
    gb_write(&gb, wEntitiesOptions1Table + 0x01, ENTITY_OPT1_IS_BOSS);
    gb_write_hram(&gb, hRoomStatus, ROOM_STATUS_EVENT_1); /* Boss defeated */
    gb_write_hram(&gb, hActiveEntityType, 0x10); /* Not Master Stalfos */

    EntityInitHandler(&gb);

    /* Should unload entity and return */
    assert(gb_read(&gb, wEntitiesStatusTable + 0x01) == ENTITY_STATUS_DISABLED);
    assert(g_mock_label_27F2_calls == 0);
    assert(g_mock_get_entity_init_handler_calls == 0);

    printf("  PASSED\n");
}

/* Test EntityInitHandler - Master Stalfos in non-Master-Stalfos room (defeated) */
void test_EntityInitHandler_MasterStalfos(void) {
    printf("Testing EntityInitHandler (Master Stalfos in other room)...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write(&gb, wEntitiesOptions1Table + 0x02, 0x00);
    gb_write_hram(&gb, hRoomStatus, 0x00);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_MASTER_STALFOS);
    gb_write_hram(&gb, hMapRoom, 0x20); /* Different room (not MSTALFOS_1/2/3) */
    gb_write(&gb, wIndoorARoomStatus + 0x20, 0x00); /* Defeated (bit 4-5 = 0) */

    EntityInitHandler(&gb);

    /* Should unload entity */
    assert(gb_read(&gb, wEntitiesStatusTable + 0x02) == ENTITY_STATUS_DISABLED);

    printf("  PASSED\n");
}

/* Test EntityInitHandler - Master Stalfos (non-defeated, in Master Stalfos room) */
void test_EntityInitHandler_MasterStalfosNonDefeated(void) {
    printf("Testing EntityInitHandler (Master Stalfos non-defeated)...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hActiveEntityType, ENTITY_MASTER_STALFOS);
    gb_write_hram(&gb, hMapRoom, ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1);
    gb_write(&gb, wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1, 0x30); /* Not defeated */

    EntityInitHandler(&gb);

    /* Should continue to normal init (in Master Stalfos room, no defeat check) */
    assert(gb_read(&gb, wDidBossIntro) == 0);
    assert(gb_read(&gb, wInBossBattle) == 1);
    assert(gb_read(&gb, wBossIntroDelay) == 0x20);
    assert(gb_read(&gb, wEntitiesStatusTable + 0x03) == ENTITY_STATUS_ACTIVE);

    printf("  PASSED\n");
}

/* Test EntityInitHandler - Indoor mini-boss */
void test_EntityInitHandler_IndoorMiniBoss(void) {
    printf("Testing EntityInitHandler (indoor mini-boss)...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x05);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, wD478, 0x00);
    gb_write(&gb, wEntitiesOptions1Table + 0x05, ENTITY_OPT1_IS_MINI_BOSS);
    gb_write_hram(&gb, hActiveEntityType, 0x20);
    gb_write_hram(&gb, hRoomStatus, 0x00);
    gb_write_hram(&gb, hMapRoom, 0x10);
    g_mock_label_27F2_calls = 0;
    g_mock_get_entity_init_handler_calls = 0;

    EntityInitHandler(&gb);

    /* Should set wC1CF from options1 */
    assert(gb_read(&gb, wC1CF) == ENTITY_OPT1_IS_MINI_BOSS);
    /* label_27F2 called directly, not through mock */
    assert(g_mock_get_entity_init_handler_calls == 0);

    printf("  PASSED\n");
}

/* Test EntityInitHandler - Normal entity */
void test_EntityInitHandler_Normal(void) {
    printf("Testing EntityInitHandler (normal)...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x08);
    gb_write(&gb, wIsIndoor, 0); /* Overworld */
    gb_write(&gb, wEntitiesOptions1Table + 0x08, 0x00);
    gb_write_hram(&gb, hActiveEntityType, 0x30);
    gb_write_hram(&gb, hRoomStatus, 0x00);
    gb_write_hram(&gb, hMapRoom, 0x20);

    EntityInitHandler(&gb);

    assert(gb_read(&gb, wDidBossIntro) == 0);
    assert(gb_read(&gb, wInBossBattle) == 1);
    assert(gb_read(&gb, wBossIntroDelay) == 0x20);
    assert(gb_read(&gb, wEntitiesStatusTable + 0x08) == ENTITY_STATUS_ACTIVE);

    printf("  PASSED\n");
}

void test_bank3_entities(void) {
    /* test_ConfigureNewEntity(); */
    /* test_ConfigureEntityHealth(); */
    test_MasterStalfosDefeated();
    test_EntityInitHorsePiece();
    test_EntityInitMarinAtTalTalHeights();
    test_EntityInitHandler_BossDefeated();
    test_EntityInitHandler_MasterStalfos();
    test_EntityInitHandler_IndoorMiniBoss();
    test_EntityInitHandler_Normal();

    printf("\nAll Bank 3 entity tests passed!\n");
}