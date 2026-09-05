#include <stdio.h>
#include <assert.h>
#include "gb.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_adjust_bank_number_for_gbc(void) {
    GBState gb;
    gb_init(&gb);

    /* Test DMG mode (hIsGBC = 0) */
    gb_write(&gb, hIsGBC, 0);
    TEST_ASSERT(AdjustBankNumberForGBC(&gb, 0x05) == 0x05, "DMG bank 0x05 altered unexpectedly");
    TEST_ASSERT(AdjustBankNumberForGBC(&gb, 0x12) == 0x12, "DMG bank 0x12 altered unexpectedly");

    /* Test GBC mode (hIsGBC = 1) */
    gb_write(&gb, hIsGBC, 1);
    TEST_ASSERT(AdjustBankNumberForGBC(&gb, 0x05) == 0x25, "GBC bank 0x05 bit 5 not set");
    TEST_ASSERT(AdjustBankNumberForGBC(&gb, 0x12) == 0x32, "GBC bank 0x12 bit 5 not set");
}

static void test_switch_bank(void) {
    GBState gb;
    gb_init(&gb);

    SwitchBank(&gb, 0x07);
    TEST_ASSERT(gb.rom_bank == 0x07, "ROM bank not switched to 0x07");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x07, "wCurrentBank not updated to 0x07");
}

static void test_switch_adjusted_bank(void) {
    GBState gb;
    gb_init(&gb);

    /* DMG mode */
    gb_write(&gb, hIsGBC, 0);
    SwitchAdjustedBank(&gb, 0x03);
    TEST_ASSERT(gb.rom_bank == 0x03, "DMG SwitchAdjustedBank did not set bank 3");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x03, "DMG wCurrentBank not 3");

    /* GBC mode */
    gb_write(&gb, hIsGBC, 1);
    SwitchAdjustedBank(&gb, 0x03);
    TEST_ASSERT(gb.rom_bank == 0x23, "GBC SwitchAdjustedBank did not set bank 0x23");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x23, "GBC wCurrentBank not 0x23");
}

static void test_reload_saved_bank(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x14);
    gb.rom_bank = 0x01;

    ReloadSavedBank(&gb);
    TEST_ASSERT(gb.rom_bank == 0x14, "ReloadSavedBank did not restore wCurrentBank");
}

static void test_copy_objects_attributes_to_wram2(void) {
    GBState gb;
    gb_init(&gb);

    /* Allocate simulated ROM buffer with 4 banks */
    uint8_t test_rom[0x10000];
    for (int i = 0; i < 0x10000; i++) {
        test_rom[i] = 0;
    }
    /* Bank 3 is at offset 0xC000 in ROM */
    test_rom[0xC000] = 0xDE;
    test_rom[0xC001] = 0xAD;
    test_rom[0xC002] = 0xBE;
    test_rom[0xC003] = 0xEF;

    gb_attach_rom(&gb, test_rom, sizeof(test_rom));

    /* Set source bank in hMultiPurpose0 to 3 */
    gb_write(&gb, hMultiPurpose0, 3);

    /* Copy 4 bytes from 0x4000 to WRAM2 (0xD100) */
    CopyObjectsAttributesToWRAM2(&gb, 0xD100, 0x4000, 4);

    /* Check WRAM bank 2 contains copied data */
    TEST_ASSERT(gb.wram[2][0x100] == 0xDE, "WRAM2 byte 0 incorrect");
    TEST_ASSERT(gb.wram[2][0x101] == 0xAD, "WRAM2 byte 1 incorrect");
    TEST_ASSERT(gb.wram[2][0x102] == 0xBE, "WRAM2 byte 2 incorrect");
    TEST_ASSERT(gb.wram[2][0x103] == 0xEF, "WRAM2 byte 3 incorrect");

    /* Verify WRAM bank was restored to 0 (or 1) and ROM bank to 0x20 */
    TEST_ASSERT(gb.wram_bank == 1 || gb.wram_bank == 0, "WRAM bank not restored");
    TEST_ASSERT(gb.rom_bank == 0x20, "ROM bank not restored to 0x20");
}

int run_bank_tests(void) {
    printf("[*] Running AdjustBankNumberForGBC tests...\n");
    test_adjust_bank_number_for_gbc();

    printf("[*] Running SwitchBank tests...\n");
    test_switch_bank();

    printf("[*] Running SwitchAdjustedBank tests...\n");
    test_switch_adjusted_bank();

    printf("[*] Running ReloadSavedBank tests...\n");
    test_reload_saved_bank();

    printf("[*] Running CopyObjectsAttributesToWRAM2 tests...\n");
    test_copy_objects_attributes_to_wram2();

    if (failures == 0) {
        printf("  [PASS] All bank.asm functions verified successfully!\n");
    }
    return failures;
}
