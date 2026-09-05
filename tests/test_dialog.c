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

void run_dialog_tests(void) {
    printf("[*] Running Dialog lookup tests...\n");
    test_dialog_lookups();

    if (failures == 0) {
        printf("  [PASS] All dialog.asm functions verified successfully!\n\n");
    }
}
