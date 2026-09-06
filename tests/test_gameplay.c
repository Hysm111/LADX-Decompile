#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/gameplay.h"
#include "constants/gameplay.h"
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

void run_gameplay_tests(void) {
    printf("[*] Running CheckPresentSaveScreen tests...\n");
    test_check_present_save_screen();

    printf("[*] Running returnFromGameplayHandler tests...\n");
    test_return_from_gameplay_handler();

    if (failures == 0) {
        printf("  [PASS] All gameplay.asm functions verified successfully!\n\n");
    }
}
