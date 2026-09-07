#include "bank1/save.h"
#include "bank1/world_map.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "bank1/room_transition.h"
#include "constants/gfx.h"
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


void test_load_tileset_0f_and_attributes(void) {
    printf("[*] Running LoadTileset0F & func_001_6D11 tests (01:6CE3, 01:6D11)...\n");
    GBState gb;

    /* 1. DMG mode: fills checkerboard in bank 0, does not touch bank 1 attributes */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 0);
    LoadTileset0F(&gb);

    /* Row 0: alternating AE, AF */
    assert(gb_read(&gb, 0x9800) == 0xAE);
    assert(gb_read(&gb, 0x9801) == 0xAF);
    assert(gb_read(&gb, 0x9802) == 0xAE);
    assert(gb_read(&gb, 0x9813) == 0xAF); /* col 19 */
    assert(gb_read(&gb, 0x9814) == 0x00); /* col 20 not touched */
    assert(gb_read(&gb, 0x981F) == 0x00); /* col 31 not touched */

    /* Row 1: alternating AF, AE (inverted due to bit 5) */
    assert(gb_read(&gb, 0x9820) == 0xAF);
    assert(gb_read(&gb, 0x9821) == 0xAE);
    assert(gb_read(&gb, 0x9833) == 0xAE); /* col 19 */
    assert(gb_read(&gb, 0x9834) == 0x00); /* col 20 not touched */

    /* 2. GBC mode with GAMEPLAY_WORLD */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    LoadTileset0F(&gb);

    assert(gb_read(&gb, rVBK) == 0); /* bank restored */
    /* Check VRAM bank 1 attributes */
    gb_write(&gb, rVBK, 1);
    for (int i = 0; i < 0x400; i++) {
        assert(gb_read(&gb, 0x9800 + i) == 0x05);
    }
    gb_write(&gb, rVBK, 0);

    /* 3. GBC mode with other gameplay type */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wGameplayType, 0);
    func_001_6D11(&gb);
    gb_write(&gb, rVBK, 1);
    for (int i = 0; i < 0x400; i++) {
        assert(gb_read(&gb, 0x9800 + i) == 0x06);
    }
}

void test_write_dma_code_to_hram(void) {
    printf("[*] Running WriteDMACodeToHRAM tests (01:6D32)...\n");
    GBState gb;
    gb_init(&gb);

    WriteDMACodeToHRAM(&gb);

    static const uint8_t expected[10] = {
        0x3E, 0xC0, 0xE0, 0x46, 0x3E, 0x28, 0x3D, 0x20, 0xFD, 0xC9
    };
    for (int i = 0; i < 10; i++) {
        assert(gb_read(&gb, hDMARoutine + i) == expected[i]);
    }
}

void test_update_minimap_entrance_arrow(void) {
    printf("[*] Running UpdateMinimapEntranceArrowAndReturn tests (01:6DEA)...\n");
    GBState gb;

    /* 1. ROM_DebugTool2 enabled -> returns */
    gb_init(&gb);
    uint8_t dummy_rom[0x4000];
    memset(dummy_rom, 0, sizeof(dummy_rom));
    dummy_rom[ROM_DebugTool2] = 1;
    gb_attach_rom(&gb, dummy_rom, sizeof(dummy_rom));
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE) == 0);

    /* 2. Outdoors (wIsIndoor == 0) -> returns */
    dummy_rom[ROM_DebugTool2] = 0;
    gb_write(&gb, wIsIndoor, 0);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE) == 0);

    /* 3. Non-dungeon map (hMapId >= 8) -> returns */
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, 8);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE) == 0);

    /* 4. Tail Cave (hMapId = 0) normal */
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hIsSideScrolling, 0);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE) == 0xA3);

    /* 5. Tail Cave side-scrolling -> writes 0x7F */
    gb_write(&gb, hIsSideScrolling, 1);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_TAIL_CAVE) == 0x7F);

    /* 6. Color Dungeon (hMapId = 0xFF) */
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    gb_write(&gb, hIsSideScrolling, 0);
    UpdateMinimapEntranceArrowAndReturn(&gb);
    assert(gb_read(&gb, vBGMap1 + 0x20B + MINIMAP_ARROW_COLOR_DUNGEON) == 0xA3);
}

void test_increment_gameplay_subtype(void) {
    printf("[*] Running IncrementGameplaySubtype tests (01:44D6)...\n");
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wGameplaySubtype, 5);
    IncrementGameplaySubtype(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 6);

    IncrementGameplaySubtypeAndReturn(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 7);
}


void test_func_001_5888(void) {
    printf("[*] Running func_001_5888 tests (01:5888)...\n");
    GBState gb;
    gb_init(&gb);

    for (int i = 0; i < 0x0C; i++) {
        gb_write(&gb, wRoomTransitionState + i, 0xAA);
    }
    gb_write(&gb, wRoomTransitionState - 1, 0x55);
    gb_write(&gb, wRoomTransitionState + 0x0C, 0x55);

    func_001_5888(&gb);

    assert(gb_read(&gb, wRoomTransitionState - 1) == 0x55);
    for (int i = 0; i < 0x0C; i++) {
        assert(gb_read(&gb, wRoomTransitionState + i) == 0x00);
    }
    assert(gb_read(&gb, wRoomTransitionState + 0x0C) == 0x55);
}

void test_initialize_inventory_bar(void) {
    printf("[*] Running InitializeInventoryBar tests (01:5895)...\n");
    GBState gb;
    gb_init(&gb);

    InitializeInventoryBar(&gb);

    assert(gb_read(&gb, wWindowY) == 0x80);
    assert(gb_read(&gb, rWX) == 0x07);
    assert(gb_read(&gb, wSubscreenScrollIncrement) == 0x08);
    assert(gb_read(&gb, wInventoryAppearing) == 0x00);
}

void test_func_001_58A8(void) {
    printf("[*] Running func_001_58A8 tests (01:58A8)...\n");
    GBState gb;

    /* 1. DMG mode */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 0);
    gb_write(&gb, wDB54, 0x56);
    gb_write(&gb, hFrameCounter, 0x08);

    func_001_58A8(&gb);

    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6C) == 0x40);
    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6D) == 0x48);
    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6E) == 0x3E);
    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6F) == 0x10);

    /* 2. GBC mode with frame bit 3 set */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wDB54, 0x56);
    gb_write(&gb, hFrameCounter, 0x08);

    func_001_58A8(&gb);

    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6F) == 0x03);

    /* 3. GBC mode with frame bit 3 cleared */
    gb_write(&gb, hFrameCounter, 0x00);
    func_001_58A8(&gb);
    assert(gb_read(&gb, wDynamicOAMBuffer + 0x6F) == 0x00);
}

void test_peach_picture_state_2(void) {
    printf("[*] Running PeachPictureState2Handler tests (01:6856)...\n");
    GBState gb;

    /* 1. Eagles Tower */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_EAGLES_TOWER);
    gb_write(&gb, wGameplaySubtype, 2);
    PeachPictureState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_EAGLES_TOWER_TOP);
    assert(gb_read(&gb, wC13F) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    /* 2. Schule House */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, ROOM_INDOOR_B_SCHULE_HOUSE);
    gb_write(&gb, wGameplaySubtype, 2);
    PeachPictureState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_SCHULE_PAINTING);
    assert(gb_read(&gb, wC13F) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    /* 3. Christine */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, 0x00);
    gb_write(&gb, wGameplaySubtype, 2);
    PeachPictureState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_CHRISTINE);
    assert(gb_read(&gb, wC13F) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}

void test_peach_picture_state_3(void) {
    printf("[*] Running PeachPictureState3Handler tests (01:6873)...\n");
    GBState gb;

    /* 1. Eagles Tower Collapse */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_EAGLES_TOWER);
    gb_write(&gb, wGameplaySubtype, 3);
    PeachPictureState3Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_EAGLES_TOWER_COLLAPSE);
    assert(gb_read(&gb, wWindowY) == 0xFF);
    assert(gb_read(&gb, hBaseScrollX) == 0);
    assert(gb_read(&gb, hBaseScrollY) == 0);
    assert(gb_read(&gb, wTransitionSequenceCounter) == 0);
    assert(gb_read(&gb, wC16C) == 0);
    for (int i = 0; i < 8; i++) {
        assert(gb_read(&gb, wD210 + i) == 0);
    }
    assert(gb_read(&gb, wPaletteUnknownE) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* 2. Schule Painting */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, ROOM_INDOOR_B_SCHULE_HOUSE);
    gb_write(&gb, wGameplaySubtype, 3);
    PeachPictureState3Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_SCHULE_PAINTING);

    /* 3. Peach */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, 0x00);
    gb_write(&gb, wGameplaySubtype, 3);
    PeachPictureState3Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_PEACH);
}


void test_func_001_695B(void) {
    printf("[*] Running func_001_695B tests (01:695B)...\n");
    GBState gb;

    /* 1. wD215 == 0 -> returns, shake is 0 */
    gb_init(&gb);
    gb_write(&gb, wScreenShakeVertical, 5);
    gb_write(&gb, wD215, 0);
    func_001_695B(&gb);
    assert(gb_read(&gb, wScreenShakeVertical) == 0);
    assert(gb_read(&gb, wD215) == 0);

    /* 2. wD215 == 5 -> decrements to 4, (4 & 4) != 0 -> shake is 0x00 */
    gb_write(&gb, wD215, 5);
    func_001_695B(&gb);
    assert(gb_read(&gb, wD215) == 4);
    assert(gb_read(&gb, wScreenShakeVertical) == 0x00);

    /* 3. wD215 == 4 -> decrements to 3, (3 & 4) == 0 -> shake is 0xFE */
    func_001_695B(&gb);
    assert(gb_read(&gb, wD215) == 3);
    assert(gb_read(&gb, wScreenShakeVertical) == 0xFE);
}

void test_func_6A7C(void) {
    printf("[*] Running func_6A7C tests (01:6A7C)...\n");
    GBState gb;

    /* 1. Not Eagles Tower -> early return */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hActiveEntityPosX, 0x12);
    func_6A7C(&gb);
    assert(gb_read(&gb, hActiveEntityPosX) == 0x12);

    /* 2. Eagles Tower */
    gb_write(&gb, hMapId, MAP_EAGLES_TOWER);
    gb_write(&gb, wD214, 0);
    gb_write(&gb, wD211, 0x10);
    gb_write(&gb, wD213, 0);
    gb_write(&gb, wScreenShakeVertical, 0);
    func_6A7C(&gb);
    assert(gb_read(&gb, hActiveEntityPosX) == 0x48);
    assert(gb_read(&gb, hActiveEntityVisualPosY) == 0x30);
    assert(gb_read(&gb, wOAMNextAvailableSlot) == 0);
}

void test_peach_picture_state_4(void) {
    printf("[*] Running PeachPictureState4Handler tests (01:68AA)...\n");
    GBState gb;
    gb_init(&gb);

    /* When wTransitionSequenceCounter != 4 */
    gb_write(&gb, wGameplaySubtype, 4);
    gb_write(&gb, wTransitionSequenceCounter, 2);
    PeachPictureState4Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* When wTransitionSequenceCounter == 4 */
    gb_write(&gb, wTransitionSequenceCounter, 4);
    PeachPictureState4Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 5);
    assert(gb_read(&gb, wD210) == 0x80);
}

void test_peach_picture_state_5_and_68D9(void) {
    printf("[*] Running PeachPictureState5Handler & func_001_68D9 tests (01:68C0, 01:68D9)...\n");
    GBState gb;

    /* 1. Eagles Tower -> immediately sets subtype 7 */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_EAGLES_TOWER);
    gb_write(&gb, wGameplaySubtype, 5);
    PeachPictureState5Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 7);

    /* 2. Other map, no button pressed -> does not advance */
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, wGameplaySubtype, 5);
    gb_write(&gb, hJoypadState, 0);
    PeachPictureState5Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 5);

    /* 3. Button A pressed -> sets jingle and calls func_001_68D9 */
    gb_write(&gb, hJoypadState, J_A);
    PeachPictureState5Handler(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);
    assert(gb_read(&gb, wGameplaySubtype) == 6);
    assert(gb_read(&gb, wTransitionSequenceCounter) == 0);
    assert(gb_read(&gb, wC16C) == 0);
}

void test_peach_picture_state_7(void) {
    printf("[*] Running PeachPictureState7Handler tests (01:68E4)...\n");
    GBState gb;
    gb_init(&gb);

    /* 1. wD210 > 1 -> decrements, updates shake */
    gb_write(&gb, wGameplaySubtype, 7);
    gb_write(&gb, wD210, 5);
    PeachPictureState7Handler(&gb);
    assert(gb_read(&gb, wD210) == 4);
    assert(gb_read(&gb, wScreenShakeVertical) == 0xFE);
    assert(gb_read(&gb, wGameplaySubtype) == 7);

    /* 2. wD210 == 1 -> reaches 0, resets shake, sets wD210=0x20, advances to state 8 */
    gb_write(&gb, wD210, 1);
    PeachPictureState7Handler(&gb);
    assert(gb_read(&gb, wScreenShakeVertical) == 0);
    assert(gb_read(&gb, wD210) == 0x20);
    assert(gb_read(&gb, wGameplaySubtype) == 8);
}


void test_peach_picture_state_8(void) {
    printf("[*] Running PeachPictureState8Handler tests (01:6908)...\n");
    GBState gb;
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, wGameplaySubtype, 8);

    /* 1. wD210 > 1 -> decrements wD210, stays in state 8 */
    gb_write(&gb, wD210, 5);
    PeachPictureState8Handler(&gb);
    assert(gb_read(&gb, wD210) == 4);
    assert(gb_read(&gb, wGameplaySubtype) == 8);

    /* 2. wD210 == 1 -> explosion, resets timers, wD213 goes from 0 to 1 */
    gb_write(&gb, wD210, 1);
    gb_write(&gb, wD211, 0x10);
    gb_write(&gb, wD213, 0);
    PeachPictureState8Handler(&gb);
    assert(gb_read(&gb, wD210) == 0x30);
    assert(gb_read(&gb, wD214) == 0x30);
    assert(gb_read(&gb, wD215) == 0x18);
    assert(gb_read(&gb, wD211) == 0x18);
    assert(gb_read(&gb, wD213) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 8);

    /* 3. wD210 == 1 with wD213 == 3 -> wD213 becomes 4, advances to state 9 with wD210 = 0x80 */
    gb_write(&gb, wD210, 1);
    gb_write(&gb, wD213, 3);
    PeachPictureState8Handler(&gb);
    assert(gb_read(&gb, wD213) == 4);
    assert(gb_read(&gb, wD210) == 0x80);
    assert(gb_read(&gb, wGameplaySubtype) == 9);
}

void test_peach_picture_state_9(void) {
    printf("[*] Running PeachPictureState9Handler tests (01:6945)...\n");
    GBState gb;
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, wGameplaySubtype, 9);

    /* 1. wD210 > 1 -> decrements, stays in state 9 */
    gb_write(&gb, wD210, 3);
    PeachPictureState9Handler(&gb);
    assert(gb_read(&gb, wD210) == 2);
    assert(gb_read(&gb, wGameplaySubtype) == 9);

    /* 2. wD210 == 1 -> decrements to 0, advances to state 10 (0x0A) */
    gb_write(&gb, wD210, 1);
    gb_write(&gb, wTransitionSequenceCounter, 5);
    gb_write(&gb, wC16C, 2);
    PeachPictureState9Handler(&gb);
    assert(gb_read(&gb, wD210) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 0x0A);
    assert(gb_read(&gb, wTransitionSequenceCounter) == 0);
    assert(gb_read(&gb, wC16C) == 0);
}

void test_file_save_fade_out_and_state_A(void) {
    printf("[*] Running FileSaveFadeOut & PeachPictureStateAHandler tests (01:5822, 01:5825)...\n");
    GBState gb;
    gb_init(&gb);

    /* 1. Transition counter != 4 -> does nothing */
    gb_write(&gb, wTransitionSequenceCounter, 2);
    FileSaveFadeOut(&gb);
    assert(gb_read(&gb, wGameplayType) == 0);

    /* 2. Transition counter == 4 on CGB */
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    gb_write(&gb, wIsIndoor, 0);

    /* Set byte in bank 3 wBGPal1 */
    gb_write(&gb, rSVBK, 3);
    gb_write(&gb, wBGPal1, 0x55);
    gb_write(&gb, wIsFileSelectionArrowShifted, 0xFF);

    PeachPictureStateAHandler(&gb);

    /* Check palette copied to bank 2 */
    gb_write(&gb, rSVBK, 2);
    assert(gb_read(&gb, wBGPal1) == 0x55);
    gb_write(&gb, rSVBK, 0);

    /* Check return to world gameplay */
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_WORLD);
    assert(gb_read(&gb, wGameplaySubtype) == GAMEPLAY_WORLD_LOAD_2);
    assert(gb_read(&gb, hVolumeRight) == 7);
    assert(gb_read(&gb, hVolumeLeft) == 0x70);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_BASE_OVERWORLD_DUP);
    assert(gb_read(&gb, wWindowY) == 0x80);
}

void test_peach_picture_state_0_and_1(void) {
    printf("[*] Running PeachPictureState0Handler & 1 tests (01:6808, 01:6829)...\n");
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wGameplaySubtype, 0);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);

    /* Set byte in bank 1 wBGPal1 */
    gb_write(&gb, rSVBK, 0);
    gb_write(&gb, wBGPal1, 0x33);

    PeachPictureState0Handler(&gb);

    /* Check palette copied to bank 3 */
    gb_write(&gb, rSVBK, 3);
    assert(gb_read(&gb, wBGPal1) == 0x33);
    gb_write(&gb, rSVBK, 0);

    /* Check state 1 execution */
    assert(gb_read(&gb, wGameplaySubtype) == 2);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_0F);
    assert(gb_read(&gb, hVolumeRight) == 3);
    assert(gb_read(&gb, hVolumeLeft) == 0x30);
    assert(gb_read(&gb, wScrollXOffset) == 0);
}

void test_peach_picture_entry_point(void) {
    printf("[*] Running PeachPictureEntryPoint tests (01:67EE)...\n");
    GBState gb;
    gb_init(&gb);

    /* Subtype 2 dispatches to state 2 */
    gb_write(&gb, wGameplaySubtype, 2);
    gb_write(&gb, hMapId, MAP_EAGLES_TOWER);
    PeachPictureEntryPoint(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_EAGLES_TOWER_TOP);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}


void test_play_validation_jingle(void) {
    printf("[*] Running PlayValidationJingle tests (01:49BE)...\n");
    GBState gb;
    gb_init(&gb);
    PlayValidationJingle(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);
}

void test_func_001_5A59(void) {
    printf("[*] Running func_001_5A59 tests (01:5A59)...\n");
    GBState gb;
    gb_init(&gb);

    /* Room 0x06 has special table entry 0x3E -> lookup 0x3E is 0x5E */
    gb_write(&gb, hMapRoom, 0x06);
    gb_write(&gb, hLinkPositionY, 0x50); /* Place dialog at top */
    func_001_5A59(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0x5E);
    assert((gb_read(&gb, wDialogState) & 0x7F) == 1);
}

void test_world_map_states(void) {
    printf("[*] Running WorldMapState0-4 tests (01:5648-01:570B)...\n");
    GBState gb;
    gb_init(&gb);

    /* Test State 2 */
    gb_write(&gb, wGameplaySubtype, 2);
    WorldMapState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_WORLD_MAP);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    /* Test State 3 */
    WorldMapState3Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_WORLD_MAP_TILEMAP);
    assert(gb_read(&gb, wPaletteUnknownE) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* Test State 4 (counter != 4) */
    gb_write(&gb, wTransitionSequenceCounter, 2);
    WorldMapState4Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* Test State 4 (counter == 4) */
    gb_write(&gb, wTransitionSequenceCounter, 4);
    WorldMapState4Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 5);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);

    /* Test State 1 with Owl marker check */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    gb_write(&gb, wDB54, 0x16); /* room 0x16: MapSpecialLocationNamesTable has 0x06 (Owl reminder) */
    gb_write(&gb, wC5A2, 0);
    gb_write(&gb, (uint16_t)(wOverworldRoomStatus + 0x16), OW_ROOM_STATUS_OWL_TALKED);
    gb_write(&gb, rLCDC, LCDCF_WINON | LCDCF_ON);

    WorldMapState1Handler(&gb);
    assert(gb_read(&gb, hVolumeRight) == 3);
    assert(gb_read(&gb, hVolumeLeft) == 0x30);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    assert(gb_read(&gb, wDBB4) == 0x16);
    assert(gb_read(&gb, wC1B1) == 1); /* Owl talked -> 1 */
    assert(gb_read(&gb, wC1B4) == 0x16);
    assert((gb_read(&gb, rLCDC) & LCDCF_WINON) == 0);
    assert((gb_read(&gb, wLCDControl) & LCDCF_WINON) == 0);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_WORLD_MAP);

    /* Test State 1 with dungeon icon (room 0x0E: 0x17 -> category 1 -> icon 2) */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    gb_write(&gb, wDB54, 0x0E);
    WorldMapState1Handler(&gb);
    assert(gb_read(&gb, wC1B1) == 2);

    /* Test State 0 on CGB */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wGameplaySubtype, 0);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    gb_write(&gb, wDB54, 0x00);
    gb_write(&gb, rSVBK, 0);
    gb_write(&gb, wBGPal1, 0x77);

    WorldMapState0Handler(&gb);
    /* Subtype was 0 -> State0 increments to 1 -> State1 increments to 2 */
    assert(gb_read(&gb, wGameplaySubtype) == 2);
    /* Verify palette copied to bank 3 */
    gb_write(&gb, rSVBK, 3);
    assert(gb_read(&gb, wBGPal1) == 0x77);
    assert(gb_read(&gb, wIsFileSelectionArrowShifted) == 1);
    gb_write(&gb, rSVBK, 0);
}


void test_move_select_and_jingle(void) {
    printf("[*] Running MoveSelect and playMoveSelectionJingle tests (01:6BA8, 01:6BAE)...\n");
    GBState gb;
    gb_init(&gb);

    playMoveSelectionJingle(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_MOVE_SELECTION);

    gb_init(&gb);
    gb_write(&gb, hJoypadState, J_LEFT);
    MoveSelect(&gb);
    assert(gb_read(&gb, hJingle) == 0);

    gb_write(&gb, hJoypadState, J_UP);
    MoveSelect(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_MOVE_SELECTION);
}

void test_label_001_5B3F(void) {
    printf("[*] Running label_001_5B3F tests (01:5B3F)...\n");
    GBState gb;
    gb_init(&gb);

    /* Room 0x35: Y=3, X=5 -> e = 3*8 + 0x14 = 0x2C, d = 5*8 + 0x14 = 0x3C */
    gb_write(&gb, wDBB4, 0x35);
    gb_write(&gb, hFrameCounter, 0x00); /* Bit 4 = 0 -> render arrows */

    label_001_5B3F(&gb);

    uint16_t oam = (uint16_t)(wDynamicOAMBuffer + 0x50);
    assert(gb_read(&gb, oam + 0) == 0x2C);
    assert(gb_read(&gb, oam + 1) == 0x3C);
    assert(gb_read(&gb, oam + 2) == 0xF0);
    assert(gb_read(&gb, oam + 3) == 0x00);

    assert(gb_read(&gb, oam + 4) == 0x2C);
    assert(gb_read(&gb, oam + 5) == 0x44);
    assert(gb_read(&gb, oam + 6) == 0xF0);
    assert(gb_read(&gb, oam + 7) == 0x20);

    /* Arrows present */
    uint16_t arrow = (uint16_t)(wDynamicOAMBuffer + 0x58);
    assert(gb_read(&gb, arrow + 0) == (uint8_t)(0x2C + 4));
    assert(gb_read(&gb, arrow + 1) == (uint8_t)(0x3C + 0xF6));
    assert(gb_read(&gb, arrow + 2) == 0xF6);

    /* Test bit 4 = 1 -> skip arrows */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x35);
    gb_write(&gb, hFrameCounter, 0x10);
    label_001_5B3F(&gb);
    assert(gb_read(&gb, oam + 0) == 0x2C);
    assert(gb_read(&gb, arrow + 0) == 0x00);
}

void test_func_001_5A71(void) {
    printf("[*] Running func_001_5A71 tests (01:5A71)...\n");
    GBState gb;
    gb_init(&gb);

    /* Case 1: Dialog active -> cursor does not move */
    gb_write(&gb, wDBB4, 0x10);
    gb_write(&gb, wDialogState, 1);
    gb_write(&gb, hPressedButtonsMask, J_RIGHT);
    func_001_5A71(&gb);
    assert(gb_read(&gb, wDBB4) == 0x10);

    /* Case 2: Move right to unvisited room -> blocked by fog of war, plays bump */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x10);
    gb_write(&gb, (uint16_t)(wOverworldRoomStatus + 0x10), 1);
    gb_write(&gb, (uint16_t)(wOverworldRoomStatus + 0x11), 0); /* unvisited */
    gb_write(&gb, hPressedButtonsMask, J_RIGHT);
    gb_write(&gb, hJoypadState, J_RIGHT);
    func_001_5A71(&gb);
    assert(gb_read(&gb, wDBB4) == 0x10); /* restored */
    assert(gb_read(&gb, hJingle) == JINGLE_BUMP);

    /* Case 3: Move right to visited room -> allowed, plays selection jingle */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x10);
    gb_write(&gb, (uint16_t)(wOverworldRoomStatus + 0x11), 1); /* visited */
    gb_write(&gb, hPressedButtonsMask, J_RIGHT);
    gb_write(&gb, hJoypadState, J_RIGHT);
    func_001_5A71(&gb);
    assert(gb_read(&gb, wDBB4) == 0x11);
    assert(gb_read(&gb, hJingle) == JINGLE_MOVE_SELECTION);

    /* Case 4: Free movement mode allows unvisited room */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x10);
    gb_write(&gb, wFreeMovementMode, 1);
    gb_write(&gb, hPressedButtonsMask, J_RIGHT);
    gb_write(&gb, hJoypadState, J_RIGHT);
    func_001_5A71(&gb);
    assert(gb_read(&gb, wDBB4) == 0x11);
    assert(gb_read(&gb, hJingle) == JINGLE_MOVE_SELECTION);

    /* Case 5: Auto-repeat counter test */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x20);
    gb_write(&gb, wFreeMovementMode, 1);
    gb_write(&gb, wC182, 0x17);
    gb_write(&gb, hPressedButtonsMask, J_RIGHT);
    func_001_5A71(&gb);
    assert(gb_read(&gb, wC182) == 0x15); /* loops back to 0x15 */
}


void test_func_001_5C49_and_5C55(void) {
    printf("[*] Running func_001_5C49 and func_001_5C55 tests (01:5C49, 01:5C55)...\n");
    GBState gb;
    gb_init(&gb);

    /* Test 1: Preservation of wEntitiesPhysicsFlagsTable */
    gb_write(&gb, wEntitiesPhysicsFlagsTable, 0x42);
    gb_write(&gb, wC1B1, 2); /* Dungeon icon */
    gb_write(&gb, wC1B2, 0);
    gb_write(&gb, wC1B3, 0);
    gb_write(&gb, wC1B4, 0x12); /* Room (1, 2) -> Top-right corner (X=0x78, Y=0x28) */

    func_001_5C49(&gb);
    assert(gb_read(&gb, wEntitiesPhysicsFlagsTable) == 0x42);
    assert(gb_read(&gb, hActiveEntityPosX) == 0x78);
    assert(gb_read(&gb, hActiveEntityVisualPosY) == 0x28);

    /* Check written OAM sprite tiles for dungeon icon (variant 1: tiles 0x24, 0x26) */
    assert(gb_read(&gb, wDynamicOAMBuffer + 2) == 0x24);
    assert(gb_read(&gb, wDynamicOAMBuffer + 6) == 0x26);

    /* Test 2: Room (8, 9) -> Bottom-left corner (X=0x28, Y=0x78) */
    gb_init(&gb);
    gb_write(&gb, wC1B1, 1); /* Owl icon */
    gb_write(&gb, wC1B4, 0x89);
    func_001_5C49(&gb);
    assert(gb_read(&gb, hActiveEntityPosX) == 0x28);
    assert(gb_read(&gb, hActiveEntityVisualPosY) == 0x78);
}

void test_world_map_interactive_and_entry_point(void) {
    printf("[*] Running WorldMapInteractiveHandler and WorldMapEntryPoint tests (01:571C, 01:5626)...\n");
    GBState gb;
    gb_init(&gb);

    /* Test 1a: Interactive handler with A button on Bottle Grotto (room 0x24) -> opens dialog 0x57 */
    gb_write(&gb, wDBB4, 0x24);
    gb_write(&gb, hJoypadState, J_A);
    WorldMapInteractiveHandler(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0x57);
    assert(gb_read(&gb, wDialogState) == 0x81); /* room < 0x70 -> 0x81 */

    /* Test 1b: Interactive handler with A button on Goponga Swamp (room 0x34) -> opens dialog 0x76 */
    gb_init(&gb);
    gb_write(&gb, wDBB4, 0x34);
    gb_write(&gb, hJoypadState, J_A);
    WorldMapInteractiveHandler(&gb);
    assert(gb_read(&gb, wDialogIndex) == 0x76);
    assert(gb_read(&gb, wDialogState) == 0x81);

    /* Test 2: Interactive handler with B button -> exits map (advances subtype) */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 5);
    gb_write(&gb, hJoypadState, J_B);
    WorldMapInteractiveHandler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 6);
    assert(gb_read(&gb, wPaletteUnknownE) == 1);

    /* Test 3: WorldMapEntryPoint when subtype != 5 clears inputs */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    gb_write(&gb, hJoypadState, J_B);
    gb_write(&gb, hPressedButtonsMask, J_B);
    WorldMapEntryPoint(&gb);
    assert(gb_read(&gb, hJoypadState) == 0);
    assert(gb_read(&gb, hPressedButtonsMask) == 0);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_WORLD_MAP);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}


void test_build_save_slot_hearts_draw_command(void) {
    printf("[*] Running BuildSaveSlotHeartsDrawCommand tests (01:5D53)...\n");
    GBState gb;
    gb_init(&gb);

    /* Test 1: Slot 0, 3 full hearts (health = 24), max hearts = 5 */
    gb_write(&gb, wDrawCommandsSize, 0);
    gb_write(&gb, hMultiPurpose4, 0); /* Slot 0 */
    gb_write(&gb, hMultiPurpose2, 24); /* Health = 3 full hearts */
    gb_write(&gb, hMultiPurpose3, 5);  /* Max health = 5 containers */

    BuildSaveSlotHeartsDrawCommand(&gb);
    assert(gb_read(&gb, wDrawCommandsSize) == 0x14);

    /* Row 1 header */
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xCB);
    assert(gb_read(&gb, wDrawCommand + 2) == 0x06);

    /* Verify 5 hearts drawn with tile 0xAE */
    for (int i = 0; i < 5; i++) {
        assert(gb_read(&gb, wDrawCommand + 3 + i) == 0xAE);
    }
    /* Remaining 2 tiles in row 1 are empty (0x7E) */
    assert(gb_read(&gb, wDrawCommand + 3 + 5) == 0x7E);
    assert(gb_read(&gb, wDrawCommand + 3 + 6) == 0x7E);

    /* Test 2: Slot 1, 10 hearts (spans row 1 and row 2) */
    gb_init(&gb);
    gb_write(&gb, wDrawCommandsSize, 0);
    gb_write(&gb, hMultiPurpose4, 1); /* Slot 1 */
    gb_write(&gb, hMultiPurpose2, 80); /* 10 hearts */
    gb_write(&gb, hMultiPurpose3, 10);

    BuildSaveSlotHeartsDrawCommand(&gb);
    /* Row 1 destination */
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 1) == 0x2B);
    /* All 7 tiles in row 1 are hearts (0xAE) */
    for (int i = 0; i < 7; i++) {
        assert(gb_read(&gb, wDrawCommand + 3 + i) == 0xAE);
    }
    /* Row 2 destination header (offset 10..12) */
    assert(gb_read(&gb, wDrawCommand + 10) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 11) == 0x4B);
    assert(gb_read(&gb, wDrawCommand + 12) == 0x06);
    /* 3 hearts in row 2 */
    for (int i = 0; i < 3; i++) {
        assert(gb_read(&gb, wDrawCommand + 13 + i) == 0xAE);
    }
    /* Remaining 4 tiles in row 2 are 0x7E */
    for (int i = 3; i < 7; i++) {
        assert(gb_read(&gb, wDrawCommand + 13 + i) == 0x7E);
    }
}

void test_func_5DC0_and_save_game_to_file(void) {
    printf("[*] Running func_5DC0 and SaveGameToFile tests (01:5DC0, 01:5DE6)...\n");
    GBState gb;
    gb_init(&gb);

    /* Test 1: func_5DC0 with no names */
    func_5DC0(&gb);
    assert(gb_read(&gb, wSaveFilesCount) == 0);

    /* Slot 0 populated: 'L', 'I', 'N', 'K', 0 */
    gb_write(&gb, wSaveSlotNames + 0, 'L');
    func_5DC0(&gb);
    assert(gb_read(&gb, wSaveFilesCount) == 1);

    /* Slot 2 populated: slot 2 starts at index 10 */
    gb_write(&gb, wSaveSlotNames + 10, 'Z');
    func_5DC0(&gb);
    assert(gb_read(&gb, wSaveFilesCount) == 5); /* 1 | 4 */

    /* Slot 1 populated: slot 1 starts at index 5 */
    gb_write(&gb, wSaveSlotNames + 5, 'M');
    func_5DC0(&gb);
    assert(gb_read(&gb, wSaveFilesCount) == 7); /* 1 | 2 | 4 */

    /* Test 2: SaveGameToFile */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, wHealth, 0); /* 0 health should reset to starting health */
    gb_write(&gb, wMaxHearts, 6); /* MaxHeartsToStartingHealthTable[6] == 40 */
    gb_write(&gb, wOverworldRoomStatus + 0x10, 0x55);
    gb_write(&gb, wColorDungeonItemFlags + 1, 0xAA);
    gb_write(&gb, wColorDungeonRoomStatus + 2, 0x77);
    gb_write(&gb, wTunicType, 2);
    gb_write(&gb, wPhotos1, 0x11);
    gb_write(&gb, wPhotos2, 0x22);

    SaveGameToFile(&gb);
    assert(gb_read(&gb, wHealth) == 40);

    /* Verify SRAM destination 0xA105 (SaveGame1.main) */
    assert(gb_read(&gb, 0xA105 + 0x10) == 0x55);
    /* DX1 offset: 0xA105 + 0x380 = 0xA485 */
    assert(gb_read(&gb, 0xA485 + 1) == 0xAA);
    /* DX2 offset: 0xA485 + 5 = 0xA48A */
    assert(gb_read(&gb, 0xA48A + 2) == 0x77);
    /* DX3 offset: 0xA48A + 0x20 = 0xA4AA */
    assert(gb_read(&gb, 0xA4AA + 0) == 2);    /* wTunicType */
    assert(gb_read(&gb, 0xA4AA + 1) == 0x11); /* wPhotos1 */
    assert(gb_read(&gb, 0xA4AA + 2) == 0x22); /* wPhotos2 */

    /* Test 3: Save slot 1 (dest: 0xA4B2) */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 1);
    gb_write(&gb, wHealth, 20);
    gb_write(&gb, wOverworldRoomStatus + 0x05, 0x99);
    SaveGameToFile(&gb);
    assert(gb_read(&gb, 0xA4B2 + 0x05) == 0x99);
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
    test_load_tileset_0f_and_attributes();
    test_write_dma_code_to_hram();
    test_update_minimap_entrance_arrow();
    test_increment_gameplay_subtype();
    test_func_001_5888();
    test_initialize_inventory_bar();
    test_func_001_58A8();
    test_peach_picture_state_2();
    test_peach_picture_state_3();
    test_func_001_695B();
    test_func_6A7C();
    test_peach_picture_state_4();
    test_peach_picture_state_5_and_68D9();
    test_peach_picture_state_7();
    test_peach_picture_state_8();
    test_peach_picture_state_9();
    test_file_save_fade_out_and_state_A();
    test_peach_picture_state_0_and_1();
    test_peach_picture_entry_point();
    test_play_validation_jingle();
    test_func_001_5A59();
    test_world_map_states();
    test_move_select_and_jingle();
    test_label_001_5B3F();
    test_func_001_5A71();
    test_func_001_5C49_and_5C55();
    test_world_map_interactive_and_entry_point();
    test_build_save_slot_hearts_draw_command();
    test_func_5DC0_and_save_game_to_file();
    printf("  [PASS] All bank1 room transition & sprite functions verified successfully!\n\n");
}
