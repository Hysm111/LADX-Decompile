#include "home/animated_tiles.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static int failures = 0;
static int mock_skip_calls = 0;
static int mock_counter_calls = 0;
static int mock_photo_calls = 0;
static int mock_config_calls = 0;

static uint8_t test_rom[0x4000 * 0x40]; /* 64 banks */

static void init_test_gb(GBState *gb) {
    gb_init(gb);
    memset(test_rom, 0, sizeof(test_rom));
    gb->rom = test_rom;
    gb->rom_size = sizeof(test_rom);
}

static void mock_skip_func(GBState *gb) {
    (void)gb;
    mock_skip_calls++;
}

static void mock_counter_func(GBState *gb) {
    (void)gb;
    mock_counter_calls++;
}

static void mock_photo_func(GBState *gb) {
    (void)gb;
    mock_photo_calls++;
}

static void mock_config_func(GBState *gb, uint8_t flag, uint8_t *out_bank) {
    (void)gb;
    mock_config_calls++;
    if (flag == 1) {
        *out_bank = 0x22;
    } else {
        *out_bank = 0x23;
    }
}

static void test_increment_animated_tiles_data_offset(void) {
    GBState gb;
    init_test_gb(&gb);
    gb_write(&gb, hAnimatedTilesDataOffset, 0x40);

    uint8_t res = IncrementAnimatedTilesDataOffset(&gb);
    TEST_ASSERT(res == 0x80, "IncrementAnimatedTilesDataOffset did not add 0x40");
    TEST_ASSERT(gb_read(&gb, hAnimatedTilesDataOffset) == 0x80, "hAnimatedTilesDataOffset register mismatch");

    res = IncrementAnimatedTilesDataOffset(&gb);
    TEST_ASSERT(res == 0xC0, "Second increment failed");
}

static void test_skip_tiles_group_animation(void) {
    GBState gb;
    init_test_gb(&gb);
    mock_skip_calls = 0;

    SkipTilesGroupAnimation(&gb, mock_skip_func);
    TEST_ASSERT(mock_skip_calls == 1, "func_020_54f5 not called");
    TEST_ASSERT(gb.rom_bank == 0x0C, "ROM bank not adjusted to 0x0C");
}

static void test_load_animated_tiles_frame(void) {
    GBState gb;
    init_test_gb(&gb);
    gb.rom_bank = 0x0C;

    /* Setup 0x40 source bytes at bank 0x0C offset (0x6A00 - 0x4000) */
    size_t rom_offset = (size_t)0x0C * 0x4000 + (0x6A00 - 0x4000);
    for (int i = 0; i < 0x40; i++) {
        test_rom[rom_offset + i] = (uint8_t)(i + 1);
    }

    LoadAnimatedTilesFrame(&gb, 0x6A00, 0x96C0, NULL);
    for (int i = 0; i < 0x40; i++) {
        TEST_ASSERT(gb_read(&gb, 0x96C0 + i) == (uint8_t)(i + 1), "VRAM copy mismatch in LoadAnimatedTilesFrame");
    }

    /* Test MAP_COLOR_DUNGEON special branch */
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    mock_config_calls = 0;
    LoadAnimatedTilesFrame(&gb, 0x6A00, 0x96C0, mock_config_func);
    TEST_ASSERT(mock_config_calls == 2, "ConfigureAnimatedTilesCopy not called twice for Color Dungeon");
}

static void test_animate_counter_tiles_group(void) {
    GBState gb;

    /* Frame not matching (count & 7 != 0) -> calls Skip */
    init_test_gb(&gb);
    gb_write(&gb, hAnimatedTilesFrameCount, 1);
    mock_skip_calls = 0;
    mock_counter_calls = 0;
    AnimateCounterTilesGroup(&gb, mock_counter_func, mock_skip_func);
    TEST_ASSERT(mock_skip_calls == 1, "Skip not called on non-matching frame");
    TEST_ASSERT(mock_counter_calls == 0, "Counter called on non-matching frame");

    /* Frame matching (count & 7 == 0) -> calls counter routine */
    gb_write(&gb, hAnimatedTilesFrameCount, 8);
    mock_skip_calls = 0;
    mock_counter_calls = 0;
    AnimateCounterTilesGroup(&gb, mock_counter_func, mock_skip_func);
    TEST_ASSERT(mock_counter_calls == 1, "Counter routine not called on matching frame");
    TEST_ASSERT(gb.rom_bank == 0x0C, "ROM bank not set to 0x0C after counter tiles update");
}

static void test_slow_speed_tile_groups(void) {
    GBState gb;
    init_test_gb(&gb);

    /* Test slow speed skip (frame & 0xF != 0) */
    gb_write(&gb, hAnimatedTilesFrameCount, 3);
    mock_skip_calls = 0;
    AnimateTideTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 1, "Tide group skip failed");

    AnimateVillageTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 2, "Village group skip failed");

    AnimateWaterDungeonTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 3, "Water dungeon group skip failed");

    AnimateSlowWaterfallTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 4, "Slow waterfall group skip failed");

    AnimateUndergroundTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 5, "Underground group skip failed");

    /* Test slow speed frame match (frame & 0xF == 0) */
    gb_write(&gb, hAnimatedTilesFrameCount, 0x10);
    gb_write(&gb, hAnimatedTilesDataOffset, 0);
    AnimateTideTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(gb_read(&gb, hAnimatedTilesDataOffset) == 0x40, "Offset not incremented by AnimateTideTilesGroup");
}

static void test_medium_and_fast_speed_tile_groups(void) {
    GBState gb;
    init_test_gb(&gb);

    /* Medium speed skips on frame & 7 != 0 */
    gb_write(&gb, hAnimatedTilesFrameCount, 1);
    mock_skip_calls = 0;
    AnimateWarpTilesGroup(&gb, mock_skip_func, NULL);
    AnimateBubblesTilesGroup(&gb, mock_skip_func, NULL);
    AnimateWeatherVaneTilesGroup(&gb, mock_skip_func, NULL);
    AnimateCrystalBlockTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 4, "Medium speed groups skip failed");

    /* Fast speed skips on frame & 3 != 0 */
    gb_write(&gb, hAnimatedTilesFrameCount, 1);
    mock_skip_calls = 0;
    AnimateWaterCurrentsTilesGroup(&gb, mock_skip_func, NULL);
    AnimateWaterfallTilesGroup(&gb, mock_skip_func, NULL);
    AnimateLightBeamTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 3, "Fast speed groups skip failed");

    /* Fast speed executes on frame & 3 == 0 */
    gb_write(&gb, hAnimatedTilesFrameCount, 4);
    gb_write(&gb, hAnimatedTilesDataOffset, 0);
    AnimateWaterCurrentsTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(gb_read(&gb, hAnimatedTilesDataOffset) == 0x40, "Offset not incremented by fast speed group");
}

static void test_dungeon1_and_lava_tiles_groups(void) {
    GBState gb;
    init_test_gb(&gb);
    gb.rom_bank = 0x0C;

    /* Frame not matching */
    gb_write(&gb, hAnimatedTilesFrameCount, 2);
    mock_skip_calls = 0;
    AnimateDungeon1TilesGroup(&gb, mock_skip_func, NULL);
    AnimateLavaTilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 2, "Dungeon 1 / Lava skip failed");

    /* Frame matching (0x10 -> index 2 -> offset 0x80) */
    gb_write(&gb, hAnimatedTilesFrameCount, 0x10);
    size_t rom_offset = (size_t)0x0C * 0x4000 + (0x6D80 - 0x4000);
    for (int i = 0; i < 0x40; i++) {
        test_rom[rom_offset + i] = 0xAA;
    }
    AnimateDungeon1TilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(gb_read(&gb, 0x96C0) == 0xAA, "Dungeon 1 tile copy mismatch");
}

static void test_dungeon2_and_label_1cb8(void) {
    GBState gb;
    init_test_gb(&gb);

    /* Non-Color Dungeon: calls label_1CB8 */
    gb_write(&gb, hMapId, 0x01);
    /* next_count = 1 + 1 = 2 -> (2 & 3) != 0 -> AnimateDungeon1TilesGroup */
    gb_write(&gb, hAnimatedTilesFrameCount, 1);
    mock_skip_calls = 0;
    AnimateDungeon2TilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(mock_skip_calls == 1, "Dungeon 2 forward to Dungeon 1 failed");

    /* Color Dungeon: loads from wAnimatedScrollingTilesStorage to 0x8400 */
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    for (int i = 0; i < 0x40; i++) {
        gb_write(&gb, wAnimatedScrollingTilesStorage + i, 0x55);
    }
    AnimateDungeon2TilesGroup(&gb, mock_skip_func, NULL);
    TEST_ASSERT(gb_read(&gb, 0x8400) == 0x55, "Color Dungeon 2 scrolling tiles copy mismatch");
}

static void test_animate_photo_tiles_group(void) {
    GBState gb;
    init_test_gb(&gb);
    mock_photo_calls = 0;

    AnimatePhotoTilesGroup(&gb, mock_photo_func);
    TEST_ASSERT(mock_photo_calls == 1, "Photo group callback not called");
    TEST_ASSERT(gb.rom_bank == 0x38, "ROM bank not set to 0x38 for photo group");
}

static void test_copy_link_tiles_pair(void) {
    GBState gb;
    init_test_gb(&gb);

    /* Use WRAM source */
    for (int i = 0; i < 8; i++) {
        gb_write(&gb, 0xC500 + i, (uint8_t)(i + 0x10));
    }

    CopyLinkTilesPair(&gb, 0xC500, 0x8000, 8);
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT(gb_read(&gb, 0x8000 + i) == (uint8_t)(i + 0x10), "CopyLinkTilesPair data mismatch");
    }
    TEST_ASSERT(gb.rom_bank == 0x20, "ROM bank not restored to 0x20 after CopyLinkTilesPair");
}

static void test_animate_marin_beach_tiles(void) {
    GBState gb;

    /* 1. Pending draw command -> returns early */
    init_test_gb(&gb);
    gb_write(&gb, wDrawCommand, 1);
    AnimateMarinBeachTiles(&gb);
    TEST_ASSERT(gb.rom_bank == 1, "ROM bank changed despite draw command active");

    /* 2. Valid frame counter & 0x0F == 0 */
    init_test_gb(&gb);
    gb_write(&gb, wDrawCommand, 0);
    gb_write(&gb, hFrameCounter, 0x00); /* & 0x0F == 0, & 0x30 == 0 */
    size_t rom_offset = (size_t)0x10 * 0x4000 + (0x6500 - 0x4000);
    for (int i = 0; i < 0x40; i++) {
        test_rom[rom_offset + i] = (uint8_t)(i + 0x77);
    }
    AnimateMarinBeachTiles(&gb);
    TEST_ASSERT(gb_read(&gb, 0x9500) == 0x77, "Marin beach tiles copy mismatch");
    TEST_ASSERT(gb.rom_bank == 0x10, "ROM bank not set to 0x10");
}

void run_animated_tiles_tests(void) {
    failures = 0;

    printf("[*] Running IncrementAnimatedTilesDataOffset tests...\n");
    test_increment_animated_tiles_data_offset();

    printf("[*] Running SkipTilesGroupAnimation tests...\n");
    test_skip_tiles_group_animation();

    printf("[*] Running LoadAnimatedTilesFrame tests...\n");
    test_load_animated_tiles_frame();

    printf("[*] Running AnimateCounterTilesGroup tests...\n");
    test_animate_counter_tiles_group();

    printf("[*] Running Slow-speed animated tile group tests...\n");
    test_slow_speed_tile_groups();

    printf("[*] Running Medium and Fast-speed animated tile group tests...\n");
    test_medium_and_fast_speed_tile_groups();

    printf("[*] Running Dungeon 1 and Lava tile group tests...\n");
    test_dungeon1_and_lava_tiles_groups();

    printf("[*] Running Dungeon 2 and label_1CB8 tests...\n");
    test_dungeon2_and_label_1cb8();

    printf("[*] Running AnimatePhotoTilesGroup tests...\n");
    test_animate_photo_tiles_group();

    printf("[*] Running CopyLinkTilesPair tests...\n");
    test_copy_link_tiles_pair();

    printf("[*] Running AnimateMarinBeachTiles tests...\n");
    test_animate_marin_beach_tiles();

    if (failures == 0) {
        printf("  [PASS] All animated_tiles.asm functions verified successfully!\n");
    } else {
        printf("  [FAIL] %d animated_tiles test(s) failed.\n", failures);
        exit(1);
    }
}
