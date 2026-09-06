#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/room.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/maps.h"
#include "constants/rooms.h"
#include "constants/tilesets.h"
#include "constants/sfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_mark_trigger_as_resolved(void) {
    GBState gb;

    /* Case 1: wRoomEventEffectExecuted is already non-zero -> should do nothing */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 1);
    gb_write(&gb, wC1CF, 0x55);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wC1CF) == 0x55, "wC1CF modified when event already executed");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 0x00, "wC5A6 modified when event already executed");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_NONE, "hJingle modified when event already executed");

    /* Case 2: wRoomEventEffectExecuted is 0 and wC19D == 0 -> should resolve and play jingle */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0x77);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC1CF) == 0, "wC1CF not cleared to 0");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_PUZZLE_SOLVED, "hJingle not set to JINGLE_PUZZLE_SOLVED");

    /* Case 3: wRoomEventEffectExecuted is 0 but wC19D != 0 -> should resolve without playing jingle */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0x88);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x01);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC1CF) == 0, "wC1CF not cleared to 0");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_NONE, "hJingle triggered despite wC19D != 0");
}

static uint8_t mock_tileset_rom[0x84000];

#define ROM_BANK_20_OFFSET(addr) ((size_t)0x20 * 0x4000 + ((addr) - 0x4000))

static void test_select_room_tilesets_overworld(void) {
    GBState gb;
    gb_init(&gb);
    memset(mock_tileset_rom, 0, sizeof(mock_tileset_rom));
    gb_attach_rom(&gb, mock_tileset_rom, sizeof(mock_tileset_rom));

    /* Setup mock OverworldTilesetsTable */
    /* Room 0x00 -> de_low = 0 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldTilesetsTable_Addr + 0)] = 0x05; /* New tileset */

    /* Setup RoomSpritesheetGroupsTable for overworld room 0x00: group 0x02 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(RoomSpritesheetGroupsTable_Addr + 0x00)] = 0x02;

    /* Setup OverworldEntitySpritesheetsTable for group 2: group * 4 = 8 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 0)] = 0x10; /* slot 0 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 1)] = 0x20; /* slot 1 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 2)] = 0xFF; /* slot 2: keep */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 3)] = 0x40; /* slot 3: already 0x40 */

    /* Pre-load state */
    gb_write(&gb, wCurrentBank, 0x01);
    gb_write(&gb, wIsIndoor, 0); /* Overworld */
    gb_write(&gb, hMapRoom, 0x00);
    gb_write(&gb, hMapId, 0x00);
    gb_write(&gb, hWorldTileset, 0x01); /* Different from 0x05 */
    gb_write(&gb, hNeedsUpdatingBGTiles, 0);

    /* Pre-load entity spritesheets: slot 0 is 0x00, slot 1 is 0x00, slot 2 is 0x30, slot 3 is 0x40 */
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    gb_write(&gb, wLoadedEntitySpritesheets + 1, 0x00);
    gb_write(&gb, wLoadedEntitySpritesheets + 2, 0x30);
    gb_write(&gb, wLoadedEntitySpritesheets + 3, 0x40);

    SelectRoomTilesets(&gb);

    /* Verify BG tileset updated and scheduled */
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x05, "Overworld hWorldTileset not updated");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == TILESET_LOAD_WORLD, "hNeedsUpdatingBGTiles not set");

    /* Verify spritesheets scheduled:
     * Slot 0 was 0x00 -> updated to 0x10 (variant A: slot 0, hNeedsUpdatingEntityTilesA = 1)
     * Slot 1 was 0x00 -> updated to 0x20 (variant B: slot 1, wNeedsUpdatingEntityTilesB = 1)
     * Slot 2 was 0xFF -> skipped
     * Slot 3 was 0x40 -> matched loaded 0x40 -> skipped
     */
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x10, "Spritesheet slot 0 not updated");
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 1) == 0x20, "Spritesheet slot 1 not updated");
    TEST_ASSERT(gb_read(&gb, wEntityTilesSpriteslotIndexA) == 0, "Variant A index not 0");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingEntityTilesA) == 1, "hNeedsUpdatingEntityTilesA not set");
    TEST_ASSERT(gb_read(&gb, wEntityTilesSpriteslotIndexB) == 1, "Variant B index not 1");
    TEST_ASSERT(gb_read(&gb, wNeedsUpdatingEntityTilesB) == 1, "wNeedsUpdatingEntityTilesB not set");

    /* Verify bank restored to saved bank */
    TEST_ASSERT(gb.rom_bank == 0x01, "ROM bank not restored to 0x01");
}

static void test_select_room_tilesets_special_cases(void) {
    GBState gb;
    gb_init(&gb);
    memset(mock_tileset_rom, 0, sizeof(mock_tileset_rom));
    gb_attach_rom(&gb, mock_tileset_rom, sizeof(mock_tileset_rom));

    /* 1. Egg room hack: room 0x07 increments to 0x08 -> de_low = 4 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldTilesetsTable_Addr + 4)] = 0x09;
    gb_write(&gb, wCurrentBank, 0x01);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hMapRoom, ROOM_OW_RIGHT_OF_EGG); /* 0x07 */
    gb_write(&gb, hMapId, 0x00);
    gb_write(&gb, hWorldTileset, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x09, "Egg room hack did not load index 4");

    /* 2. Overworld Siren room owl talked: group 0x23 -> 0x24 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(RoomSpritesheetGroupsTable_Addr + ROOM_OW_SIREN)] = 0x23;
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 0x24 * 4 + 0)] = 0x55;
    gb_write(&gb, wOverworldRoomStatus + ROOM_OW_SIREN, OW_ROOM_STATUS_OWL_TALKED);
    gb_write(&gb, hMapRoom, ROOM_OW_SIREN);
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x55, "Siren room owl talked group 0x24 not used");

    /* 3. Color dungeon (Map 0xFF) indoors */
    mock_tileset_rom[ROM_BANK_20_OFFSET(ColorDungeonTilesetsTable_Addr + 0x05)] = 0x12;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    gb_write(&gb, hMapRoom, 0x05);
    gb_write(&gb, hWorldTileset, 0x00);
    gb_write(&gb, hNeedsUpdatingEntityTilesA, 0);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x12, "Color dungeon tileset not read from ColorDungeonTilesetsTable");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingEntityTilesA) == 1, "Color dungeon entity tiles A not set");
    TEST_ASSERT(gb.rom_bank == 0x01, "ROM bank not restored after Color Dungeon");

    /* 4. Indoor camera shop: Map 0x10, room 0xB5 -> group 0x3D */
    mock_tileset_rom[ROM_BANK_20_OFFSET(IndoorEntitySpritesheetsTable_Addr + 0x3D * 4 + 0)] = 0x77;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_HOUSE);
    gb_write(&gb, hMapRoom, ROOM_INDOOR_B_CAMERA_SHOP);
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x77, "Indoor camera shop did not use group 0x3D");

    /* 5. W_TILESET_NO_UPDATE indoors: updates hWorldTileset but does NOT set hNeedsUpdatingBGTiles */
    mock_tileset_rom[ROM_BANK_20_OFFSET(IndoorsTilesetsTable_Addr + 0x15)] = W_TILESET_NO_UPDATE;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, 0x15);
    gb_write(&gb, hWorldTileset, 0x00);
    gb_write(&gb, hNeedsUpdatingBGTiles, 0);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == W_TILESET_NO_UPDATE, "W_TILESET_NO_UPDATE not written to hWorldTileset");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "hNeedsUpdatingBGTiles set on NO_UPDATE");
}

void run_room_tests(void) {
    printf("[*] Running MarkTriggerAsResolved tests...\n");
    test_mark_trigger_as_resolved();

    printf("[*] Running SelectRoomTilesets tests...\n");
    test_select_room_tilesets_overworld();
    test_select_room_tilesets_special_cases();

    if (failures == 0) {
        printf("  [PASS] All room.asm functions verified successfully!\n\n");
    }
}
