#include "bank1/room_transition.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/dialog.h"
#include "constants/entities.h"
#include "constants/rooms.h"
#include "constants/maps.h"
#include "constants/sfx.h"
#include "constants/link.h"
#include "constants/gameplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void test_prepare_entity_position_for_room_transition(void) {
    printf("[*] Running PrepareEntityPositionForRoomTransition tests (01:5EAB)...\n");
    GBState gb;

    /* Direction: RIGHT (0). off_x = 0xA0, sign_x = 0x00, off_y = 0x00, sign_y = 0x00 */
    gb_init(&gb);
    gb_write(&gb, hMultiPurposeD, 3);
    gb_write(&gb, wRoomTransitionDirection, 0);
    gb_write(&gb, wEntitiesPosXTable + 2, 0x70);
    gb_write(&gb, wEntitiesPosXSignTable + 2, 0x00);
    gb_write(&gb, wEntitiesPosYTable + 2, 0x40);
    gb_write(&gb, wEntitiesPosYSignTable + 2, 0x00);

    PrepareEntityPositionForRoomTransition(&gb, 2);

    assert(gb_read(&gb, wEntitiesLoadOrderTable + 2) == 3);
    assert(gb_read(&gb, hMultiPurposeD) == 4);
    assert(gb_read(&gb, hMultiPurpose0) == 0xA0);
    assert(gb_read(&gb, hMultiPurpose1) == 0x00);
    assert(gb_read(&gb, hMultiPurpose2) == 0x00);
    assert(gb_read(&gb, hMultiPurpose3) == 0x00);
    /* 0x70 + 0xA0 = 0x110 -> low byte 0x10, carry into sign -> 0x01 */
    assert(gb_read(&gb, wEntitiesPosXTable + 2) == 0x10);
    assert(gb_read(&gb, wEntitiesPosXSignTable + 2) == 0x01);
    /* Y: 0x40 + 0 = 0x40, sign = 0 */
    assert(gb_read(&gb, wEntitiesPosYTable + 2) == 0x40);
    assert(gb_read(&gb, wEntitiesPosYSignTable + 2) == 0x00);

    /* Direction: LEFT (1). off_x = 0x60, sign_x = 0xFF, off_y = 0x00, sign_y = 0x00 */
    gb_init(&gb);
    gb_write(&gb, hMultiPurposeD, 0);
    gb_write(&gb, wRoomTransitionDirection, 1);
    gb_write(&gb, wEntitiesPosXTable + 1, 0x20);
    gb_write(&gb, wEntitiesPosXSignTable + 1, 0x00);
    PrepareEntityPositionForRoomTransition(&gb, 1);
    /* 0x20 + 0x60 = 0x80 (no carry), sign = 0 + 0xFF + 0 = 0xFF */
    assert(gb_read(&gb, wEntitiesPosXTable + 1) == 0x80);
    assert(gb_read(&gb, wEntitiesPosXSignTable + 1) == 0xFF);

    /* Direction: TOP (2). off_x = 0x00, sign_x = 0x00, off_y = 0x80, sign_y = 0xFF */
    gb_init(&gb);
    gb_write(&gb, hMultiPurposeD, 0);
    gb_write(&gb, wRoomTransitionDirection, 2);
    gb_write(&gb, wEntitiesPosYTable + 0, 0x30);
    gb_write(&gb, wEntitiesPosYSignTable + 0, 0x00);
    PrepareEntityPositionForRoomTransition(&gb, 0);
    /* 0x30 + 0x80 = 0xB0 (no carry), sign = 0 + 0xFF = 0xFF */
    assert(gb_read(&gb, wEntitiesPosYTable + 0) == 0xB0);
    assert(gb_read(&gb, wEntitiesPosYSignTable + 0) == 0xFF);
}

void test_update_recent_rooms_list(void) {
    printf("[*] Running UpdateRecentRoomsList tests (01:5F02)...\n");
    GBState gb;
    gb_init(&gb);

    for (int i = 0; i < 6; i++) {
        gb_write(&gb, wRecentRooms + i, 0xFF);
    }
    gb_write(&gb, wRecentRoomsIndex, 0);

    /* Room already in list -> does nothing */
    gb_write(&gb, wRecentRooms + 2, 0x12);
    gb_write(&gb, hMapRoom, 0x12);
    UpdateRecentRoomsList(&gb);
    assert(gb_read(&gb, wRecentRoomsIndex) == 0);

    /* Room not in list -> adds room, advances index, clears evicted room flag */
    gb_write(&gb, hMapRoom, 0x34);
    gb_write(&gb, wRecentRooms + 1, 0x77); /* evicted room */
    gb_write(&gb, wEntitiesClearedRooms + 0x77, 0x5A);
    UpdateRecentRoomsList(&gb);
    assert(gb_read(&gb, wRecentRoomsIndex) == 1);
    assert(gb_read(&gb, wRecentRooms + 1) == 0x34);
    assert(gb_read(&gb, wEntitiesClearedRooms + 0x77) == 0);

    /* Wraparound at 6 */
    gb_write(&gb, wRecentRoomsIndex, 5);
    gb_write(&gb, hMapRoom, 0x88);
    gb_write(&gb, wRecentRooms + 0, 0x99);
    gb_write(&gb, wEntitiesClearedRooms + 0x99, 0xFF);
    UpdateRecentRoomsList(&gb);
    assert(gb_read(&gb, wRecentRoomsIndex) == 0);
    assert(gb_read(&gb, wRecentRooms + 0) == 0x88);
    assert(gb_read(&gb, wEntitiesClearedRooms + 0x99) == 0);
}

void test_hide_all_sprites(void) {
    printf("[*] Running HideAllSprites tests (01:5F2E)...\n");
    GBState gb;

    /* DMG */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 0);
    for (int i = 0; i < 40; i++) {
        gb_write(&gb, wOAMBuffer + i * 4, 0x10);
    }
    HideAllSprites(&gb);
    assert(gb.sram_enabled == false);
    for (int i = 0; i < 40; i++) {
        assert(gb_read(&gb, wOAMBuffer + i * 4) == 0xF4);
    }

    /* CGB */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    HideAllSprites(&gb);
    assert(gb.sram_enabled == false);
}

void test_hide_sprites(void) {
    printf("[*] Running HideSprites and HideSpritesUnderDialog tests (01:5F4B, 01:5F68)...\n");
    GBState gb;

    /* 1. Inventory appearing */
    gb_init(&gb);
    gb_write(&gb, wInventoryAppearing, 1);
    gb_write(&gb, wWindowY, 0x20); /* d = 0x20 + 8 = 0x28 */
    /* Sprite 0: Y = 0x20 (< 0x28, not hidden) */
    gb_write(&gb, wOAMBuffer + 0, 0x20);
    /* Sprite 1: Y = 0x30 (>= 0x28, hidden) */
    gb_write(&gb, wOAMBuffer + 4, 0x30);
    HideSprites(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == 0x20);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x00);

    /* 2. Dialog box at bottom (DIALOG_BOX_BOTTOM_FLAG set) */
    gb_init(&gb);
    gb_write(&gb, wInventoryAppearing, 0);
    gb_write(&gb, wWindowY, 0x80); /* not 0 */
    gb_write(&gb, wDialogState, DIALOG_BOX_BOTTOM_FLAG | DIALOG_LETTER_IN_1);
    /* d = 0x58 */
    /* Entry 9 (first tested): Y = 0x50 (< 0x58, not hidden) */
    gb_write(&gb, wOAMBuffer + 9 * 4, 0x50);
    /* Entry 10: Y = 0x60 (>= 0x58, hidden) */
    gb_write(&gb, wOAMBuffer + 10 * 4, 0x60);
    HideSprites(&gb);
    assert(gb_read(&gb, wOAMBuffer + 9 * 4) == 0x50);
    assert(gb_read(&gb, wOAMBuffer + 10 * 4) == 0x00);

    /* 3. Dialog box at top, Dialog04F Piece of Heart exception */
    gb_init(&gb);
    gb_write(&gb, wInventoryAppearing, 0);
    gb_write(&gb, wWindowY, 0x80);
    gb_write(&gb, wDialogState, DIALOG_LETTER_IN_1); /* top box, d = 0x3E */
    gb_write(&gb, wDialogIndex, Dialog04F);
    gb_write(&gb, wDialogIndexHi, 0);

    /* Entry 9: Y = 0x20 (< 0x3E -> hide candidate), Tile = 0x9B (Piece of Heart) -> preserved */
    gb_write(&gb, wOAMBuffer + 9 * 4 + 0, 0x20);
    gb_write(&gb, wOAMBuffer + 9 * 4 + 2, 0x9B);

    /* Entry 10: Y = 0x20 (< 0x3E -> hide candidate), Tile = 0x30 (regular sprite) -> hidden */
    gb_write(&gb, wOAMBuffer + 10 * 4 + 0, 0x20);
    gb_write(&gb, wOAMBuffer + 10 * 4 + 2, 0x30);

    HideSprites(&gb);
    assert(gb_read(&gb, wOAMBuffer + 9 * 4 + 0) == 0x20);
    assert(gb_read(&gb, wOAMBuffer + 10 * 4 + 0) == 0x00);
}


static uint8_t g_last_spawned_entity = 0;
static uint16_t mock_spawn_new_entity(GBState *gb, uint8_t entity_type) {
    g_last_spawned_entity = entity_type;
    return 3; /* Always assign slot 3 */
}

static bool g_mock_audio_called = false;
static void mock_func_01F_4003(GBState *gb) {
    g_mock_audio_called = true;
}

void test_synchronize_dungeons_item_flags(void) {
    printf("[*] Running SynchronizeDungeonsItemFlags tests (01:5E67)...\n");
    GBState gb;

    /* 1. Overworld -> does nothing */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hMapId, 2);
    for (int i = 0; i < 5; i++) {
        gb_write(&gb, wCurrentDungeonItemFlags + i, 0x10 + i);
    }
    SynchronizeDungeonsItemFlags(&gb);
    for (int i = 0; i < 5; i++) {
        assert(gb_read(&gb, wDungeonItemFlags + 2 * 5 + i) == 0);
    }

    /* 2. Dungeon 2 (hMapId = 2) */
    gb_write(&gb, wIsIndoor, 1);
    SynchronizeDungeonsItemFlags(&gb);
    for (int i = 0; i < 5; i++) {
        assert(gb_read(&gb, wDungeonItemFlags + 2 * 5 + i) == 0x10 + i);
    }

    /* 3. Non-dungeon cave (hMapId >= MAP_CAVE_B) -> does nothing */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_CAVE_B);
    SynchronizeDungeonsItemFlags(&gb);
    for (int i = 0; i < 5; i++) {
        assert(gb_read(&gb, wDungeonItemFlags + i) == 0);
    }

    /* 4. Color Dungeon (hMapId = MAP_COLOR_DUNGEON = 0xFF) */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    for (int i = 0; i < 5; i++) {
        gb_write(&gb, wCurrentDungeonItemFlags + i, 0x40 + i);
    }
    SynchronizeDungeonsItemFlags(&gb);
    for (int i = 0; i < 5; i++) {
        assert(gb_read(&gb, wColorDungeonItemFlags + i) == 0x40 + i);
    }
}

void test_create_following_npc_entity(void) {
    printf("[*] Running CreateFollowingNpcEntity tests (01:5FB3)...\n");
    GBState gb;

    /* 1. Indoor exclusions */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hIsSideScrolling, 1);
    gb_write(&gb, wIsRoosterFollowingLink, 1);
    g_last_spawned_entity = 0;
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(g_last_spawned_entity == 0);

    /* 2. Rooster spawn */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wIsRoosterFollowingLink, 1);
    gb_write(&gb, hLinkPositionX, 0x40);
    gb_write(&gb, hLinkPositionY, 0x30);
    gb_write(&gb, hLinkPositionZ, 0x05);
    gb_write(&gb, wC13B, 0x02);
    /* Existing rooster in slot 1 */
    gb_write(&gb, wEntitiesTypeTable + 1, ENTITY_ROOSTER);
    gb_write(&gb, wEntitiesStatusTable + 1, 0x01);
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(gb_read(&gb, wEntitiesStatusTable + 1) == 0); /* old rooster deactivated */
    assert(g_last_spawned_entity == ENTITY_ROOSTER);
    assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x40);
    assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x32); /* 0x30 + 0x02 */
    assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x05);

    /* 3. Ghost trigger (state 2 -> state 1) */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wIsGhostFollowingLink, 2);
    gb_write(&gb, hMapRoom, ROOM_SECTION_OW_GHOST_TRIGGER);
    gb_write(&gb, wHasInstrument4, 0x02);
    gb_write(&gb, wPowerBraceletLevel, 1);
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(gb_read(&gb, wIsGhostFollowingLink) == 1);

    /* Ghost spawn (state 1) */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wIsGhostFollowingLink, 1);
    gb_write(&gb, hLinkPositionX, 0x55);
    gb_write(&gb, hLinkPositionY, 0x66);
    gb_write(&gb, wC13B, 0x04);
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(g_last_spawned_entity == ENTITY_GHOST);
    assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x55);
    assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x6A);
    assert(gb_read(&gb, wEntitiesPrivateState1Table + 3) == 1);
    assert(gb_read(&gb, hJingle) == JINGLE_GHOST_PRESENCE);

    /* 4. Marin spawn */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wIsMarinFollowingLink, 1);
    gb_write(&gb, hLinkPositionX, 0x20);
    gb_write(&gb, hLinkPositionY, 0x40);
    gb_write(&gb, hLinkPositionZ, 0x00);
    gb_write(&gb, hLinkDirection, 2);
    gb_write(&gb, wC13B, 0x00);
    gb_write(&gb, wDB10, 1);
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(g_last_spawned_entity == ENTITY_MARIN_AT_THE_SHORE);
    assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x40); /* 0x20 + 0x20 */
    assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x50); /* 0x40 + 0x10 */
    assert(gb_read(&gb, wEntitiesPrivateState4Table + 3) == 1);
    assert(gb_read(&gb, wEntitiesPrivateCountdown1Table + 3) == 0x0C);
    for (int i = 0; i < 0x10; i++) {
        assert(gb_read(&gb, wLinkPositionXHistory + i) == 0x20);
        assert(gb_read(&gb, wLinkPositionYHistory + i) == 0x40);
        assert(gb_read(&gb, wLinkPositionZHistory + i) == 0x00);
        assert(gb_read(&gb, wLinkDirectionHistory + i) == 2);
    }

    /* 5. Bow-Wow spawn */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, wIsBowWowFollowingLink, BOW_WOW_FOLLOWING);
    gb_write(&gb, hLinkPositionX, 0x18);
    gb_write(&gb, hLinkPositionY, 0x28);
    gb_write(&gb, hLinkPositionZ, 0x00);
    CreateFollowingNpcEntity(&gb, mock_spawn_new_entity);
    assert(g_last_spawned_entity == ENTITY_BOW_WOW);
    assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x18);
    assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x28);
    assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x00);
}

void test_func_001_6162(void) {
    printf("[*] Running func_001_6162 tests (01:6162)...\n");
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wGameplayType, 2);
    gb_write(&gb, wGameplaySubtype, 3);
    gb_write(&gb, wOBJ0Palette, 0xFF);
    gb_write(&gb, wOBJ1Palette, 0xFF);
    gb_write(&gb, wBGPalette, 0xFF);
    gb_write(&gb, rBGP, 0xE4);
    gb_write(&gb, rOBP0, 0xE4);
    gb_write(&gb, rOBP1, 0xE4);
    gb_write(&gb, hBaseScrollY, 0x10);
    gb_write(&gb, hBaseScrollX, 0x20);
    gb_write(&gb, wSwitchBlocksState, 1);
    gb_write(&gb, wSwitchableObjectAnimationStage, 2);
    gb_write(&gb, hButtonsInactiveDelay, 0);

    g_mock_audio_called = false;
    func_001_6162(&gb, mock_func_01F_4003);

    assert(g_mock_audio_called == true);
    assert(gb_read(&gb, wGameplayType) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 0);
    assert(gb_read(&gb, wOBJ0Palette) == 0);
    assert(gb_read(&gb, wOBJ1Palette) == 0);
    assert(gb_read(&gb, wBGPalette) == 0);
    assert(gb_read(&gb, rBGP) == 0);
    assert(gb_read(&gb, rOBP0) == 0);
    assert(gb_read(&gb, rOBP1) == 0);
    assert(gb_read(&gb, hBaseScrollY) == 0);
    assert(gb_read(&gb, hBaseScrollX) == 0);
    assert(gb_read(&gb, wSwitchBlocksState) == 0);
    assert(gb_read(&gb, wSwitchableObjectAnimationStage) == 0);
    assert(gb_read(&gb, hButtonsInactiveDelay) == 0x18);
}

void test_load_counter_animated_tiles(void) {
    printf("[*] Running LoadCounterAnimatedTiles tests (01:61AA)...\n");
    GBState gb;
    gb_init(&gb);

    /* Allocate dummy ROM for bank $0F */
    uint8_t dummy_rom[0x4000 * 16];
    memset(dummy_rom, 0, sizeof(dummy_rom));
    /* Fill offset $5730 in bank $0F with 0xAA, and $5810 with 0xBB */
    memset(&dummy_rom[0x0F * 0x4000 + (0x5730 - 0x4000)], 0xAA, 0x10);
    memset(&dummy_rom[0x0F * 0x4000 + (0x5810 - 0x4000)], 0xBB, 0x10);
    gb_attach_rom(&gb, dummy_rom, sizeof(dummy_rom));

    /* Dialog ID = 0xB3: low nibble = 3 (addr 0x5730), high nibble = 0xB (addr 0x5810) */
    gb_write(&gb, wTextDebuggerDialogId, 0xB3);
    LoadCounterAnimatedTiles(&gb);

    /* Verify low nibble copied to 0x96D0 */
    for (int i = 0; i < 0x10; i++) {
        assert(gb_read(&gb, 0x96D0 + i) == 0xAA);
    }
    /* Verify high nibble copied to 0x96C0 */
    for (int i = 0; i < 0x10; i++) {
        assert(gb_read(&gb, 0x96C0 + i) == 0xBB);
    }
    assert(gb_read(&gb, 0x9909) == 0x6C);
    assert(gb_read(&gb, 0x990A) == 0x6D);
}

void test_open_dungeon_name_dialog(void) {
    printf("[*] Running OpenDungeonNameDialog tests (01:61EE)...\n");
    GBState gb;

    /* 1. Motion state not default */
    gb_init(&gb);
    gb_write(&gb, wLinkMotionState, 1);
    gb_write(&gb, wFreeMovementMode, 0);
    gb_write(&gb, hMapId, 2);
    OpenDungeonNameDialog(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0);

    /* 2. Free movement mode active */
    gb_init(&gb);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    gb_write(&gb, wFreeMovementMode, 1);
    gb_write(&gb, hMapId, 2);
    OpenDungeonNameDialog(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0);

    /* 3. Successful open */
    gb_init(&gb);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    gb_write(&gb, wFreeMovementMode, 0);
    gb_write(&gb, hMapId, 2);
    OpenDungeonNameDialog(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0x56 + 2);
}

void run_bank1_tests(void) {
    test_prepare_entity_position_for_room_transition();
    test_update_recent_rooms_list();
    test_hide_all_sprites();
    test_hide_sprites();
    test_synchronize_dungeons_item_flags();
    test_create_following_npc_entity();
    test_func_001_6162();
    test_load_counter_animated_tiles();
    test_open_dungeon_name_dialog();
    printf("  [PASS] All bank1 room transition & sprite functions verified successfully!\n\n");
}
