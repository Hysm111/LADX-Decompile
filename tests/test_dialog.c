#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/joypad.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/dialog.h"
#include "constants/hardware.h"
#include "constants/dialog.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_dialog_lookups(void) {
    GBState gb;
    gb_init(&gb);

    /* Allocate test ROM buffer: 32 banks (0x80000 bytes) */
    uint8_t rom[0x80000];
    memset(rom, 0, sizeof(rom));

    /* Bank $1C is at file offset 0x1C * 0x4000 = 0x70000 */
    /* Map address $4641 is offset 0x70000 + (0x4641 - 0x4000) = 0x70641 */
    /* Map address $4741 is offset 0x70000 + (0x4741 - 0x4000) = 0x70741 */
    uint32_t ascii_base = 0x70000 + (ADDR_CodepointToTileMap - 0x4000);
    uint32_t diacr_base = 0x70000 + (ADDR_CodepointToDiacritic - 0x4000);

    rom[ascii_base + 0x20] = 0x5A; /* Codepoint 0x20 -> tile 0x5A */
    rom[ascii_base + 0x41] = 0x7B; /* Codepoint 0x41 -> tile 0x7B */

    rom[diacr_base + 0x05] = 0x12; /* Codepoint 0x05 -> diacritic 0x12 */
    rom[diacr_base + 0x10] = 0x34; /* Codepoint 0x10 -> diacritic 0x34 */

    gb_attach_rom(&gb, rom, sizeof(rom));

    /* Initial ROM bank is 5 */
    gb.rom_bank = 5;

    /* Test ReadValueInDialogsBank directly */
    uint8_t val1 = ReadValueInDialogsBank(&gb, ADDR_CodepointToTileMap, 0x20);
    TEST_ASSERT(val1 == 0x5A, "ReadValueInDialogsBank returned incorrect value");
    TEST_ASSERT(gb.rom_bank == 1, "ReadValueInDialogsBank did not restore bank 1");

    /* Test ReadTileValueFromAsciiTable */
    gb.rom_bank = 7;
    uint8_t val2 = ReadTileValueFromAsciiTable(&gb, 0x41);
    TEST_ASSERT(val2 == 0x7B, "ReadTileValueFromAsciiTable returned incorrect tile");
    TEST_ASSERT(gb.rom_bank == 1, "ReadTileValueFromAsciiTable did not restore bank 1");

    /* Test ReadTileValueFromDiacriticsTable */
    gb.rom_bank = 9;
    uint8_t val3 = ReadTileValueFromDiacriticsTable(&gb, 0x05);
    TEST_ASSERT(val3 == 0x12, "ReadTileValueFromDiacriticsTable returned incorrect diacritic");
    TEST_ASSERT(gb.rom_bank == 1, "ReadTileValueFromDiacriticsTable did not restore bank 1");

    uint8_t val4 = ReadTileValueFromDiacriticsTable(&gb, 0x10);
    TEST_ASSERT(val4 == 0x34, "ReadTileValueFromDiacriticsTable 2nd check returned incorrect diacritic");
    TEST_ASSERT(gb.rom_bank == 1, "ReadTileValueFromDiacriticsTable did not restore bank 1");
}


static int test_dlg_cb_14_called = 0;
static void mock_dlg_cb_14(GBState *gb) {
    test_dlg_cb_14_called++;
    TEST_ASSERT(gb->rom_bank == 0x14, "ROM bank not 0x14");
}

static int test_dlg_cb_1c_called = 0;
static void mock_dlg_cb_1c(GBState *gb) {
    test_dlg_cb_1c_called++;
    TEST_ASSERT(gb->rom_bank == 0x1C, "ROM bank not 0x1C");
}

static int test_dlg_cb_21_called = 0;
static void mock_dlg_cb_21(GBState *gb) {
    test_dlg_cb_21_called++;
    TEST_ASSERT(gb->rom_bank == 0x21, "ROM bank not 0x21");
}

static int test_clear_pixels_called = 0;
static void mock_clear_pixels(GBState *gb) {
    test_clear_pixels_called++;
    TEST_ASSERT(gb->rom_bank == 0x1C, "ROM bank not 0x1C");
}

static void test_dialog_state_machine(void) {
    GBState gb;

    /* 1. OpenDialogInTable0 */
    gb_init(&gb);
    gb_write(&gb, hLinkPositionY, 0x30); /* < 0x48 -> bottom flag set (0x81) */
    OpenDialogInTable0(&gb, 0x42);
    TEST_ASSERT(gb_read(&gb, wDialogIndex) == 0x42, "wDialogIndex mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogIndexHi) == 0x00, "wDialogIndexHi mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogAskSelectionIndex) == 0x00, "wDialogAskSelectionIndex mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogOpenCloseAnimationFrame) == 0x00, "wDialogOpenCloseAnimationFrame mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogCharacterIndex) == 0x00, "wDialogCharacterIndex mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogCharacterIndexHi) == 0x00, "wDialogCharacterIndexHi mismatch");
    TEST_ASSERT(gb_read(&gb, wNameIndex) == 0x00, "wNameIndex mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogSFX) == 0x0F, "wDialogSFX mismatch");
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x81, "wDialogState bottom flag mismatch");

    /* Link Y >= 0x48 -> top flag (0x01) */
    gb_write(&gb, hLinkPositionY, 0x50);
    OpenDialogInTable0(&gb, 0x10);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x01, "wDialogState top flag mismatch");

    /* 2. OpenDialogInTable1 and OpenDialogInTable2 */
    OpenDialogInTable1(&gb, 0x25);
    TEST_ASSERT(gb_read(&gb, wDialogIndex) == 0x25, "table 1 dialog index");
    TEST_ASSERT(gb_read(&gb, wDialogIndexHi) == 0x01, "table 1 dialog index hi");

    OpenDialogInTable2(&gb, 0x33);
    TEST_ASSERT(gb_read(&gb, wDialogIndex) == 0x33, "table 2 dialog index");
    TEST_ASSERT(gb_read(&gb, wDialogIndexHi) == 0x02, "table 2 dialog index hi");

    /* 3. DialogOpenAnimationStartHandler */
    gb_init(&gb);
    test_dlg_cb_14_called = 0;
    DialogOpenAnimationStartHandler(&gb, mock_dlg_cb_14);
    TEST_ASSERT(test_dlg_cb_14_called == 1, "DialogOpenAnimationStartHandler callback not called");

    /* 4. DialogOpenAnimationHandler (NOP) */
    DialogOpenAnimationHandler(&gb);

    /* 5. DialogClosingEndHandler */
    gb_init(&gb);
    gb_write(&gb, wDialogState, 0x8E);
    gb_write(&gb, hIsGBC, 0);
    DialogClosingEndHandler(&gb, mock_dlg_cb_21);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0, "wDialogState not cleared");
    TEST_ASSERT(gb_read(&gb, wDialogCooldown) == DIALOG_COOLDOWN, "cooldown not set");

    /* CGB with GAMEPLAY_WORLD and palette effect address >= 8 */
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wBGPaletteEffectAddress, 0x08);
    test_dlg_cb_21_called = 0;
    DialogClosingEndHandler(&gb, mock_dlg_cb_21);
    TEST_ASSERT(test_dlg_cb_21_called == 1, "CGB DialogClosingEndHandler callback not called");

    /* 6. DialogOpenAnimationEndHandler */
    test_dlg_cb_1c_called = 0;
    DialogOpenAnimationEndHandler(&gb, mock_dlg_cb_1c);
    TEST_ASSERT(test_dlg_cb_1c_called == 1, "DialogOpenAnimationEndHandler callback not called");

    /* 7. IncrementDialogState and IncrementDialogStateAndReturn */
    gb_write(&gb, wDialogState, 0x05);
    IncrementDialogState(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x06, "IncrementDialogState failed");
    IncrementDialogStateAndReturn(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x07, "IncrementDialogStateAndReturn failed");

    /* 8. UpdateDialogState */
    gb_write(&gb, wDialogOpenCloseAnimationFrame, 4);
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wDialogState, 0x87); /* bottom flag + state 7 */
    UpdateDialogState(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogOpenCloseAnimationFrame) == 0, "animation frame not cleared");
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x8E, "UpdateDialogState failed to set closing state");

    /* Photo album mode */
    gb_write(&gb, wGameplayType, GAMEPLAY_PHOTO_ALBUM);
    gb_write(&gb, wDialogState, 0x87);
    UpdateDialogState(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0, "photo album dialog state not 0");

    /* 9. DialogFinishedHandler */
    gb_write(&gb, wGameplayType, GAMEPLAY_WORLD);
    gb_write(&gb, wDialogState, 0x8C); /* state END */
    gb_write(&gb, wDialogInteractionLocked, 1);
    gb_write(&gb, hJoypadState, J_A);
    DialogFinishedHandler(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x8C, "locked dialog responded to joypad");

    gb_write(&gb, wDialogInteractionLocked, 0);
    gb_write(&gb, hJoypadState, 0);
    DialogFinishedHandler(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x8C, "no buttons pressed responded to joypad");

    gb_write(&gb, hJoypadState, J_B);
    DialogFinishedHandler(&gb);
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x8E, "pressed B did not advance to closing");

    /* 10. DialogClosingBeginHandler */
    test_dlg_cb_1c_called = 0;
    DialogClosingBeginHandler(&gb, mock_dlg_cb_1c);
    TEST_ASSERT(test_dlg_cb_1c_called == 1, "DialogClosingBeginHandler callback not called");

    /* 11. DialogLetterAnimationStartHandler */
    gb_write(&gb, wDialogState, 0x06);
    gb_write(&gb, wDialogScrollDelay, 2);
    test_clear_pixels_called = 0;
    DialogLetterAnimationStartHandler(&gb, mock_clear_pixels);
    TEST_ASSERT(gb_read(&gb, wDialogScrollDelay) == 1, "delay not decremented");
    TEST_ASSERT(test_clear_pixels_called == 0, "pixels cleared before delay over");
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x06, "state incremented prematurely");

    gb_write(&gb, wDialogScrollDelay, 0);
    DialogLetterAnimationStartHandler(&gb, mock_clear_pixels);
    TEST_ASSERT(test_clear_pixels_called == 1, "clear pixels not called");
    TEST_ASSERT(gb_read(&gb, wDialogState) == 0x07, "state not incremented after clear pixels");
}

void run_dialog_tests(void) {
    printf("[*] Running Dialog lookup tests...\n");
    test_dialog_lookups();
    test_dialog_state_machine();

    if (failures == 0) {
        printf("  [PASS] All dialog.asm functions verified successfully!\n\n");
    }
}
