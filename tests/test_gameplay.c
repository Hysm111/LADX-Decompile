#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/gameplay.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_check_present_save_screen(void) {
    GBState gb;

    /* Base valid setup */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wDialogState, 0x00);
    gb_write(&gb, wC167, 0x00);
    gb_write(&gb, wRoomTransitionState, 0x00);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, GAMEPLAY_WORLD_INTERACTIVE);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wD474, 0x00);
    gb_write(&gb, wD464, 0x00);
    gb_write(&gb, wC16C, 0x55);

    /* Test valid presentation */
    bool presented = CheckPresentSaveScreen(&gb);
    TEST_ASSERT(presented == true, "Save screen should be presented");
    TEST_ASSERT(gb_read(&gb, wGameplayType) == GAMEPLAY_FILE_SAVE, "wGameplayType not set to GAMEPLAY_FILE_SAVE");
    TEST_ASSERT(gb_read(&gb, wGameplaySubtype) == 0, "wGameplaySubtype not cleared");
    TEST_ASSERT(gb_read(&gb, wTransitionSequenceCounter) == 0, "wTransitionSequenceCounter not cleared");
    TEST_ASSERT(gb_read(&gb, wC16C) == 0, "wC16C not cleared");
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0, "wDialogState not cleared");

    /* Condition 1: wTransitionSequenceCounter != 4 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x02);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite transition counter != 4");

    /* Condition 2: wDialogState != 0 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wDialogState, 0x01);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite dialog active");

    /* Condition 3: wC167 != 0 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wC167, 0x02);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite wC167 != 0");

    /* Condition 4: wRoomTransitionState != 0 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wRoomTransitionState, 0x01);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite room transition scrolling");

    /* Condition 5: wGameplayType >= GAMEPLAY_INVENTORY */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_INVENTORY);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented during inventory");

    /* Condition 6: Button combo missing Select */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented without Select pressed");

    /* Condition 7: wD474 != 0 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wD474, 0x01);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite wD474 != 0");

    /* Condition 8: wD464 != 0 */
    gb_init(&gb);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, hPressedButtonsMask, J_A | J_B | J_START | J_SELECT);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wD464, 0x01);
    TEST_ASSERT(CheckPresentSaveScreen(&gb) == false, "Presented despite wD464 != 0");
}

static int mock_dialog_calls = 0;
static int mock_palettes_calls = 0;

static void mock_execute_dialog(GBState *gb) {
    mock_dialog_calls++;
    TEST_ASSERT(gb->rom_bank == 0x0F, "Dialog executed in wrong bank");
}

static void mock_load_bg_palettes(GBState *gb) {
    mock_palettes_calls++;
    TEST_ASSERT(gb->rom_bank == 0x24, "LoadBGPalettes executed in wrong bank");
}

static void test_return_from_gameplay_handler(void) {
    GBState gb;

    /* DMG mode: executes dialog in bank 0x0F, returns without palette call */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 0x00);
    mock_dialog_calls = 0;
    mock_palettes_calls = 0;

    returnFromGameplayHandler(&gb, mock_execute_dialog, mock_load_bg_palettes);
    TEST_ASSERT(mock_dialog_calls == 1, "Dialog handler not called on DMG");
    TEST_ASSERT(mock_palettes_calls == 0, "Palette handler called on DMG");

    /* CGB mode: executes dialog in bank 0x0F, then loads palettes in bank 0x24 */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 0x01);
    mock_dialog_calls = 0;
    mock_palettes_calls = 0;

    returnFromGameplayHandler(&gb, mock_execute_dialog, mock_load_bg_palettes);
    TEST_ASSERT(mock_dialog_calls == 1, "Dialog handler not called on CGB");
    TEST_ASSERT(mock_palettes_calls == 1, "Palette handler not called on CGB");
    TEST_ASSERT(gb.rom_bank == 0x24, "Final bank not 0x24 on CGB");
}

static void test_enable_sram(void) {
    GBState gb;
    gb_init(&gb);

    gb.sram_bank = 2;
    gb.sram_enabled = false;

    EnableSRAM(&gb);
    TEST_ASSERT(gb.sram_bank == 0, "SRAM bank not reset to 0");
    TEST_ASSERT(gb.sram_enabled == true, "SRAM not enabled");
}

static int mock_sync_calls = 0;
static void mock_sync_func(GBState *gb) {
    mock_sync_calls++;
    TEST_ASSERT(gb->rom_bank == 0x01, "Sync function not executed in bank 1");
}

static void test_synchronize_dungeons_trampoline(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x06);
    gb.rom_bank = 0x06;

    mock_sync_calls = 0;
    SynchronizeDungeonsItemFlags_trampoline(&gb, mock_sync_func);
    TEST_ASSERT(mock_sync_calls == 1, "Sync function was not called");
    TEST_ASSERT(gb.rom_bank == 0x06, "Current bank not restored");
}

static void test_get_random_byte(void) {
    GBState gb;
    gb_init(&gb);

    /* sum = 0x10 + 0x20 + 0x30 = 0x60 (0110 0000b) -> rrca is 0x30 (0011 0000b) */
    gb_write(&gb, hFrameCounter, 0x10);
    gb_write(&gb, wRandomSeed, 0x20);
    gb_write(&gb, rLY, 0x30);

    uint8_t rnd = GetRandomByte(&gb);
    TEST_ASSERT(rnd == 0x30, "Random byte did not match expected rrca value");
    TEST_ASSERT(gb_read(&gb, wRandomSeed) == 0x30, "wRandomSeed not updated");

    /* sum = 0x01 + 0x00 + 0x00 = 0x01 (0000 0001b) -> rrca is 0x80 (1000 0000b) */
    gb_write(&gb, hFrameCounter, 0x01);
    gb_write(&gb, wRandomSeed, 0x00);
    gb_write(&gb, rLY, 0x00);

    rnd = GetRandomByte(&gb);
    TEST_ASSERT(rnd == 0x80, "Random byte bit 0 rotation failed");
    TEST_ASSERT(gb_read(&gb, wRandomSeed) == 0x80, "wRandomSeed not updated for bit 0 rotation");
}

static void test_read_joypad_state(void) {
    GBState gb;

    /* 1. Map transition active -> early return, joypad untouched */
    gb_init(&gb);
    gb_write(&gb, wRoomTransitionState, 0x01);
    gb_write(&gb, hPressedButtonsMask, 0x55);
    gb_write(&gb, hJoypadState, 0xAA);
    gb.joypad_input = J_A;
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == 0x55, "Buttons modified during room transition");
    TEST_ASSERT(gb_read(&gb, hJoypadState) == 0xAA, "JoypadState modified during room transition");

    /* 2. World gameplay, not interactive -> clear joypad */
    gb_init(&gb);
    gb_write(&gb, wRoomTransitionState, 0x00);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, 0x00); /* not interactive */
    gb_write(&gb, hPressedButtonsMask, 0x55);
    gb_write(&gb, hJoypadState, 0xAA);
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == 0x00, "Mask not cleared for non-interactive world");
    TEST_ASSERT(gb_read(&gb, hJoypadState) == 0x00, "State not cleared for non-interactive world");

    /* 3. World interactive, transition counter != 4 -> clear joypad */
    gb_init(&gb);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, GAMEPLAY_WORLD_INTERACTIVE);
    gb_write(&gb, wTransitionSequenceCounter, 0x02);
    gb_write(&gb, hPressedButtonsMask, 0x55);
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == 0x00, "Mask not cleared when transition counter != 4");

    /* 4. World interactive, wPaletteUnknownE != 0 -> clear joypad */
    gb_init(&gb);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, GAMEPLAY_WORLD_INTERACTIVE);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wPaletteUnknownE, 0x01);
    gb_write(&gb, hPressedButtonsMask, 0x55);
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == 0x00, "Mask not cleared when wPaletteUnknownE != 0");

    /* 5. World interactive, pass out animation exception: motion pass out and physics modifier == 4 */
    gb_init(&gb);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, GAMEPLAY_WORLD_INTERACTIVE);
    gb_write(&gb, wTransitionSequenceCounter, 0x00); /* normally would block */
    gb_write(&gb, wLinkMotionState, LINK_MOTION_PASS_OUT);
    gb_write(&gb, hLinkPhysicsModifier, 0x04);
    gb.joypad_input = J_B;
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == J_B, "Pass out exception did not read joypad");
    TEST_ASSERT(gb_read(&gb, hJoypadState) == J_B, "Pass out exception did not set newly pressed button");

    /* 6. Normal interactive world read: previous button held, new button pressed */
    gb_init(&gb);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wGameplaySubtype, GAMEPLAY_WORLD_INTERACTIVE);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wPaletteUnknownE, 0x00);
    gb_write(&gb, hPressedButtonsMask, J_A);
    gb.joypad_input = J_A | J_START;
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == (J_A | J_START), "Held buttons mask incorrect");
    TEST_ASSERT(gb_read(&gb, hJoypadState) == J_START, "Newly pressed joypad state incorrect");
    TEST_ASSERT(gb_read(&gb, rP1) == (J_BUTTONS | J_DPAD), "rP1 not reset to J_BUTTONS | J_DPAD");

    /* 7. Non-world gameplay (e.g. file save menu) reads joypad directly */
    gb_init(&gb);
    gb_write(&gb, wGameplayType, GAMEPLAY_FILE_SAVE);
    gb.joypad_input = J_UP | J_SELECT;
    ReadJoypadState(&gb);
    TEST_ASSERT(gb_read(&gb, hPressedButtonsMask) == (J_UP | J_SELECT), "Non-world menu joypad read failed");
    TEST_ASSERT(gb_read(&gb, hJoypadState) == (J_UP | J_SELECT), "Non-world menu newly pressed failed");
}

static void test_get_intersected_object_bg_address(void) {
    GBState gb;
    gb_init(&gb);

    /* top = 0x18 + 0x08 = 0x20 -> row = 4. left = 0x10 + 0x00 = 0x10 -> col = 2 */
    /* address = 0x9800 + 4 * 32 + 2 = 0x9882 */
    gb_write(&gb, hIntersectedObjectTop, 0x18);
    gb_write(&gb, hBaseScrollY, 0x08);
    gb_write(&gb, hIntersectedObjectLeft, 0x10);
    gb_write(&gb, hBaseScrollX, 0x00);

    uint16_t addr = GetIntersectedObjectBGAddress(&gb);
    TEST_ASSERT(addr == 0x9882, "Calculated BG address mismatch");
    TEST_ASSERT(gb_read(&gb, hIntersectedObjectBGAddressHigh) == 0x98, "High byte mismatch");
    TEST_ASSERT(gb_read(&gb, hIntersectedObjectBGAddressLow) == 0x82, "Low byte mismatch");
}

static int table_call_idx = -1;
static void jump_target_0(GBState *gb) {
    (void)gb;
    table_call_idx = 0;
}
static void jump_target_1(GBState *gb) {
    (void)gb;
    table_call_idx = 1;
}
static void jump_target_2(GBState *gb) {
    (void)gb;
    table_call_idx = 2;
}

static void test_table_jump(void) {
    GBState gb;
    gb_init(&gb);

    const JumpTableFunc table[] = {
        jump_target_0,
        jump_target_1,
        jump_target_2,
    };

    table_call_idx = -1;
    TableJump(&gb, 0, table);
    TEST_ASSERT(table_call_idx == 0, "TableJump index 0 failed");

    table_call_idx = -1;
    TableJump(&gb, 1, table);
    TEST_ASSERT(table_call_idx == 1, "TableJump index 1 failed");

    table_call_idx = -1;
    TableJump(&gb, 2, table);
    TEST_ASSERT(table_call_idx == 2, "TableJump index 2 failed");
}

void run_gameplay_tests(void) {
    printf("[*] Running CheckPresentSaveScreen tests...\n");
    test_check_present_save_screen();

    printf("[*] Running returnFromGameplayHandler tests...\n");
    test_return_from_gameplay_handler();

    printf("[*] Running EnableSRAM tests...\n");
    test_enable_sram();

    printf("[*] Running SynchronizeDungeonsItemFlags_trampoline tests...\n");
    test_synchronize_dungeons_trampoline();

    printf("[*] Running GetRandomByte tests...\n");
    test_get_random_byte();

    printf("[*] Running ReadJoypadState tests...\n");
    test_read_joypad_state();

    printf("[*] Running GetIntersectedObjectBGAddress tests...\n");
    test_get_intersected_object_bg_address();

    printf("[*] Running TableJump tests...\n");
    test_table_jump();

    if (failures == 0) {
        printf("  [PASS] All gameplay.asm functions verified successfully!\n\n");
    }
}
