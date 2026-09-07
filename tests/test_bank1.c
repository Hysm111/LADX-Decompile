#include "constants/audio.h"
#include "bank1/file_menu.h"
#include "constants/directions.h"
#include "bank1/save.h"
#include "bank1/game_over.h"
#include "bank1/world_handler.h"
#include "bank1/face_shrine_mural.h"
#include "bank1/siren_instruments.h"
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


void test_load_saved_file(void) {
    printf("[*] Running LoadSavedFile tests (01:52A4)...\n");
    GBState gb;

    /* Test 1: New game initialization when wSpawnPositionX is 0 */
    gb_init(&gb);
    /* In a new game file, SRAM has wSpawnPositionX = 0, health = 24 */
    gb_write(&gb, 0xA105 + (wHealth - wOverworldRoomStatus), 24);
    gb_write(&gb, 0xA105 + (wMaxHearts - wOverworldRoomStatus), 3);
    gb_write(&gb, 0xA105 + (wSpawnPositionX - wOverworldRoomStatus), 0);

    LoadSavedFile(&gb);
    assert(gb_read(&gb, wHealth) == 24); /* 3 hearts = 24 */
    assert(gb_read(&gb, wMaxArrows) == 0x30);
    assert(gb_read(&gb, wMaxBombs) == 0x30);
    assert(gb_read(&gb, wMaxMagicPowder) == 0x20);
    assert(gb_read(&gb, wMapEntranceRoom) == ROOM_INDOOR_B_MARIN_HOUSE);
    assert(gb_read(&gb, hMapRoom) == ROOM_INDOOR_B_MARIN_HOUSE);
    assert(gb_read(&gb, wDB54) == ROOM_INDOOR_B_MARIN_HOUSE);
    assert(gb_read(&gb, wIsIndoor) == 1);
    assert(gb_read(&gb, hMapId) == MAP_HOUSE);
    assert(gb_read(&gb, wMapEntrancePositionX) == 0x50);
    assert(gb_read(&gb, wMapEntrancePositionY) == 0x60);
    assert(gb_read(&gb, hLinkDirection) == DIRECTION_DOWN);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_INVENTORY);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_WORLD);

    /* Test 2: Loading existing save from SRAM slot 0 with indoor spawn */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    /* Populate SRAM for slot 0 (SaveGame1.main: 0xA105) */
    gb_write(&gb, 0xA105 + 0x20, 0x7E); /* room status */
    /* DX1: 0xA105 + 0x380 = 0xA485 */
    gb_write(&gb, 0xA485 + 2, 0x33);
    /* DX2: 0xA485 + 5 = 0xA48A */
    gb_write(&gb, 0xA48A + 3, 0x44);
    /* DX3: 0xA48A + 0x20 = 0xA4AA */
    gb_write(&gb, 0xA4AA + 0, 1);    /* tunic */
    gb_write(&gb, 0xA4AA + 1, 0x88); /* photo1 */
    gb_write(&gb, 0xA4AA + 2, 0x99); /* photo2 */

    /* Set up spawn variables in SRAM room status / WRAM */
    gb_write(&gb, 0xA105 + (wSpawnPositionX - wOverworldRoomStatus), 0x48);
    gb_write(&gb, 0xA105 + (wSpawnPositionY - wOverworldRoomStatus), 0x52);
    gb_write(&gb, 0xA105 + (wSpawnMapRoom - wOverworldRoomStatus), 0x37);
    gb_write(&gb, 0xA105 + (wSpawnMapId - wOverworldRoomStatus), 0x05);
    gb_write(&gb, 0xA105 + (wSpawnIndoorRoom - wOverworldRoomStatus), 0x12);
    gb_write(&gb, 0xA105 + (wSpawnIsIndoor - wOverworldRoomStatus), 0x01);

    LoadSavedFile(&gb);
    /* Check loaded SRAM data */
    assert(gb_read(&gb, wOverworldRoomStatus + 0x20) == 0x7E);
    assert(gb_read(&gb, wColorDungeonItemFlags + 2) == 0x33);
    assert(gb_read(&gb, wColorDungeonRoomStatus + 3) == 0x44);
    assert(gb_read(&gb, wTunicType) == 1);
    assert(gb_read(&gb, wPhotos1) == 0x88);
    assert(gb_read(&gb, wPhotos2) == 0x99);

    /* Check spawn positions and directions */
    assert(gb_read(&gb, wMapEntrancePositionX) == 0x48);
    assert(gb_read(&gb, wMapEntrancePositionY) == 0x52);
    assert(gb_read(&gb, hMapRoom) == 0x37);
    assert(gb_read(&gb, wMapEntranceRoom) == 0x37);
    assert(gb_read(&gb, hMapId) == 0x05);
    assert(gb_read(&gb, wIndoorRoom) == 0x12);
    assert(gb_read(&gb, wIsIndoor) == 1);
    assert(gb_read(&gb, hLinkDirection) == DIRECTION_UP);
    assert(gb_read(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_STANDING_UP);

    /* Test 3: wDBD1 != 0 skips SRAM loading and restores health from max hearts if 0 */
    gb_init(&gb);
    gb_write(&gb, wDBD1, 1);
    gb_write(&gb, wHealth, 0);
    gb_write(&gb, wMaxHearts, 6); /* MaxHeartsToStartingHealthTable[6] == 40 */
    gb_write(&gb, wSpawnPositionX, 0x48);
    LoadSavedFile(&gb);
    assert(gb_read(&gb, wDBD1) == 0);
    assert(gb_read(&gb, wHealth) == 40);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_WORLD);
}


void test_func_001_4954(void) {
    printf("[*] Running func_001_4954 tests (01:4954)...\n");
    GBState gb;

    /* Slot 0: Y = 0x3B. Frame counter bit 3 set: normal frame */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, hFrameCounter, 0x08);
    func_001_4954(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == 0x3B);
    assert(gb_read(&gb, wOAMBuffer + 1) == 0x18);
    assert(gb_read(&gb, wOAMBuffer + 2) == 0x00);
    assert(gb_read(&gb, wOAMBuffer + 3) == 0x00);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x3B);
    assert(gb_read(&gb, wOAMBuffer + 5) == 0x20);
    assert(gb_read(&gb, wOAMBuffer + 6) == 0x02);
    assert(gb_read(&gb, wOAMBuffer + 7) == 0x00);

    /* Frame counter bit 3 clear: flipped frame (attr 0x20) */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, hFrameCounter, 0x00);
    func_001_4954(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == 0x3B);
    assert(gb_read(&gb, wOAMBuffer + 1) == 0x18);
    assert(gb_read(&gb, wOAMBuffer + 2) == 0x02);
    assert(gb_read(&gb, wOAMBuffer + 3) == 0x20);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x3B);
    assert(gb_read(&gb, wOAMBuffer + 5) == 0x20);
    assert(gb_read(&gb, wOAMBuffer + 6) == 0x00);
    assert(gb_read(&gb, wOAMBuffer + 7) == 0x20);

    /* Slot 1: Y = 0x53 */
    gb_write(&gb, wSaveSlot, 1);
    func_001_4954(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == 0x53);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x53);

    /* Slot 3: Y = 0x83 */
    gb_write(&gb, wSaveSlot, 3);
    func_001_4954(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == 0x83);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x83);
}

void test_file_selection_interactive_and_choice(void) {
    printf("[*] Running FileSelection interactive & choice tests (01:48E8-01:4A04)...\n");
    GBState gb;

    /* Test 1: Navigation without saved files (wraps 0..2) */
    gb_init(&gb);
    gb_write(&gb, wSaveFilesCount, 0);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, hJoypadState, J_UP);
    FileSelectionInteractiveHandler(&gb);
    assert(gb_read(&gb, wSaveSlot) == 2);

    /* DOWN from 2 wraps to 0 */
    gb_write(&gb, hJoypadState, J_DOWN);
    FileSelectionInteractiveHandler(&gb);
    assert(gb_read(&gb, wSaveSlot) == 0);

    /* Test 2: Navigation with saved files (wraps 0..3) */
    gb_write(&gb, wSaveFilesCount, 1);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, hJoypadState, J_UP);
    FileSelectionInteractiveHandler(&gb);
    assert(gb_read(&gb, wSaveSlot) == 3);

    /* On slot 3, test left/right shifts arrow */
    gb_write(&gb, wIsFileSelectionArrowShifted, 0);
    gb_write(&gb, hJoypadState, J_RIGHT);
    gb_write(&gb, hFrameCounter, 0x00); /* Frame counter bit 4 clear: arrow visible */
    FileSelectionInteractiveHandler(&gb);
    assert(gb_read(&gb, wIsFileSelectionArrowShifted) == 1);
    assert(gb_read(&gb, wOAMBuffer + 8) == 0x88);
    assert(gb_read(&gb, wOAMBuffer + 9) == 0x64);
    assert(gb_read(&gb, wOAMBuffer + 10) == 0xBE);

    /* Test A button increments subtype */
    gb_write(&gb, wGameplaySubtype, 2);
    gb_write(&gb, hJoypadState, J_A);
    FileSelectionInteractiveHandler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    /* Test 3: HandleFileSelectionCommand */
    gb_init(&gb);
    gb_write(&gb, wIsFileSelectionArrowShifted, 0);
    HandleFileSelectionCommand(&gb);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_DELETE);
    assert(gb_read(&gb, wGameplaySubtype) == 0);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);

    gb_write(&gb, wIsFileSelectionArrowShifted, 1);
    HandleFileSelectionCommand(&gb);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_COPY);

    /* Test 4: FileSelectionExecuteChoice on empty slot */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 1);
    /* Name is empty (all zeroes) */
    FileSelectionExecuteChoice(&gb);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_NEW);
    assert(gb_read(&gb, wGameplaySubtype) == 0);

    /* Test 5: FileSelectionExecuteChoice on populated slot */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, wSaveSlotNames + 0, 'Z');
    gb_write(&gb, wGameplaySubtype, 2);
    FileSelectionExecuteChoice(&gb);
    assert(gb_read(&gb, wBGPalette) == 0);
    assert(gb_read(&gb, wOBJ0Palette) == 0);
    assert(gb_read(&gb, wOBJ1Palette) == 0);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_BASE_OVERWORLD);
    assert(gb_read(&gb, wGameplaySubtype) == 3); /* incremented */

    /* Test 6: FileSelectionLoadSavedFile */
    gb_init(&gb);
    gb_write(&gb, wSpawnPositionX, 0);
    gb_write(&gb, wHealth, 0);
    gb_write(&gb, wMaxHearts, 3);
    FileSelectionLoadSavedFile(&gb);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_WORLD);
}


void test_file_creation_init_and_sram(void) {
    printf("[*] Running FileCreationInit & WriteByteToSRAM tests (01:4A11-01:4A46)...\n");
    GBState gb;

    /* Test 1: FileCreationInit1Handler */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 0);
    FileCreationInit1Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_FILL_TILEMAP);
    assert(gb_read(&gb, wDBA8) == 0);
    assert(gb_read(&gb, wNameEntryCurrentChar) == 0);
    assert(gb_read(&gb, wSaveSlotNameCharIndex) == 0);

    /* Test 2: FileCreationInit2Handler for Slot 1 */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 1);
    gb_write(&gb, wGameplaySubtype, 1);
    FileCreationInit2Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_MENU_FILE_CREATION);
    assert(gb_read(&gb, wDrawCommand + 0) == (FILE_NEW_SAVE_SLOT_INDEX_BG >> 8));
    assert(gb_read(&gb, wDrawCommand + 1) == (FILE_NEW_SAVE_SLOT_INDEX_BG & 0xFF));
    assert(gb_read(&gb, wDrawCommand + 2) == 0);
    assert(gb_read(&gb, wDrawCommand + 3) == (1 + FILE_NEW_SAVE_SLOT_1_TILE));
    assert(gb_read(&gb, wDrawCommand + 4) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 2);

    /* Test 3: WriteByteToSRAM */
    gb_init(&gb);
    WriteByteToSRAM(&gb, 0xA100, 0x05, 0x42);
    assert(gb_read(&gb, 0xA105) == 0x42);
}

void test_transition_to_file_menu_reload(void) {
    printf("[*] Running TransitionToFileMenu & label_001_4555 tests (01:4552, 01:4555)...\n");
    GBState gb;
    gb_init(&gb);

    /* Populate SRAM for Slot 1 */
    gb_write(&gb, 0xA454 + 0, 'L');
    gb_write(&gb, 0xA454 + 1, 'I');
    gb_write(&gb, 0xA454 + 2, 'N');
    gb_write(&gb, 0xA454 + 3, 'K');
    gb_write(&gb, 0xA454 + 4, '1');
    gb_write(&gb, 0xA45F, 24);
    gb_write(&gb, 0xA460, 3);
    gb_write(&gb, 0xA45C, 0);
    gb_write(&gb, 0xA45D, 2);

    /* Populate SRAM for Slot 2 */
    gb_write(&gb, 0xA801 + 0, 'Z');
    gb_write(&gb, 0xA801 + 1, 'E');
    gb_write(&gb, 0xA801 + 2, 'L');
    gb_write(&gb, 0xA801 + 3, 'D');
    gb_write(&gb, 0xA801 + 4, 'A');
    gb_write(&gb, 0xA80C, 40);
    gb_write(&gb, 0xA80D, 5);
    gb_write(&gb, 0xA809, 0);
    gb_write(&gb, 0xA80A, 0);

    /* Populate SRAM for Slot 3 */
    gb_write(&gb, 0xABAE + 0, 'M');
    gb_write(&gb, 0xABAE + 1, 'A');
    gb_write(&gb, 0xABAE + 2, 'R');
    gb_write(&gb, 0xABAE + 3, 'I');
    gb_write(&gb, 0xABAE + 4, 'N');
    gb_write(&gb, 0xABB9, 80);
    gb_write(&gb, 0xABBA, 10);
    gb_write(&gb, 0xABB6, 0);
    gb_write(&gb, 0xABB7, 7);

    TransitionToFileMenu(&gb, 1);
    assert(gb_read(&gb, wForceFileSelectionScreenMusic) == 1);

    /* Slot 1 checks */
    assert(gb_read(&gb, wSaveSlot1Name + 0) == 'L');
    assert(gb_read(&gb, wSaveSlot1Name + 4) == '1');
    assert(gb_read(&gb, wFile1Health) == 24);
    assert(gb_read(&gb, wFile1MaxHearts) == 3);
    assert(gb_read(&gb, wFile1DeathCountHigh) == 0);
    assert(gb_read(&gb, wFile1DeathCountLow) == 2);

    /* Slot 2 checks */
    assert(gb_read(&gb, wSaveSlot2Name + 0) == 'Z');
    assert(gb_read(&gb, wSaveSlot2Name + 4) == 'A');
    assert(gb_read(&gb, wFile2Health) == 40);
    assert(gb_read(&gb, wFile2MaxHearts) == 5);
    assert(gb_read(&gb, wFile2DeathCountHigh) == 0);
    assert(gb_read(&gb, wFile2DeathCountLow) == 0);

    /* Slot 3 checks */
    assert(gb_read(&gb, wSaveSlot3Name + 0) == 'M');
    assert(gb_read(&gb, wSaveSlot3Name + 4) == 'N');
    assert(gb_read(&gb, wFile3Health) == 80);
    assert(gb_read(&gb, wFile3MaxHearts) == 10);
    assert(gb_read(&gb, wFile3DeathCountHigh) == 0);
    assert(gb_read(&gb, wFile3DeathCountLow) == 7);

    /* Mode and display checks */
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SELECT);
    assert(gb_read(&gb, wGameplaySubtype) == 0);
    assert(gb_read(&gb, hBaseScrollY) == 0);
    assert(gb_read(&gb, hBaseScrollX) == 0);
    assert(gb_read(&gb, wBGPalette) == 0);
}


void test_file_creation_grid_and_entry(void) {
    printf("[*] Running FileCreation character grid & entry point tests (01:4852, 01:4A07-01:4CDA)...\n");
    GBState gb;

    /* Test 1: DrawSaveSlotName */
    gb_init(&gb);
    gb_write(&gb, wDrawCommandsSize, 0);
    gb_write(&gb, wSaveSlot1Name + 0, 'A');
    gb_write(&gb, wSaveSlot1Name + 1, 0);
    gb_write(&gb, wSaveSlot1Name + 2, 0);
    gb_write(&gb, wSaveSlot1Name + 3, 0);
    gb_write(&gb, wSaveSlot1Name + 4, 0);
    DrawSaveSlotName(&gb, 0x984A, wSaveSlot1Name);
    assert(gb_read(&gb, wDrawCommandsSize) == 0x10);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0x4A);
    assert(gb_read(&gb, wDrawCommand + 2) == 0x04);
    assert(gb_read(&gb, wDrawCommand + 8) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 9) == 0x2A);
    assert(gb_read(&gb, wDrawCommand + 10) == 0x04);
    assert(gb_read(&gb, wDrawCommand + 16) == 0x00);

    /* Test 2: func_001_4CDA */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, wSaveSlotNameCharIndex, 0);
    gb_write(&gb, wNameEntryCurrentChar, 0); /* 'A' (0x42) */
    func_001_4CDA(&gb);
    assert(gb_read(&gb, wSaveSlot1Name) == 0x42);

    /* Test 3: func_001_4C8A (A button and B button) */
    gb_write(&gb, hJoypadState, J_A);
    gb_write(&gb, hFrameCounter, 0x10); /* underline cursor visible */
    func_001_4C8A(&gb);
    assert(gb_read(&gb, wSaveSlotNameCharIndex) == 1);
    assert(gb_read(&gb, wOAMBuffer + 4) == 0x23);
    assert(gb_read(&gb, wOAMBuffer + 6) == 0xE0);

    gb_write(&gb, hJoypadState, J_B);
    func_001_4C8A(&gb);
    assert(gb_read(&gb, wSaveSlotNameCharIndex) == 0);

    /* Test 4: func_001_4BF5 (Grid navigation) */
    gb_init(&gb);
    gb_write(&gb, wNameEntryCurrentChar, 0);
    gb_write(&gb, hJoypadState, J_RIGHT);
    func_001_4BF5(&gb);
    assert(gb_read(&gb, wNameEntryCurrentChar) == 1);
    assert(gb_read(&gb, wOAMBuffer + 0) == (0x38 + 0x0B));
    assert(gb_read(&gb, wOAMBuffer + 1) == (0x1C + 0x04));
    assert(gb_read(&gb, wOAMBuffer + 2) == 0xE0);

    /* Left from 0 wraps to 63 */
    gb_write(&gb, wNameEntryCurrentChar, 0);
    gb_write(&gb, hJoypadState, J_LEFT);
    func_001_4BF5(&gb);
    assert(gb_read(&gb, wNameEntryCurrentChar) == 63);

    /* Up from 0 wraps to 48 */
    gb_write(&gb, wNameEntryCurrentChar, 0);
    gb_write(&gb, hJoypadState, J_UP);
    func_001_4BF5(&gb);
    assert(gb_read(&gb, wNameEntryCurrentChar) == 48);

    /* Test 5: FileCreationInteractiveHandler with "ZELDA" secret name */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    /* "ZELDA" in NameEntryCharmap: 0x5B, 0x46, 0x4D, 0x45, 0x42 */
    gb_write(&gb, wSaveSlotNames + 0, 0x5B);
    gb_write(&gb, wSaveSlotNames + 1, 0x46);
    gb_write(&gb, wSaveSlotNames + 2, 0x4D);
    gb_write(&gb, wSaveSlotNames + 3, 0x45);
    gb_write(&gb, wSaveSlotNames + 4, 0x42);
    gb_write(&gb, hJoypadState, J_START);
    FileCreationInteractiveHandler(&gb);

    /* Music track should trigger easter egg */
    assert(gb_read(&gb, wMusicTrackToPlay) == MUSIC_FILE_SELECT_ZELDA);
    /* SRAM SaveGame1.main name written */
    assert(gb_read(&gb, 0xA454 + 0) == 0x5B);
    assert(gb_read(&gb, 0xA454 + 4) == 0x42);
    /* SRAM SaveGame1.main health written (0x18) */
    assert(gb_read(&gb, 0xA45F) == 0x18);
    /* SRAM SaveGame1.main max hearts written (0x03) */
    assert(gb_read(&gb, 0xA460) == 0x03);
    /* SRAM SaveGame1.main death count written (0) */
    assert(gb_read(&gb, 0xA45C) == 0);
    assert(gb_read(&gb, 0xA45D) == 0);
    /* Returns to file select screen */
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SELECT);

    /* Test 6: FileCreationEntryPoint dispatcher */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 0);
    FileCreationEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    FileCreationEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 2);
}


void test_file_deletion_and_digits(void) {
    printf("[*] Running FileDeletion and BCD death counts tests (01:47FD-01:4839, 01:4D1A-01:4F8A)...\n");
    GBState gb;

    /* Test 1: CopyDigitsToFileScreenBG */
    gb_init(&gb);
    gb_write(&gb, wDrawCommandsSize, 0);
    /* high_b = 0x45 (tens=4, units=5), low_c = 0x03 (hundreds=3) */
    CopyDigitsToFileScreenBG(&gb, 0x98E7, 0x45, 0x03);
    assert(gb_read(&gb, wDrawCommandsSize) == 6);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xE7);
    assert(gb_read(&gb, wDrawCommand + 2) == 0x02);
    assert(gb_read(&gb, wDrawCommand + 3) == Data_001_4F3B[3]);
    assert(gb_read(&gb, wDrawCommand + 4) == Data_001_4F3B[4]);
    assert(gb_read(&gb, wDrawCommand + 5) == Data_001_4F3B[5]);
    assert(gb_read(&gb, wDrawCommand + 6) == 0x00);

    /* Test 2: CopyDeathCountsToBG */
    gb_init(&gb);
    gb_write(&gb, wSaveFilesCount, 0x05); /* files 1 and 3 active */
    gb_write(&gb, wFile1DeathCountHigh, 0x00);
    gb_write(&gb, wFile1DeathCountLow, 0x00);
    gb_write(&gb, wFile3DeathCountHigh, 0x12);
    gb_write(&gb, wFile3DeathCountLow, 0x00);
    CopyDeathCountsToBG(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    assert(gb_read(&gb, wDrawCommandsSize) == 12); /* 2 files * 6 bytes */

    /* Test 3: DrawSaveSlot names 1, 2, 3 */
    gb_init(&gb);
    DrawSaveSlot1Name(&gb);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xC5);
    DrawSaveSlot2Name(&gb);
    assert(gb_read(&gb, wDrawCommand + 16 + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 16 + 1) == 0x25);
    DrawSaveSlot3Name(&gb);
    assert(gb_read(&gb, wDrawCommand + 32 + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 32 + 1) == 0x85);

    /* Test 4: DrawSaveSlot max hearts */
    gb_init(&gb);
    gb_write(&gb, wSaveFilesCount, 0x07);
    gb_write(&gb, wFile1Health, 24);
    gb_write(&gb, wFile1MaxHearts, 3);
    DrawSaveSlot1MaxHearts(&gb);
    assert(gb_read(&gb, hMultiPurpose4) == 0);
    assert(gb_read(&gb, hMultiPurpose2) == 24);
    assert(gb_read(&gb, hMultiPurpose3) == 3);

    /* Test 5: FileDeletion state handlers */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    FileDeletionState0Handler(&gb);
    assert(gb_read(&gb, wPaletteDataFlags) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    FileDeletionState1Handler(&gb);
    assert(gb_read(&gb, wPaletteDataFlags) == 2);
    assert(gb_read(&gb, wGameplaySubtype) == 2);

    FileDeletionState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_FILL_TILEMAP);
    assert(gb_read(&gb, wSaveSlot) == 0);
    assert(gb_read(&gb, wCreditsScratch0) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    FileDeletionState3Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_MENU_FILE_ERASE);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    FileDeletionState4Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 5);

    FileDeletionState5Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 6);

    FileDeletionState6Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 7);

    FileDeletionState7Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 8);

    FileDeletionState8Handler(&gb);
    assert(gb_read(&gb, wPaletteDataFlags) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 9);

    FileDeletionState9Handler(&gb);
    assert(gb_read(&gb, wPaletteDataFlags) == 2);
    assert(gb_read(&gb, wGameplaySubtype) == 10);
}


void test_file_deletion_interactive_and_erase(void) {
    printf("[*] Running FileDeletion interactive & erase tests (01:4CFB-01:4F3A)...\n");
    GBState gb;

    /* Test 1: CopyQuitOkTilemap and CopyReturnToMenuTilemap */
    gb_init(&gb);
    CopyQuitOkTilemap(&gb);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xE4);
    assert(gb_read(&gb, wDrawCommand + 2) == 0x0D);

    gb_write(&gb, wDrawCommandsSize, 0);
    CopyReturnToMenuTilemap(&gb);
    assert(gb_read(&gb, wDrawCommandsSize) == 17);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 3) == 0x11);

    /* Test 2: BlankSaveSlotNameDrawCommand */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 1);
    BlankSaveSlotNameDrawCommand(&gb);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99);
    assert(gb_read(&gb, wDrawCommand + 1) == 0x05);
    assert(gb_read(&gb, wDrawCommand + 8) == 0x00);

    /* Test 3: func_001_4F0C (QUIT/OK cursor arrow) */
    gb_init(&gb);
    gb_write(&gb, wCreditsScratch0, 0);
    gb_write(&gb, hJoypadState, J_RIGHT);
    gb_write(&gb, hFrameCounter, 0x00); /* visible */
    func_001_4F0C(&gb);
    assert(gb_read(&gb, wCreditsScratch0) == 1);
    assert(gb_read(&gb, wOAMBuffer + 12) == 0x88);
    assert(gb_read(&gb, wOAMBuffer + 13) == 0x6C); /* OK position */
    assert(gb_read(&gb, wOAMBuffer + 14) == 0xBE);

    /* Test 4: FileDeletionState10Handler navigation */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, hJoypadState, J_DOWN);
    FileDeletionState10Handler(&gb);
    assert(gb_read(&gb, wSaveSlot) == 1);

    /* Slot 3 with A returns to file menu */
    gb_write(&gb, wSaveSlot, 3);
    gb_write(&gb, hJoypadState, J_A);
    FileDeletionState10Handler(&gb);
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SELECT);

    /* Slot 1 with A advances to confirmation */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 1);
    gb_write(&gb, wGameplaySubtype, 10);
    gb_write(&gb, hJoypadState, J_A);
    FileDeletionState10Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 11);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99); /* Quit/Ok tilemap copied */

    /* Test 5: FileDeletionState11Handler B cancels */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 11);
    gb_write(&gb, hJoypadState, J_B);
    FileDeletionState11Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 10);

    /* Test 6: FileDeletionState11Handler A with OK erases SRAM */
    gb_init(&gb);
    gb_write(&gb, wSaveSlot, 0);
    gb_write(&gb, wCreditsScratch0, 1); /* OK chosen */
    /* Fill SRAM slot 1 with non-zero dummy data */
    for (uint16_t i = 0; i < 0x03A8; i++) {
        gb_write(&gb, 0xA105 + i, 0xEE);
    }
    gb_write(&gb, hJoypadState, J_A);
    FileDeletionState11Handler(&gb);
    /* SRAM slot 1 must now be all 0x00 */
    assert(gb_read(&gb, 0xA105) == 0);
    assert(gb_read(&gb, 0xA105 + 0x03A7) == 0);
    /* Returned to file select screen */
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SELECT);

    /* Test 7: FileDeletionEntryPoint dispatcher */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FileDeletionEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}


void test_file_copy_subsystem(void) {
    printf("[*] Running FileCopy complete subsystem tests (01:4F8C-01:5292)...\n");
    GBState gb;

    /* Test 1: FileCopyState2Handler through State5Handler */
    gb_init(&gb);
    FileCopyState2Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_FILL_TILEMAP);
    assert(gb_read(&gb, wIntroTimer) == 0);
    assert(gb_read(&gb, wIntroSubTimer) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    FileCopyState3Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_MENU_FILE_COPY);
    assert(gb_read(&gb, wGameplaySubtype) == 2);

    FileCopyState4Handler(&gb);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xC4);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    gb_write(&gb, wDrawCommandsSize, 0);
    FileCopyState5Handler(&gb);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x98);
    assert(gb_read(&gb, wDrawCommand + 1) == 0xCD);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* Test 2: Arrow positioning */
    gb_init(&gb);
    gb_write(&gb, wIntroTimer, 1);
    func_001_5094(&gb);
    assert(gb_read(&gb, wOAMBuffer + 0) == (uint8_t)(Data_001_48E4[1] + 5));
    assert(gb_read(&gb, wOAMBuffer + 1) == 0x14);
    assert(gb_read(&gb, wOAMBuffer + 2) == 0xBE);

    gb_write(&gb, wIntroSubTimer, 2);
    func_001_51CE(&gb);
    assert(gb_read(&gb, wOAMBuffer + 8) == (uint8_t)(Data_001_48E4[2] + 5));
    assert(gb_read(&gb, wOAMBuffer + 9) == 0x5C);
    assert(gb_read(&gb, wOAMBuffer + 10) == 0xBE);

    /* Test 3: FileCopyState8Handler empty vs non-empty slot */
    gb_init(&gb);
    gb_write(&gb, wIntroTimer, 0);
    gb_write(&gb, wGameplaySubtype, 8);
    /* Slot 1 empty (all zeroes) */
    gb_write(&gb, hJoypadState, J_A);
    FileCopyState8Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 8); /* does not advance */

    /* Slot 1 with valid name */
    gb_write(&gb, wSaveSlot1Name, 'L');
    gb_write(&gb, hJoypadState, J_A);
    FileCopyState8Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 9); /* advances to destination select */

    /* Test 4: FileCopyState9Handler navigation and cancel */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 9);
    gb_write(&gb, wIntroSubTimer, 0);
    gb_write(&gb, hJoypadState, J_DOWN);
    FileCopyState9Handler(&gb);
    assert(gb_read(&gb, wIntroSubTimer) == 1);

    /* B button cancels back to 8 */
    gb_write(&gb, hJoypadState, J_B);
    FileCopyState9Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 8);

    /* A button advances to confirmation (10) */
    gb_write(&gb, wGameplaySubtype, 9);
    gb_write(&gb, wIntroSubTimer, 1);
    gb_write(&gb, hJoypadState, J_A);
    FileCopyState9Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 10);
    assert(gb_read(&gb, wDrawCommand + 0) == 0x99); /* Quit/Ok tilemap copied */

    /* Test 5: FileCopyStateAHandler B cancels back to 9 */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 10);
    gb_write(&gb, hJoypadState, J_B);
    FileCopyStateAHandler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 9);

    /* Test 6: FileCopyStateAHandler executes copy in SRAM */
    gb_init(&gb);
    gb_write(&gb, wIntroTimer, 0);    /* src: slot 0 (0xA100) */
    gb_write(&gb, wIntroSubTimer, 1); /* dst: slot 1 (0xA4AD) */
    gb_write(&gb, wCreditsScratch0, 1); /* OK selected */
    /* Write test pattern in slot 0 */
    for (uint16_t i = 0; i < 0x03AD; i++) {
        gb_write(&gb, 0xA100 + i, (uint8_t)(i & 0xFF));
    }
    gb_write(&gb, hJoypadState, J_A);
    FileCopyStateAHandler(&gb);
    /* Destination slot 1 must now contain identical pattern */
    for (uint16_t i = 0; i < 0x03AD; i++) {
        assert(gb_read(&gb, 0xA4AD + i) == (uint8_t)(i & 0xFF));
    }
    assert(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SELECT);

    /* Test 7: FileCopyEntryPoint dispatcher */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FileCopyEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}


void test_file_save_screen_and_init(void) {
    printf("[*] Running FileSaveScreen and InitSaveFiles tests (01:4000-01:414F, 01:46AA-01:47CD)...\n");
    GBState gb;

    /* Test 1: func_001_4794 prefix validation and recovery */
    gb_init(&gb);
    /* Corrupt slot 0 prefix */
    gb_write(&gb, 0xA100, 0x00);
    gb_write(&gb, 0xA105, 0x55);
    func_001_4794(&gb, 0);
    /* Prefix should now be 1, 3, 5, 7, 9 */
    assert(gb_read(&gb, 0xA100) == 1);
    assert(gb_read(&gb, 0xA101) == 3);
    assert(gb_read(&gb, 0xA102) == 5);
    assert(gb_read(&gb, 0xA103) == 7);
    assert(gb_read(&gb, 0xA104) == 9);
    /* Main area wiped */
    assert(gb_read(&gb, 0xA105) == 0);

    /* Test 2: InitSaveFiles with DebugTool */
    gb_init(&gb);
    uint8_t dummy_rom[0x100] = { 0 };
    dummy_rom[0x0003] = 1; /* Enable debug save creation */
    gb_attach_rom(&gb, dummy_rom, sizeof(dummy_rom));
    InitSaveFiles(&gb);
    assert(gb_read(&gb, 0xA453) == 0x01); /* sword level 1 */
    assert(gb_read(&gb, 0xA460) == 0x0A); /* 10 hearts */

    /* Test 3: FileSaveDelay1 and FileSaveDelay2 */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FileSaveDelay1(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_SAVE_MENU);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    FileSaveDelay2(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_MENU_FILE_SAVE);
    assert(gb_read(&gb, wWindowY) == 0xFF);
    assert(gb_read(&gb, wPaletteUnknownE) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 4);

    /* Test 4: func_001_412A navigation */
    gb_init(&gb);
    gb_write(&gb, wC13F, 0);
    gb_write(&gb, hJoypadState, J_DOWN);
    func_001_412A(&gb);
    assert(gb_read(&gb, wC13F) == 1);
    assert(gb_read(&gb, wOAMBuffer + 0x18) == 0x58);
    assert(gb_read(&gb, wOAMBuffer + 0x19) == 0x24);

    /* Test 5: FileSaveInteractive Return to Game */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 5);
    gb_write(&gb, wC13F, 0); /* Return to game */
    gb_write(&gb, hJoypadState, J_A);
    FileSaveInteractive(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 6);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);

    /* Test 6: FileSaveInteractive Save and Quit */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 5);
    gb_write(&gb, wC13F, 1); /* Save and Quit */
    gb_write(&gb, hJoypadState, J_A);
    FileSaveInteractive(&gb);
    assert(gb_read(&gb, rLCDC) == 0xC7);
    assert(gb_read(&gb, wLCDControl) == 0xC7);

    /* Test 7: LCDOn configuration */
    gb_init(&gb);
    LCDOn(&gb);
    assert(gb_read(&gb, rLCDC) == 0xC7);
    assert(gb_read(&gb, rWX) == 0x07);
    assert(gb_read(&gb, rWY) == 0x80);

    /* Test 8: FileSaveEntryPoint dispatcher */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FileSaveEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}


void test_game_over_subsystem(void) {
    printf("[*] Running GameOver subsystem tests (01:41C2-01:4370)...\n");
    GBState gb;

    /* Test 1: LinkPassOutHandler passing out animation (countdown != 0) */
    gb_init(&gb);
    gb_write(&gb, hLinkCountdown, 0x10); /* 16 */
    gb_write(&gb, wBGPalette, 0xE4);
    LinkPassOutHandler(&gb);
    assert(gb_read(&gb, wScreenShakeHorizontal) == 0);
    assert(gb_read(&gb, wScreenShakeVertical) == 0);
    assert(gb_read(&gb, wObjectAffectingBGPalette) == 1);
    assert(gb_read(&gb, wOBJ0Palette) == 0x1C);
    assert(gb_read(&gb, wOBJ1Palette) == 0xE4);
    assert(gb_read(&gb, hLinkAnimationState) == Data_001_41CF[0x10 >> 3]);
    assert(gb_read(&gb, wC3CD) == Data_001_41E7[0x10 >> 3]);

    /* Test 2: LinkPassOutHandler animation finish (countdown == 0) and BCD death counter */
    gb_init(&gb);
    gb_write(&gb, hLinkCountdown, 0);
    gb_write(&gb, wDeathCount, 0x09);
    gb_write(&gb, wDeathCount + 1, 0x00);
    LinkPassOutHandler(&gb);
    assert(gb_read(&gb, hLinkCountdown) == 16);
    assert(gb_read(&gb, hGameOverStage) == 1);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_0F);
    assert(gb_read(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_HIDDEN);
    assert(gb_read(&gb, wDeathCount) == 0x10); /* 9 + 1 = 10 in BCD */
    assert(gb_read(&gb, wDeathCount + 1) == 0x00);

    /* Test BCD max cap at 999 */
    gb_init(&gb);
    gb_write(&gb, hLinkCountdown, 0);
    gb_write(&gb, wDeathCount, 0x99);
    gb_write(&gb, wDeathCount + 1, 0x09);
    LinkPassOutHandler(&gb);
    assert(gb_read(&gb, wDeathCount) == 0x99);
    assert(gb_read(&gb, wDeathCount + 1) == 0x09);

    /* Test 3: LoadGameOverStage1Handler */
    gb_init(&gb);
    gb_write(&gb, hGameOverStage, 1);
    LoadGameOverStage1Handler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_SAVE_MENU);
    assert(gb_read(&gb, hGameOverStage) == 2);

    /* Test 4: LoadGameOverStage2Handler */
    gb_init(&gb);
    gb_write(&gb, hGameOverStage, 2);
    LoadGameOverStage2Handler(&gb);
    assert(gb_read(&gb, wBGPalette) == 0xE4);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_GAME_OVER);
    assert(gb_read(&gb, wWindowY) == 0xFF);
    assert(gb_read(&gb, hGameOverStage) == 3);

    /* Test 5: LoadGameOverStage3Handler */
    gb_init(&gb);
    gb_write(&gb, hGameOverStage, 3);
    gb_write(&gb, hLinkCountdown, 5);
    LoadGameOverStage3Handler(&gb);
    assert(gb_read(&gb, hGameOverStage) == 3); /* Still waiting */

    gb_write(&gb, hLinkCountdown, 0);
    LoadGameOverStage3Handler(&gb);
    assert(gb_read(&gb, hGameOverStage) == 4);
    assert(gb_read(&gb, wMusicTrackToPlay) == MUSIC_GAME_OVER);

    /* Test 6: func_001_4339 navigation */
    gb_init(&gb);
    gb_write(&gb, wC13F, 0);
    gb_write(&gb, hJoypadState, J_DOWN);
    func_001_4339(&gb);
    assert(gb_read(&gb, wC13F) == 1);
    assert(gb_read(&gb, wOAMBuffer + 0x18) == Data_001_4336[1]);
    assert(gb_read(&gb, wOAMBuffer + 0x19) == 0x24);
    assert(gb_read(&gb, wOAMBuffer + 0x1A) == 0xBE);

    gb_write(&gb, hJoypadState, J_UP);
    func_001_4339(&gb);
    assert(gb_read(&gb, wC13F) == 0);
    assert(gb_read(&gb, wOAMBuffer + 0x18) == Data_001_4336[0]);

    /* Test 7: GameOverInteractiveHandler option 1 (Save and Quit) */
    gb_init(&gb);
    gb_write(&gb, wC13F, 1);
    gb_write(&gb, hJoypadState, J_A);
    GameOverInteractiveHandler(&gb);
    assert(gb_read(&gb, hActiveEntityTilesOffset) == 0);

    /* Test 8: GameOverInteractiveHandler option 0 (Save and Continue) */
    gb_init(&gb);
    gb_write(&gb, wC13F, 0);
    gb_write(&gb, hJoypadState, J_A);
    gb_write(&gb, wEntitiesStatusTable, 0x55);
    GameOverInteractiveHandler(&gb);
    assert(gb_read(&gb, wEntitiesStatusTable) == 0);
    assert(gb_read(&gb, wInvincibilityCounter) == 0x80);

    /* Test 9: GameOverInteractiveHandler option 2 (Continue without saving) */
    gb_init(&gb);
    gb_write(&gb, wC13F, 2);
    gb_write(&gb, hJoypadState, J_START);
    gb_write(&gb, wEntitiesStatusTable, 0xAA);
    GameOverInteractiveHandler(&gb);
    assert(gb_read(&gb, wEntitiesStatusTable) == 0);
    assert(gb_read(&gb, wInvincibilityCounter) == 0x80);

    /* Test 10: LinkPassOut dispatcher */
    gb_init(&gb);
    gb_write(&gb, hGameOverStage, 1);
    LinkPassOut(&gb);
    assert(gb_read(&gb, hGameOverStage) == 2);
}


void test_world_handler_subsystem(void) {
    printf("[*] Running World Handler subsystem tests (01:4371-01:454F, 01:5511-01:5625)...\n");
    GBState gb;

    /* Test 1: GameplayWorldLoad0Handler overworld */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hFrameCounter, 0x02);
    GameplayWorldLoad0Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_INVENTORY);

    /* Test 2: GameplayWorldLoad0Handler debug tool 2 */
    uint8_t mock_rom[16] = {0};
    mock_rom[ROM_DebugTool2] = 1;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    GameplayWorldLoad0Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_INVENTORY_DEBUG);

    /* Test 3: GameplayWorldLoad0Handler dungeon (Tail Cave) */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, wDungeonItemFlags, 0x01); /* map flag */
    GameplayWorldLoad0Handler(&gb);
    assert(gb_read(&gb, wHasDungeonMap) == 0x01);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_MINIMAP);
    assert(gb_read(&gb, wMinimapLayout) == 0x00);

    /* Test 4: GameplayWorldLoad0Handler Windfish's Egg */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_WINDFISHS_EGG);
    GameplayWorldLoad0Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_EAGLES_TOWER_CLOUDS);

    /* Test 5: GameplayWorldLoadRoomHandler */
    gb_init(&gb);
    gb_write(&gb, wMapEntrancePositionX, 0x30);
    gb_write(&gb, wMapEntrancePositionY, 0x40);
    gb_write(&gb, wMapEntrancePositionZ, 0x05);
    gb_write(&gb, wIsIndoor, 0);
    GameplayWorldLoadRoomHandler(&gb);
    assert(gb_read(&gb, hLinkPositionX) == 0x30);
    assert(gb_read(&gb, hLinkPositionY) == 0x40);
    assert(gb_read(&gb, hLinkPositionZ) == 0x05);
    assert(gb_read(&gb, wIsLinkInTheAir) == 2);
    assert(gb_read(&gb, wTilesetToLoad) == 0x07); /* Outdoor */
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    /* Test 6: GameplayWorldSelectTilesetHandler */
    gb_init(&gb);
    GameplayWorldSelectTilesetHandler(&gb);
    assert(gb_read(&gb, hWorldTileset) == 0x0F);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_ROOM_SPECIFIC);
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    /* Test 7: GameplayWorldLoadRoomTilemapHandler */
    gb_init(&gb);
    gb_write(&gb, wRoomSwitchableObject, 1);
    gb_write(&gb, wSwitchButtonPressed, 1);
    GameplayWorldLoadRoomTilemapHandler(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_ROOM_TILEMAP);
    assert(gb_read(&gb, wSwitchableObjectAnimationStage) == 5);
    assert(gb_read(&gb, hReplaceTiles) == REPLACE_TILES_BUTTON_PRESSED);
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    /* Test 8: GameplayWorldLoadHeartsAndRupeesHandler & ABButtonsHandler */
    gb_init(&gb);
    GameplayWorldLoadHeartsAndRupeesHandler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    GameplayWorldLoadABButtonsHandler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 2);

    /* Test 9: GameplayWorldLoad6Handler */
    gb_init(&gb);
    gb_write(&gb, rLCDC, 0);
    gb_write(&gb, wLinkMotionState, 0x02);
    GameplayWorldLoad6Handler(&gb);
    assert((gb_read(&gb, rLCDC) & LCDCF_WINON) != 0);
    assert(gb_read(&gb, wD463) == 0x02);
    assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_IN);
    assert(gb_read(&gb, wGameplaySubtype) == 1);

    /* Test 10: CreateMinimapTilemap and func_001_5619 */
    gb_init(&gb);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, wMinimapLayout, 0);
    CreateMinimapTilemap(&gb);
    /* Verify prefix loaded at wMinimapTilemap */
    assert(gb_read(&gb, wMinimapTilemap) == Data_001_54E8[0]);
    assert(gb_read(&gb, wMinimapTilemap + 1) == Data_001_54E8[1]);

    /* Test 11: WorldHandlerEntryPoint dispatcher */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    WorldHandlerEntryPoint(&gb);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_ROOM_SPECIFIC);
}


void test_face_shrine_mural_subsystem(void) {
    printf("[*] Running Face Shrine Mural subsystem tests (01:6AF8-01:6BA7)...\n");
    GBState gb;

    /* Test 1: FaceShrineMuralStage0Handler CGB */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wBGPal1, 0x42);
    FaceShrineMuralStage0Handler(&gb);
    /* Subtype was 0, stage 0 increments to 1, then falls into stage 1 */
    assert(gb_read(&gb, wGameplaySubtype) == 1);
    gb_write(&gb, rSVBK, 3);
    assert(gb_read(&gb, wBGPal1) == 0x42);
    gb_write(&gb, rSVBK, 0);

    /* Test 2: FaceShrineMuralStage1Handler transition completion */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 1);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    for (int i = 0; i < 12; i++) {
        gb_write(&gb, wRoomTransitionState + i, 0xFF);
    }
    FaceShrineMuralStage1Handler(&gb);
    assert(gb_read(&gb, wC167) == 1);
    assert(gb_read(&gb, wRoomTransitionState) == 0);
    assert(gb_read(&gb, wRoomTransitionState + 11) == 0);
    assert(gb_read(&gb, hVolumeRight) == 0x03);
    assert(gb_read(&gb, hVolumeLeft) == 0x30);
    assert(gb_read(&gb, wGameplaySubtype) == 2);
    assert(gb_read(&gb, wScrollXOffset) == 0);
    assert(gb_read(&gb, wTilesetToLoad) == TILESET_FACE_SHRINE_MURAL);

    /* Test 3: FaceShrineMuralStage2Handler */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FaceShrineMuralStage2Handler(&gb);
    assert(gb_read(&gb, wBGMapToLoad) == TILEMAP_FACE_SHRINE_MURAL);
    assert(gb_read(&gb, wWindowY) == 0xFF);
    assert(gb_read(&gb, hBaseScrollX) == 0);
    assert(gb_read(&gb, hBaseScrollY) == 0);
    assert(gb_read(&gb, wTransitionSequenceCounter) == 0);
    assert(gb_read(&gb, wC16C) == 0);
    assert(gb_read(&gb, wPaletteUnknownE) == 1);
    assert(gb_read(&gb, wGameplaySubtype) == 3);

    /* Test 4: FaceShrineMuralStage3Handler */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 3);
    gb_write(&gb, wTransitionSequenceCounter, 4);
    FaceShrineMuralStage3Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 4);
    assert(gb_read(&gb, wC3C4) == 0);

    /* Test 5: FaceShrineMuralStage4Handler */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 4);
    gb_write(&gb, wDialogState, 1);
    gb_write(&gb, wC3C4, 0x10);
    FaceShrineMuralStage4Handler(&gb);
    assert(gb_read(&gb, wC3C4) == 0x10); /* unchanged if dialog open */

    gb_write(&gb, wDialogState, 0);
    gb_write(&gb, wC3C4, 0x7F);
    FaceShrineMuralStage4Handler(&gb);
    assert(gb_read(&gb, wC3C4) == 0x80);
    assert(gb_read(&gb, wDialogState) != 0); /* opened dialog 0xE7 */

    /* Wrapped to 0 */
    gb_write(&gb, wDialogState, 0);
    gb_write(&gb, wC3C4, 0xFF);
    FaceShrineMuralStage4Handler(&gb);
    assert(gb_read(&gb, wC3C4) == 0);
    assert(gb_read(&gb, wGameplaySubtype) == 5);

    /* Test 6: FaceShrineMuralStage5Handler */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 5);
    gb_write(&gb, hJoypadState, 0);
    FaceShrineMuralStage5Handler(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 5);

    gb_write(&gb, hJoypadState, J_A);
    FaceShrineMuralStage5Handler(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_VALIDATE);
    assert(gb_read(&gb, wGameplaySubtype) == 6);
    assert(gb_read(&gb, wTransitionSequenceCounter) == 0);
    assert(gb_read(&gb, wC16C) == 0);

    /* Test 7: FaceShrineMuralEntryPoint */
    gb_init(&gb);
    gb_write(&gb, wGameplaySubtype, 2);
    FaceShrineMuralEntryPoint(&gb);
    assert(gb_read(&gb, wGameplaySubtype) == 3);
}

void test_siren_instruments_subsystem(void) {
    printf("[*] Running Siren Instruments subsystem tests (01:6BB5-01:6C76)...\n");
    GBState gb;

    /* Test 1: GetInstrumentNextBGAddress */
    uint16_t base = 0x9D00;
    uint16_t next1 = GetInstrumentNextBGAddress(base, 0);
    assert(next1 == 0x9D01);
    uint16_t next2 = GetInstrumentNextBGAddress(next1, 1);
    assert(next2 == 0x9D20);
    uint16_t next3 = GetInstrumentNextBGAddress(next2, 2);
    assert(next3 == 0x9D21);

    /* Test 2: LoadInstrumentsBG right side (instruments 0..3) */
    gb_init(&gb);
    gb_write(&gb, wHasInstrument1 + 0, 0x02); /* Obtained: tile 0xD0 */
    gb_write(&gb, wHasInstrument1 + 1, 0x00); /* Missing: placeholder 0xB2 */
    LoadInstrumentsBG(&gb, 1);

    /* Instrument 0 at 0x9D00 + 0x0F = 0x9D0F */
    assert(gb_read(&gb, 0x9D0F) == 0xD0);
    assert(gb_read(&gb, 0x9D10) == 0xD1);
    assert(gb_read(&gb, 0x9D2F) == 0xE0);
    assert(gb_read(&gb, 0x9D30) == 0xE1);

    /* Instrument 1 at 0x9D00 + 0x51 = 0x9D51 */
    assert(gb_read(&gb, 0x9D51) == 0x7C);
    assert(gb_read(&gb, 0x9D52) == 0x7C);
    assert(gb_read(&gb, 0x9D71) == 0x7C);
    assert(gb_read(&gb, 0x9D72) == 0xB2);

    /* Test 3: LoadSirenInstrumentTiles */
    gb_init(&gb);
    gb_write(&gb, hBGTilesLoadingStage, 2);
    LoadSirenInstrumentTiles(&gb, 2);
    assert(gb_read(&gb, hBGTilesLoadingStage) == 3);

    /* Test 4: LoadSirenInstruments state machine */
    gb_init(&gb);
    gb_write(&gb, hBGTilesLoadingStage, 0);
    LoadSirenInstruments(&gb);
    assert(gb_read(&gb, hBGTilesLoadingStage) == 1);

    gb_write(&gb, hBGTilesLoadingStage, 8);
    LoadSirenInstruments(&gb);
    assert(gb_read(&gb, hBGTilesLoadingStage) == 9);

    gb_write(&gb, hBGTilesLoadingStage, 9);
    gb_write(&gb, hNeedsUpdatingBGTiles, 1);
    LoadSirenInstruments(&gb);
    assert(gb_read(&gb, hBGTilesLoadingStage) == 0);
    assert(gb_read(&gb, hNeedsUpdatingBGTiles) == 0);
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
    test_load_saved_file();
    test_func_001_4954();
    test_file_selection_interactive_and_choice();
    test_file_creation_init_and_sram();
    test_transition_to_file_menu_reload();
    test_file_creation_grid_and_entry();
    test_file_deletion_and_digits();
    test_file_deletion_interactive_and_erase();
    test_file_copy_subsystem();
    test_file_save_screen_and_init();
    test_game_over_subsystem();
    test_world_handler_subsystem();
    test_face_shrine_mural_subsystem();
    test_siren_instruments_subsystem();
    printf("  [PASS] All bank1 room transition & sprite functions verified successfully!\n\n");
}
