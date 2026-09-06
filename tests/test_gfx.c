#include <stdio.h>
#include <string.h>
#include "gb.h"
#include "home/gfx.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"

static int gfx_failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAIL: %s at %s:%d\n", msg, __FILE__, __LINE__); \
            gfx_failures++; \
        } \
    } while (0)

#define ROM_BANK_OFFSET(bank, addr) (((bank) * 0x4000) + ((addr) - 0x4000))

static uint8_t mock_rom[0x4000 * 0x36];

static void setup_mock_data(void) {
    memset(mock_rom, 0, sizeof(mock_rom));

    /* Populate test markers in banks */
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x2800)] = 0x11;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x3000)] = 0x22;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles)] = 0x33;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x800)] = 0x55;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x1800)] = 0x66;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x2000)] = 0x77;

    mock_rom[ROM_BANK_OFFSET(0x0C, Overworld1Tiles + 0x8E0)] = 0x88;
    mock_rom[ROM_BANK_OFFSET(0x0C, Overworld1Tiles + 0x100)] = 0x99;

    mock_rom[ROM_BANK_OFFSET(0x12, Npc4Tiles + 0x100)] = 0xAA;
    mock_rom[ROM_BANK_OFFSET(0x12, EndingTiles + 0x3600)] = 0x44; /* Bank 12 active from LoadTileset15 */
    mock_rom[ROM_BANK_OFFSET(0x12, Npc3Tiles + 0x2000)] = 0xBB;

    mock_rom[ROM_BANK_OFFSET(0x0F, Overworld2Tiles + 0x600)] = 0xCC;

    mock_rom[ROM_BANK_OFFSET(0x35, PhotoAlbumTiles)] = 0xDD;
}

static void test_load_credits_koholint_disappearing_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadCreditsKoholintDisappearingTiles(&gb);

    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x11, "vTiles2 marker not copied");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x22, "vTiles1 marker not copied");
}

static void test_load_tileset_15(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadTileset15(&gb);

    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xAA, "vTiles0 Npc4 marker not copied");
    TEST_ASSERT(gb_read(&gb, vTiles2 + 0x7F0) == 0x88, "vTiles2+0x7F0 Overworld1 marker not copied");
    TEST_ASSERT(gb_read(&gb, vTiles1 + 0x500) == 0xAA, "vTiles1+0x500 Npc4 marker not copied");
}

static void test_load_credits_stairs_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadCreditsStairsTiles(&gb);

    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x400) == 0x44, "vTiles0+0x400 EndingTiles marker not copied");
}

static void test_load_credits_koholint_views_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadCreditsKoholintViewsTiles(&gb);

    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x99, "vTiles2 Overworld1 marker not copied");
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xBB, "vTiles0 Npc3 marker not copied");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0xCC, "vTiles1 Overworld2 marker not copied");
}

static void test_load_credits_link_on_sea_dmg_and_cgb(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));

    /* 1. DMG Mode: Link on sea close */
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsLinkOnSeaCloseTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x33, "DMG Close sea tiles not from EndingTiles");
    TEST_ASSERT(gb_read(&gb, vTiles0 + TILE_SIZE * 0x80) == 0x66, "DMG Link on sea part 2 mismatch");

    /* 2. CGB Mode: Link on sea close uses PhotoAlbumTiles in bank 0x35 */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    /* Also set ending tiles in GBC bank $33 */
    mock_rom[ROM_BANK_OFFSET(0x33, EndingTiles + 0x1800)] = 0x67;
    gb_write(&gb, hIsGBC, 1);
    LoadCreditsLinkOnSeaCloseTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xDD, "CGB Close sea tiles not from PhotoAlbumTiles");
    TEST_ASSERT(gb_read(&gb, vTiles0 + TILE_SIZE * 0x80) == 0x67, "CGB Link on sea part 2 mismatch");

    /* 3. Sun above tiles */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsSunAboveTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x55, "Sun above tiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles0 + TILE_SIZE * 0x80) == 0x66, "Sun above part 2 mismatch");

    /* 4. Link on sea large tiles */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsLinkOnSeaLargeTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x77, "Link on sea large tiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles0 + TILE_SIZE * 0x80) == 0x66, "Link on sea large part 2 mismatch");
}

int run_gfx_tests(void) {
    printf("[*] Running GFX and Credits tile loading tests...\n");
    setup_mock_data();
    test_load_credits_koholint_disappearing_tiles();
    test_load_tileset_15();
    test_load_credits_stairs_tiles();
    test_load_credits_koholint_views_tiles();
    test_load_credits_link_on_sea_dmg_and_cgb();

    if (gfx_failures == 0) {
        printf("  [PASS] All gfx.asm / credits tile loaders verified successfully!\n\n");
    }
    return gfx_failures;
}
