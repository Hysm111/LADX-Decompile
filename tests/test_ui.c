#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/ui.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_copy_tiles_to_piece_of_heart_meter(void) {
    GBState gb;
    gb_init(&gb);

    /* Allocate simulated ROM in bank $0C */
    uint8_t test_rom[0x34000];
    memset(test_rom, 0, sizeof(test_rom));

    /* Populate bank $0C source at $6900 */
    uint32_t rom_offset = (0x0C * 0x4000) + (0x6900 - 0x4000);
    for (int i = 0; i < 0x30; i++) {
        test_rom[rom_offset + i] = (uint8_t)(0xA0 + i);
    }

    gb_attach_rom(&gb, test_rom, sizeof(test_rom));
    gb.rom_bank = 0x0C;

    gb_write(&gb, hNeedsUpdatingBGTiles, 5);
    gb_write(&gb, hBGTilesLoadingStage, 2);

    CopyTilesToPieceOfHeartMeter(&gb, 0x89A0, 0x6900);

    /* Verify 0x30 bytes copied to VRAM at 0x89A0 */
    for (int i = 0; i < 0x30; i++) {
        TEST_ASSERT(gb.vram[0][0x9A0 + i] == (uint8_t)(0xA0 + i), "Copied byte mismatch");
    }

    /* Verify flags and bank */
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "hNeedsUpdatingBGTiles not cleared");
    TEST_ASSERT(gb_read(&gb, hBGTilesLoadingStage) == 0, "hBGTilesLoadingStage not cleared");
    TEST_ASSERT(gb.rom_bank == 0x0C, "ROM bank not restored to 0x0C");
}

static void test_load_piece_of_heart_meter_tiles(void) {
    GBState gb;
    gb_init(&gb);

    uint8_t test_rom[0x34000];
    memset(test_rom, 0, sizeof(test_rom));

    /* Setup PieceOfHeartMeterTiles at 0x6900 in bank $0C */
    uint32_t offset1 = (0x0C * 0x4000) + (PieceOfHeartMeterTiles - 0x4000);
    for (int i = 0; i < 0x30; i++) {
        test_rom[offset1 + i] = (uint8_t)(0x10 + i);
    }
    /* Setup PieceOfHeartMeterTiles + 0x30 at 0x6930 */
    uint32_t offset2 = offset1 + 0x30;
    for (int i = 0; i < 0x30; i++) {
        test_rom[offset2 + i] = (uint8_t)(0x40 + i);
    }

    gb_attach_rom(&gb, test_rom, sizeof(test_rom));
    gb.rom_bank = 0x0C;

    /* Test LoadPieceOfHeartMeterTiles1: dest = vTiles1 + $1A0 (0x89A0) */
    gb_write(&gb, hNeedsUpdatingBGTiles, 1);
    gb_write(&gb, hBGTilesLoadingStage, 1);
    LoadPieceOfHeartMeterTiles1(&gb);

    for (int i = 0; i < 0x30; i++) {
        TEST_ASSERT(gb.vram[0][0x9A0 + i] == (uint8_t)(0x10 + i), "LoadPieceOfHeartMeterTiles1 byte mismatch");
    }
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "Flags not cleared after Load 1");
    TEST_ASSERT(gb_read(&gb, hBGTilesLoadingStage) == 0, "Stage not cleared after Load 1");
    TEST_ASSERT(gb.rom_bank == 0x0C, "Bank not 0x0C after Load 1");

    /* Test LoadPieceOfHeartMeterTiles2: dest = vTiles1 + $1D0 (0x89D0) */
    gb_write(&gb, hNeedsUpdatingBGTiles, 1);
    gb_write(&gb, hBGTilesLoadingStage, 1);
    LoadPieceOfHeartMeterTiles2(&gb);

    for (int i = 0; i < 0x30; i++) {
        TEST_ASSERT(gb.vram[0][0x9D0 + i] == (uint8_t)(0x40 + i), "LoadPieceOfHeartMeterTiles2 byte mismatch");
    }
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "Flags not cleared after Load 2");
    TEST_ASSERT(gb_read(&gb, hBGTilesLoadingStage) == 0, "Stage not cleared after Load 2");
    TEST_ASSERT(gb.rom_bank == 0x0C, "Bank not 0x0C after Load 2");
}

static void test_clear_piece_of_heart_meter_tiles(void) {
    GBState gb;
    gb_init(&gb);

    uint8_t test_rom[0x34000];
    memset(test_rom, 0, sizeof(test_rom));

    /* Setup InventoryEquipmentItemsTiles + $1D0 at 0x49D0 in bank $0C */
    uint32_t offset1 = (0x0C * 0x4000) + (InventoryEquipmentItemsTiles + 0x1D0 - 0x4000);
    for (int i = 0; i < 0x30; i++) {
        test_rom[offset1 + i] = (uint8_t)(0x70 + i);
    }
    /* Setup InventoryEquipmentItemsTiles + $1A0 at 0x49A0 */
    uint32_t offset2 = (0x0C * 0x4000) + (InventoryEquipmentItemsTiles + 0x1A0 - 0x4000);
    for (int i = 0; i < 0x30; i++) {
        test_rom[offset2 + i] = (uint8_t)(0x90 + i);
    }

    gb_attach_rom(&gb, test_rom, sizeof(test_rom));
    gb.rom_bank = 0x0C;

    /* Test ClearPieceOfHeartMeterTiles1: restores $49D0 to vTiles1 + $1D0 (0x89D0) */
    gb_write(&gb, hNeedsUpdatingBGTiles, 1);
    gb_write(&gb, hBGTilesLoadingStage, 1);
    ClearPieceOfHeartMeterTiles1(&gb);

    for (int i = 0; i < 0x30; i++) {
        TEST_ASSERT(gb.vram[0][0x9D0 + i] == (uint8_t)(0x70 + i), "ClearPieceOfHeartMeterTiles1 byte mismatch");
    }
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "Flags not cleared after Clear 1");
    TEST_ASSERT(gb_read(&gb, hBGTilesLoadingStage) == 0, "Stage not cleared after Clear 1");
    TEST_ASSERT(gb.rom_bank == 0x0C, "Bank not 0x0C after Clear 1");

    /* Test ClearPieceOfHeartMeterTiles2: restores $49A0 to vTiles1 + $1A0 (0x89A0) */
    gb_write(&gb, hNeedsUpdatingBGTiles, 1);
    gb_write(&gb, hBGTilesLoadingStage, 1);
    ClearPieceOfHeartMeterTiles2(&gb);

    for (int i = 0; i < 0x30; i++) {
        TEST_ASSERT(gb.vram[0][0x9A0 + i] == (uint8_t)(0x90 + i), "ClearPieceOfHeartMeterTiles2 byte mismatch");
    }
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "Flags not cleared after Clear 2");
    TEST_ASSERT(gb_read(&gb, hBGTilesLoadingStage) == 0, "Stage not cleared after Clear 2");
    TEST_ASSERT(gb.rom_bank == 0x0C, "Bank not 0x0C after Clear 2");
}

void run_ui_tests(void) {
    printf("[*] Running Piece-of-Heart meter UI tests...\n");
    test_copy_tiles_to_piece_of_heart_meter();
    test_load_piece_of_heart_meter_tiles();
    test_clear_piece_of_heart_meter_tiles();

    if (failures == 0) {
        printf("  [PASS] All UI Piece-of-Heart meter tests passed.\n");
    } else {
        printf("  [FAIL] %d UI test(s) failed.\n", failures);
    }
}
