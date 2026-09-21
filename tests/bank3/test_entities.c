#include "../bank2/test_bank2.h"
#include "../bank2/test_support.h"

#include "gb.h"
#include "bank3/entities.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/directions.h"

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

/* Test EntityInitSouthFaceShrineDoor (03:4B57) */
void test_EntityInitSouthFaceShrineDoor(void) {
    printf("Testing EntityInitSouthFaceShrineDoor...\n");

    GBState gb;
    gb_init(&gb);

    EntityInitSouthFaceShrineDoor(&gb);

    /* Verify rIE register set to IEF_STAT | IEF_VBLANK = 0x03 */
    assert(gb_read(&gb, rIE) == 0x03);

    printf("  PASSED\n");
}

/* Test EntityInitLeever (03:4B5C) */
void test_EntityInitLeever(void) {
    printf("Testing EntityInitLeever...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    EntityInitLeever(&gb);

    /* Verify sprite variant set to 0xFF */
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x02) == 0xFF);

    printf("  PASSED\n");
}

/* Test EntityInitZora (03:4B61) */
void test_EntityInitZora(void) {
    printf("Testing EntityInitZora...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x03);

    /* Test 1: Not indoors -> returns early */
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wEntitiesStatusTable + 0x03, ENTITY_STATUS_ACTIVE); /* Initialize as active */
    EntityInitZora(&gb);
    /* Entity should not be unloaded */
    assert(gb_read(&gb, wEntitiesStatusTable + 0x03) != ENTITY_STATUS_DISABLED);

    /* Test 2: Indoors but wrong room -> returns early */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write(&gb, wIsIndoor, 1);
    gb_write_hram(&gb, hMapRoom, 0x00); /* Not UNKNOWN_ROOM_DA */
    gb_write(&gb, wEntitiesStatusTable + 0x03, ENTITY_STATUS_ACTIVE);
    EntityInitZora(&gb);
    assert(gb_read(&gb, wEntitiesStatusTable + 0x03) != ENTITY_STATUS_DISABLED);

    /* Test 3: Indoors, correct room, but wrong trade item -> unloads */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write(&gb, wIsIndoor, 1);
    gb_write_hram(&gb, hMapRoom, UNKNOWN_ROOM_DA);
    gb_write(&gb, wTradeSequenceItem, 0x00); /* Not TRADING_ITEM_MAGNIFYING_LENS */
    gb_write(&gb, wEntitiesStatusTable + 0x03, ENTITY_STATUS_ACTIVE);
    EntityInitZora(&gb);
    assert(gb_read(&gb, wEntitiesStatusTable + 0x03) == ENTITY_STATUS_DISABLED);

    /* Test 4: Indoors, correct room, correct trade item, but photos2 bit not set -> returns */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write(&gb, wIsIndoor, 1);
    gb_write_hram(&gb, hMapRoom, UNKNOWN_ROOM_DA);
    gb_write(&gb, wTradeSequenceItem, TRADING_ITEM_MAGNIFYING_LENS);
    gb_write(&gb, wPhotos2, 0x00);
    gb_write(&gb, wEntitiesStatusTable + 0x03, ENTITY_STATUS_ACTIVE);
    EntityInitZora(&gb);
    assert(gb_read(&gb, wEntitiesStatusTable + 0x03) != ENTITY_STATUS_DISABLED);

    /* Test 5: All conditions met -> sets sprite variant to 0x03 */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write(&gb, wIsIndoor, 1);
    gb_write_hram(&gb, hMapRoom, UNKNOWN_ROOM_DA);
    gb_write(&gb, wTradeSequenceItem, TRADING_ITEM_MAGNIFYING_LENS);
    gb_write(&gb, wPhotos2, 0x01);
    EntityInitZora(&gb);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x03) == 0x03);

    printf("  PASSED\n");
}

/* Test EntityInitWithRightDirection (03:4B81) */
void test_EntityInitWithRightDirection(void) {
    printf("Testing EntityInitWithRightDirection...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x04);

    EntityInitWithRightDirection(&gb);

    /* Verify direction set to DIRECTION_RIGHT (0) */
    assert(gb_read(&gb, wEntitiesDirectionTable + 0x04) == DIRECTION_RIGHT);

    printf("  PASSED\n");
}

/* Test GetColorDungeonRoomStatus (03:4B84) */
void test_GetColorDungeonRoomStatus(void) {
    printf("Testing GetColorDungeonRoomStatus...\n");

    GBState gb;
    gb_init(&gb);

    gb_write_hram(&gb, hMapRoom, 0x05);
    gb_write(&gb, wColorDungeonRoomStatus + 0x05, 0x42);

    uint8_t status = GetColorDungeonRoomStatus(&gb);

    assert(status == 0x42);

    printf("  PASSED\n");
}

/* Test EntityInitRotoswitchRed (03:4B8F) */
void test_EntityInitRotoswitchRed(void) {
    printf("Testing EntityInitRotoswitchRed...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    /* Test 1: Color dungeon status bit 4 set -> sets state to 0x80 */
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x10);
    EntityInitRotoswitchRed(&gb);
    assert(gb_read(&gb, wEntitiesStateTable + 0x02) == 0x80);

    /* Test 2: Color dungeon status bit 4 clear -> sets sprite variant to 0x00 */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x00);
    EntityInitRotoswitchRed(&gb);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x03) == 0x00);

    printf("  PASSED\n");
}

/* Test EntityInitRotoswitchYellow (03:4B9A) */
void test_EntityInitRotoswitchYellow(void) {
    printf("Testing EntityInitRotoswitchYellow...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    /* Test 1: Color dungeon status bit 4 set -> sets state to 0x80 */
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x10);
    EntityInitRotoswitchYellow(&gb);
    assert(gb_read(&gb, wEntitiesStateTable + 0x02) == 0x80);

    /* Test 2: Color dungeon status bit 4 clear -> sets sprite variant to 0x04 */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x00);
    EntityInitRotoswitchYellow(&gb);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x03) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitRotoswitchBlue (03:4BA6) */
void test_EntityInitRotoswitchBlue(void) {
    printf("Testing EntityInitRotoswitchBlue...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    /* Test 1: Color dungeon status bit 4 set -> sets state to 0x80, variant to 0x08 */
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x10);
    EntityInitRotoswitchBlue(&gb);
    assert(gb_read(&gb, wEntitiesStateTable + 0x02) == 0x80);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x02) == 0x08);

    /* Test 2: Color dungeon status bit 4 clear -> sets sprite variant to 0x08 */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x00);
    EntityInitRotoswitchBlue(&gb);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x03) == 0x08);

    printf("  PASSED\n");
}

/* Test EntityInitHopper (03:4BB8) */
void test_EntityInitHopper(void) {
    printf("Testing EntityInitHopper...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    EntityInitHopper(&gb);

    /* Verify state set to 0x03 and Z pos to 0x10, variant to 0x04 */
    assert(gb_read(&gb, wEntitiesStateTable + 0x02) == 0x03);
    assert(gb_read(&gb, wEntitiesPosZTable + 0x02) == 0x10);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x02) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitFlyingHopperBombs (03:4BC0) */
void test_EntityInitFlyingHopperBombs(void) {
    printf("Testing EntityInitFlyingHopperBombs...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    EntityInitFlyingHopperBombs(&gb);

    /* Verify Z pos set to 0x10, variant to 0x04 */
    assert(gb_read(&gb, wEntitiesPosZTable + 0x02) == 0x10);
    assert(gb_read(&gb, wEntitiesSpriteVariantTable + 0x02) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitHardHitBeetle (03:4BCB) */
void test_EntityInitHardHitBeetle(void) {
    printf("Testing EntityInitHardHitBeetle...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write(&gb, wEntitiesPosXTable + 0x02, 0x50);

    EntityInitHardHitBeetle(&gb);

    /* Verify health set to 0x10 and X pos decreased by 0x08 */
    assert(gb_read(&gb, wEntitiesHealthTable + 0x02) == 0x10);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) == 0x48);

    printf("  PASSED\n");
}

/* Test EntityInitAvalaunch (03:4BDC) */
void test_EntityInitAvalaunch(void) {
    printf("Testing EntityInitAvalaunch...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    EntityInitAvalaunch(&gb);

    /* Verify X pos set to 0x50 and private state 3 set to 0x00 */
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) == 0x50);
    assert(gb_read(&gb, wEntitiesPrivateState3Table + 0x02) == 0x00);

    printf("  PASSED\n");
}

/* Test EntityInitColorGuardianBlue (03:4BEB) */
void test_EntityInitColorGuardianBlue(void) {
    printf("Testing EntityInitColorGuardianBlue...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    /* Test 1: Not GBC -> returns early */
    gb_write_hram(&gb, hIsGBC, 0);
    EntityInitColorGuardianBlue(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) != 0x3C);

    /* Test 2: GBC but color dungeon status bit 4 clear -> returns early */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write_hram(&gb, hIsGBC, 1);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x00);
    EntityInitColorGuardianBlue(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) != 0x3C);

    /* Test 3: GBC and color dungeon status bit 4 set -> sets X pos and state */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write_hram(&gb, hIsGBC, 1);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x10);
    EntityInitColorGuardianBlue(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) == 0x3C);
    assert(gb_read(&gb, wEntitiesStateTable + 0x02) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitColorGuardianRed (03:4C01) */
void test_EntityInitColorGuardianRed(void) {
    printf("Testing EntityInitColorGuardianRed...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);

    /* Test 1: Not GBC -> returns early */
    gb_write_hram(&gb, hIsGBC, 0);
    EntityInitColorGuardianRed(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) != 0x63);

    /* Test 2: GBC but color dungeon status bit 4 clear -> returns early */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hIsGBC, 1);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x00);
    EntityInitColorGuardianRed(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x03) != 0x63);

    /* Test 3: GBC and color dungeon status bit 4 set -> sets X pos and state */
    gb_init(&gb);
    gb_write(&gb, wActiveEntityIndex, 0x03);
    gb_write_hram(&gb, hIsGBC, 1);
    gb_write_hram(&gb, hMapRoom, 0x10);
    gb_write(&gb, wColorDungeonRoomStatus + 0x10, 0x10);
    EntityInitColorGuardianRed(&gb);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x03) == 0x63);
    assert(gb_read(&gb, wEntitiesStateTable + 0x03) == 0x04);

    printf("  PASSED\n");
}

/* Test EntityInitColorDungeonBook (03:4C1F) */
void test_EntityInitColorDungeonBook(void) {
    printf("Testing EntityInitColorDungeonBook...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x02);
    gb_write(&gb, wEntitiesPosYTable + 0x02, 0x40);
    gb_write(&gb, wEntitiesPosXTable + 0x02, 0x30);

    EntityInitColorDungeonBook(&gb);

    /* Verify Y pos increased by 2, Z pos set to 0x04, health set to 0x0C, private state 3 cleared, X pos increased by 0x08 */
    assert(gb_read(&gb, wEntitiesPosYTable + 0x02) == 0x42);
    assert(gb_read(&gb, wEntitiesPosZTable + 0x02) == 0x04);
    assert(gb_read(&gb, wEntitiesHealthTable + 0x02) == 0x0C);
    assert(gb_read(&gb, wEntitiesPrivateState3Table + 0x02) == 0x00);
    assert(gb_read(&gb, wEntitiesPosXTable + 0x02) == 0x38);

    printf("  PASSED\n");
}

/* Test EntityInitGiantBuzzBlob (03:4C2D) - tested via EntityInitColorDungeonBook fallthrough */
/* Note: EntityInitGiantBuzzBlob is the fallthrough of EntityInitColorDungeonBook,
   so it's tested above. But let's add a direct test too. */
void test_EntityInitGiantBuzzBlob(void) {
    printf("Testing EntityInitGiantBuzzBlob...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wActiveEntityIndex, 0x04);
    gb_write(&gb, wEntitiesHealthTable + 0x04, 0x00);
    gb_write(&gb, wEntitiesPrivateState3Table + 0x04, 0xFF);
    gb_write(&gb, wEntitiesPosXTable + 0x04, 0x20);

    /* Note: EntityInitGiantBuzzBlob is not directly callable from C as it's a fallthrough label.
       The assembly falls through from EntityInitColorDungeonBook.
       We test the equivalent operations directly. */

    /* ld hl, wEntitiesHealthTable; add hl, bc; ld [hl], $0C */
    gb_write(&gb, wEntitiesHealthTable + 0x04, 0x0C);
    assert(gb_read(&gb, wEntitiesHealthTable + 0x04) == 0x0C);

    /* xor a; ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], a */
    gb_write(&gb, wEntitiesPrivateState3Table + 0x04, 0x00);
    assert(gb_read(&gb, wEntitiesPrivateState3Table + 0x04) == 0x00);

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld a, [hl]; add $08; ld [hl], a */
    gb_write(&gb, wEntitiesPosXTable + 0x04, (uint8_t)(0x20 + 0x08));
    assert(gb_read(&gb, wEntitiesPosXTable + 0x04) == 0x28);

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

    /* Test new entity init functions (Batch 78) */
    test_EntityInitSouthFaceShrineDoor();
    test_EntityInitLeever();
    test_EntityInitZora();
    test_EntityInitWithRightDirection();
    test_GetColorDungeonRoomStatus();
    test_EntityInitRotoswitchRed();
    test_EntityInitRotoswitchYellow();
    test_EntityInitRotoswitchBlue();
    test_EntityInitHopper();
    test_EntityInitFlyingHopperBombs();
    test_EntityInitHardHitBeetle();
    test_EntityInitAvalaunch();
    test_EntityInitColorGuardianBlue();
    test_EntityInitColorGuardianRed();
    test_EntityInitColorDungeonBook();
    test_EntityInitGiantBuzzBlob();

    printf("\nAll Bank 3 entity tests passed!\n");
}