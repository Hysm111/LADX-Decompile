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

static uint8_t mock_rom[0x4000 * 0x39];

static void setup_mock_data(void) {
    memset(mock_rom, 0, sizeof(mock_rom));

    /* Populate test markers in banks */
    mock_rom[ROM_BANK_OFFSET(0x01, IntroRainTiles)] = 0x51;

    /* Bank $13: EndingTiles ($4000), EaglesTowerTop1Tiles ($6800), EaglesTowerTop2Tiles ($7C00) */
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x2800)] = 0x11; /* Same as EaglesTowerTop1Tiles */
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x3000)] = 0x22;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles)] = 0x33;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x800)] = 0x55;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x1800)] = 0x66;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x2000)] = 0x77;
    mock_rom[ROM_BANK_OFFSET(0x13, EndingTiles + 0x3800)] = 0x18;
    mock_rom[ROM_BANK_OFFSET(0x13, EaglesTowerTop2Tiles)] = 0xA2;

    mock_rom[ROM_BANK_OFFSET(0x0C, Overworld1Tiles + 0x8E0)] = 0x88;
    mock_rom[ROM_BANK_OFFSET(0x0C, Overworld1Tiles + 0x100)] = 0x99;
    mock_rom[ROM_BANK_OFFSET(0x0C, LinkCharacterTiles)] = 0x5A;
    mock_rom[ROM_BANK_OFFSET(0x0C, InventoryEquipmentItemsTiles)] = 0x6B;
    mock_rom[ROM_BANK_OFFSET(0x0C, Items1Tiles + 0x3A0)] = 0x7C;
    mock_rom[ROM_BANK_OFFSET(0x0C, WorldMapTiles)] = 0x91;

    mock_rom[ROM_BANK_OFFSET(0x12, Npc4Tiles + 0x100)] = 0xAA;
    mock_rom[ROM_BANK_OFFSET(0x12, EndingTiles + 0x3600)] = 0x44; /* Bank 12 active from LoadTileset15 */
    mock_rom[ROM_BANK_OFFSET(0x12, Npc3Tiles + 0x2000)] = 0xBB;
    mock_rom[ROM_BANK_OFFSET(0x12, Npc3Tiles + 0x2600)] = 0xBC;

    mock_rom[ROM_BANK_OFFSET(0x0F, Overworld2Tiles + 0x600)] = 0xCC;
    mock_rom[ROM_BANK_OFFSET(0x0F, MenuTiles)] = 0x8D;
    mock_rom[ROM_BANK_OFFSET(0x0F, FontTiles)] = 0x9E;
    mock_rom[ROM_BANK_OFFSET(0x0F, TitleLogoTiles)] = 0x71;
    mock_rom[ROM_BANK_OFFSET(0x0F, SaveMenuTiles)] = 0x72;

    /* Bank $2F: GBC adjusted bank for $0F */
    mock_rom[ROM_BANK_OFFSET(0x2F, TitleLogoTiles)] = 0x71;

    mock_rom[ROM_BANK_OFFSET(0x10, Intro1Tiles)] = 0x52;
    mock_rom[ROM_BANK_OFFSET(0x10, Intro3Tiles)] = 0x53;
    mock_rom[ROM_BANK_OFFSET(0x10, ChristineTiles)] = 0x61;
    mock_rom[ROM_BANK_OFFSET(0x10, MarinBeachTiles)] = 0x62;
    mock_rom[ROM_BANK_OFFSET(0x10, FontLargeTiles)] = 0x63;
    mock_rom[ROM_BANK_OFFSET(0x10, FontLargeTiles + 0x100)] = 0x3E;
    mock_rom[ROM_BANK_OFFSET(0x10, FontLargeTiles + 0x200)] = 0x4F;
    mock_rom[ROM_BANK_OFFSET(0x10, ReliefTiles)] = 0x64;
    mock_rom[ROM_BANK_OFFSET(0x10, PaintingTiles)] = 0x65;

    mock_rom[ROM_BANK_OFFSET(0x35, PhotoAlbumTiles)] = 0xDD;
    mock_rom[ROM_BANK_OFFSET(0x35, PhotoAlbumTiles + 0x800)] = 0xDE;
    mock_rom[ROM_BANK_OFFSET(0x35, EndingCGBAltTiles)] = 0xDF;

    mock_rom[ROM_BANK_OFFSET(0x38, CreditsRollTiles)] = 0xEE;
    mock_rom[ROM_BANK_OFFSET(0x38, TitleDXTilesCGB)] = 0x81;
    mock_rom[ROM_BANK_OFFSET(0x38, TitleDXTilesDMG)] = 0x82;
    mock_rom[ROM_BANK_OFFSET(0x38, TitleDXOAMTiles)] = 0x83;
    mock_rom[ROM_BANK_OFFSET(0x38, TitleDXOAMTiles + 0x100)] = 0x84;
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

static void test_ending_scene_tiles_and_credits_roll(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));

    /* 1. func_2B92 */
    gb_write(&gb, hIsGBC, 0);
    func_2B92(&gb, FontLargeTiles + 0x100, BANK_FontLargeTiles);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x3E, "func_2B92 vTiles0 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x22, "func_2B92 vTiles1 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x11, "func_2B92 vTiles2 mismatch");

    /* 2. LoadCreditsLinkFaceCloseUpTiles DMG */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsLinkFaceCloseUpTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x18, "DMG LinkFaceCloseUp vTiles0 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x22, "DMG LinkFaceCloseUp vTiles1 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x11, "DMG LinkFaceCloseUp vTiles2 mismatch");

    /* 3. LoadCreditsLinkFaceCloseUpTiles CGB */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 1);
    mock_rom[ROM_BANK_OFFSET(0x33, EndingTiles + 0x3000)] = 0x23;
    mock_rom[ROM_BANK_OFFSET(0x33, EndingTiles + 0x2800)] = 0x12;
    LoadCreditsLinkFaceCloseUpTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xDF, "CGB LinkFaceCloseUp vTiles0 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x23, "CGB LinkFaceCloseUp vTiles1 mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x12, "CGB LinkFaceCloseUp vTiles2 mismatch");

    /* 4. LoadCreditsLinkSeatedOnLogTiles DMG */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsLinkSeatedOnLogTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x55, "DMG LinkSeatedOnLog vTiles0 mismatch");

    /* 5. LoadCreditsLinkSeatedOnLogTiles CGB */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 1);
    LoadCreditsLinkSeatedOnLogTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xDE, "CGB LinkSeatedOnLog vTiles0 mismatch");

    /* 6. LoadCreditsRollTiles DMG */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadCreditsRollTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xBC, "DMG CreditsRoll Npc3 tiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x100) == 0x4F, "DMG CreditsRoll FontLarge tiles mismatch");

    /* 7. LoadCreditsRollTiles CGB */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 1);
    mock_rom[ROM_BANK_OFFSET(0x32, Npc3Tiles + 0x2600)] = 0xBD;
    LoadCreditsRollTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0xEE, "CGB CreditsRoll tiles mismatch");
}

static void test_load_base_and_menu_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    /* Test LoadBaseTiles */
    LoadBaseTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x5A, "LoadBaseTiles LinkCharacterTiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x6B, "LoadBaseTiles InventoryEquipmentItemsTiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1 + 0x600) == 0x7C, "LoadBaseTiles Items1Tiles mismatch");
    TEST_ASSERT(gb.rom_bank == 0x01, "LoadBaseTiles bank not restored to 0x01");

    /* Test LoadMenuTiles */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);
    LoadMenuTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x5A, "LoadMenuTiles LinkCharacterTiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x8D, "LoadMenuTiles MenuTiles mismatch");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x9E, "LoadMenuTiles FontTiles mismatch");
}

static void test_load_intro_sequence_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadIntroSequenceTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x700) == 0x51, "IntroRainTiles not copied to vTiles0+0x700");
    TEST_ASSERT(gb_read(&gb, vTiles0) == 0x53, "Intro3Tiles not copied to vTiles0");
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x52, "Intro1Tiles not copied to vTiles1");
}

static void test_load_title_screen_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));

    /* DMG mode */
    gb_write(&gb, hIsGBC, 0);
    LoadTitleScreenTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x71, "TitleLogoTiles not copied to vTiles1");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x400) == 0x82, "TitleDXTilesDMG not copied to vTiles0+0x400");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x200) == 0x84, "TitleDXOAMTiles+0x100 not copied to vTiles0+0x200");

    /* CGB mode */
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 1);
    LoadTitleScreenTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x71, "TitleLogoTiles not copied to vTiles1 on CGB");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x400) == 0x81, "TitleDXTilesCGB not copied to vTiles0+0x400");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x200) == 0x83, "TitleDXOAMTiles not copied to vTiles0+0x200");
}

static void test_load_world_map_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadWorldMapTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles1 + 0x700) == 0x91, "WorldMapTiles not copied to vTiles1+0x700");
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x200) == 0x99, "Overworld1Tiles+0x100 not copied to vTiles0+0x200");
}

static void test_load_static_pictures_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    /* Relief */
    LoadFaceShrineReliefTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x64, "ReliefTiles not copied to vTiles2");

    /* Painting */
    gb_write(&gb, vTiles2, 0);
    LoadSchulePaintingTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x65, "PaintingTiles not copied to vTiles2");

    /* Christine */
    gb_write(&gb, vTiles2, 0);
    LoadChristinePortraitTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x61, "ChristineTiles not copied to vTiles2");
}

static void test_load_eagles_tower_top_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadEaglesTowerTopTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles1 + 0x400) == 0xA2, "EaglesTowerTop2Tiles not copied to vTiles1+0x400");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x11, "EaglesTowerTop1Tiles not copied to vTiles2");
}

static void test_load_marin_beach_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadMarinBeachTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles0 + 0x400) == 0x63, "FontLargeTiles not copied to vTiles0+0x400");
    TEST_ASSERT(gb_read(&gb, vTiles2) == 0x62, "MarinBeachTiles not copied to vTiles2");
}

static void test_load_save_menu_tiles(void) {
    GBState gb;
    gb_init(&gb);
    gb_attach_rom(&gb, mock_rom, sizeof(mock_rom));
    gb_write(&gb, hIsGBC, 0);

    LoadSaveMenuTiles(&gb);
    TEST_ASSERT(gb_read(&gb, vTiles1) == 0x72, "SaveMenuTiles not copied to vTiles1");
    TEST_ASSERT(gb.rom_bank == BANK_SaveMenuTiles, "Bank not set to BANK_SaveMenuTiles");
}

int run_gfx_tests(void) {
    printf("[*] Running GFX and Credits tile loading tests...\n");
    setup_mock_data();
    test_load_credits_koholint_disappearing_tiles();
    test_load_tileset_15();
    test_load_credits_stairs_tiles();
    test_load_credits_koholint_views_tiles();
    test_load_credits_link_on_sea_dmg_and_cgb();
    test_ending_scene_tiles_and_credits_roll();
    test_load_base_and_menu_tiles();
    test_load_intro_sequence_tiles();
    test_load_title_screen_tiles();
    test_load_world_map_tiles();
    test_load_static_pictures_tiles();
    test_load_eagles_tower_top_tiles();
    test_load_marin_beach_tiles();
    test_load_save_menu_tiles();

    if (gfx_failures == 0) {
        printf("  [PASS] All gfx.asm / credits tile loaders verified successfully!\n\n");
    }
    return gfx_failures;
}
