#include <stdio.h>
#include <assert.h>
#include "gb.h"
#include "home/bank.h"
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

static void test_restore_bank_and_return(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x0D);
    gb.rom_bank = 0x02;

    RestoreBankAndReturn(&gb);
    TEST_ASSERT(gb.rom_bank == 0x0D, "RestoreBankAndReturn did not reload wCurrentBank");
}

static void test_load_bank1_and_return(void) {
    GBState gb;
    gb_init(&gb);

    gb.rom_bank = 0x18;
    LoadBank1AndReturn(&gb);
    TEST_ASSERT(gb.rom_bank == 0x01, "LoadBank1AndReturn did not switch to bank 1");
}

static void test_restore_stacked_bank_and_return(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x03);
    gb.rom_bank = 0x09;

    RestoreStackedBankAndReturn(&gb, 0x11);
    TEST_ASSERT(gb.rom_bank == 0x11, "RestoreStackedBankAndReturn did not set stacked bank");
    /* wCurrentBank should remain unchanged */
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x03, "wCurrentBank should not be changed");
}

static void test_restore_stacked_bank(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x04);
    gb.rom_bank = 0x09;

    RestoreStackedBank(&gb, 0x15);
    TEST_ASSERT(gb.rom_bank == 0x15, "RestoreStackedBank did not set stacked bank in rom_bank");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x15, "RestoreStackedBank did not update wCurrentBank");
}

static void dummy_farcall_target(GBState *gb) {
    TEST_ASSERT(gb->rom_bank == 0x17, "ROM bank not switched to target during Farcall");
    gb_write(gb, 0xC100, 0x55);
}

static void test_farcall(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wFarcallBank, 0x17);
    gb_write(&gb, wFarcallReturnBank, 0x05);
    gb.rom_bank = 0x02;

    Farcall(&gb, dummy_farcall_target);

    TEST_ASSERT(gb_read(&gb, 0xC100) == 0x55, "Farcall did not execute target function");
    TEST_ASSERT(gb.rom_bank == 0x05, "Farcall did not switch back to wFarcallReturnBank");

    /* Test Farcall_trampoline */
    gb_write(&gb, wFarcallAdressHigh, 0x4E);
    gb_write(&gb, wFarcallAdressLow, 0x2A);
    uint16_t addr = Farcall_trampoline(&gb);
    TEST_ASSERT(addr == 0x4E2A, "Farcall_trampoline did not return correct 16-bit address");
}

static void test_backup_object_in_ram2(void) {
    GBState gb;
    gb_init(&gb);

    /* 1. On DMG (hIsGBC = 0), returns immediately without copying or changing ROM bank */
    gb_write(&gb, hIsGBC, 0);
    gb.wram[1][0x500] = 0x03; /* Overworld object at $D500 */
    gb.wram[2][0x500] = 0x00;
    gb.rom_bank = 0x01;
    BackupObjectInRAM2(&gb, 0xD500, 0x04);
    TEST_ASSERT(gb.wram[2][0x500] == 0x00, "DMG should not backup object to RAM2");
    TEST_ASSERT(gb.rom_bank == 0x01, "DMG should return immediately without restoring bank");
    TEST_ASSERT(gb_read(&gb, hMultiPurpose2) == 0x04, "hMultiPurpose2 not set on DMG");

    /* 2. GBC mode, object IS in ignore list (0x03), bit 7 clear -> copy performed */
    gb_write(&gb, hIsGBC, 1);
    gb.wram[1][0x500] = 0x03;
    gb.wram[2][0x500] = 0x00;
    BackupObjectInRAM2(&gb, 0xD500, 0x05);
    TEST_ASSERT(gb.wram[2][0x500] == 0x03, "GBC in-list object was not backed up to RAM2");
    TEST_ASSERT(gb.rom_bank == 0x05, "ROM bank not set to return_bank 0x05");
    TEST_ASSERT(gb_read(&gb, hMultiPurpose2) == 0x05, "hMultiPurpose2 not set");

    /* 3. GBC mode, object NOT in ignore list (0x42), bit 7 clear -> skipped */
    gb.wram[1][0x501] = 0x42;
    gb.wram[2][0x501] = 0x00;
    BackupObjectInRAM2(&gb, 0xD501, 0x06);
    TEST_ASSERT(gb.wram[2][0x501] == 0x00, "Object not in ignore list should be skipped");
    TEST_ASSERT(gb.rom_bank == 0x06, "ROM bank not restored when skipped");

    /* 4. GBC mode, object NOT in ignore list (0x42), bit 7 set -> forced backup */
    gb.wram[1][0x502] = 0x42;
    gb.wram[2][0x502] = 0x00;
    BackupObjectInRAM2(&gb, 0xD502, 0x87);
    TEST_ASSERT(gb.wram[2][0x502] == 0x42, "Forced backup (bit 7 set) did not backup object");
    TEST_ASSERT(gb.rom_bank == 0x07, "ROM bank not set to return_bank & 0x7F");
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

static int tramp_called = 0;
static void hook_bank20(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == 0x20, "Target callback not executed in bank 0x20");
}

static void hook_generic(GBState *gb) {
    tramp_called++;
    (void)gb;
}

static void test_bank_trampolines(void) {
    GBState gb;

    /* 1. func_020_6A30_trampoline -> executes in 0x20, restores wCurrentBank */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x05);
    gb.rom_bank = 0x05;
    tramp_called = 0;
    func_020_6A30_trampoline(&gb, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "func_020_6A30_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x05, "func_020_6A30_trampoline did not restore wCurrentBank");

    /* 2. func_020_6AC1_trampoline -> executes in 0x20, restores wCurrentBank */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x08);
    gb.rom_bank = 0x08;
    tramp_called = 0;
    func_020_6AC1_trampoline(&gb, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "func_020_6AC1_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x08, "func_020_6AC1_trampoline did not restore wCurrentBank");

    /* 3. UpdateIntroSeaBGPalettes_trampoline -> executes in 0x20, restores wCurrentBank */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x0A);
    gb.rom_bank = 0x0A;
    tramp_called = 0;
    UpdateIntroSeaBGPalettes_trampoline(&gb, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "UpdateIntroSeaBGPalettes_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x0A, "UpdateIntroSeaBGPalettes_trampoline did not restore wCurrentBank");

    /* 4. ClearFileMenuBG_trampoline -> executes in 0x20, restores stacked bank */
    gb_init(&gb);
    tramp_called = 0;
    ClearFileMenuBG_trampoline(&gb, 0x14, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "ClearFileMenuBG_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x14, "ClearFileMenuBG_trampoline did not restore stacked bank (0x14)");

    /* 5. LoadFileMenuBG_trampoline -> executes in 0x20, loads bank 1 */
    gb_init(&gb);
    tramp_called = 0;
    LoadFileMenuBG_trampoline(&gb, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "LoadFileMenuBG_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x01, "LoadFileMenuBG_trampoline did not load bank 1");

    /* 6. CopyLinkTunicPalette_trampoline -> executes in 0x20, loads bank 1 */
    gb_init(&gb);
    tramp_called = 0;
    CopyLinkTunicPalette_trampoline(&gb, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "CopyLinkTunicPalette_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x01, "CopyLinkTunicPalette_trampoline did not load bank 1");

    /* 7. LoadBaseTiles_trampoline -> executes callback, restores stacked bank */
    gb_init(&gb);
    tramp_called = 0;
    LoadBaseTiles_trampoline(&gb, 0x25, hook_generic);
    TEST_ASSERT(tramp_called == 1, "LoadBaseTiles_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x25, "LoadBaseTiles_trampoline did not restore stacked bank (0x25)");
}

static void hook_bank24(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == 0x24, "Target callback not executed in bank 0x24");
}

static void hook_bank0f(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == BANK_FontTiles, "Target callback not executed in BANK(FontTiles)");
    TEST_ASSERT(gb_read(gb, wCurrentBank) == BANK_FontTiles, "wCurrentBank not set to BANK(FontTiles)");
}

static void hook_bank36_switch(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == 0x36, "Target callback not executed in bank 0x36");
    TEST_ASSERT(gb_read(gb, wCurrentBank) == 0x36, "wCurrentBank not set to 0x36");
}

static void hook_bank36_select(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == 0x36, "Target callback not executed in bank 0x36");
}

static void hook_bank3d(GBState *gb) {
    tramp_called++;
    TEST_ASSERT(gb->rom_bank == 0x3D, "Target callback not executed in bank 0x3D");
}

static void test_bank_trampolines_batch2(void) {
    GBState gb;

    /* 1. ChangeBGColumnPaletteAndExecuteDrawCommands */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x02);
    /* Setup draw command at wDrawCommand (0xD601) */
    gb_write(&gb, wDrawCommand + 0, 0x98);
    gb_write(&gb, wDrawCommand + 1, 0x20);
    gb_write(&gb, wDrawCommand + 2, 0x01); /* copy row, count = 2 (0x01 + 1) */
    gb_write(&gb, wDrawCommand + 3, 0x44);
    gb_write(&gb, wDrawCommand + 4, 0x55);
    gb_write(&gb, wDrawCommand + 5, 0x00); /* end */
    tramp_called = 0;
    ChangeBGColumnPaletteAndExecuteDrawCommands(&gb, 0x12, hook_bank24);
    TEST_ASSERT(tramp_called == 1, "ChangeBGColumnPaletteAndExecuteDrawCommands callback not called");
    TEST_ASSERT(gb_read(&gb, 0x9820) == 0x44, "Draw commands not executed (byte 0)");
    TEST_ASSERT(gb_read(&gb, 0x9821) == 0x55, "Draw commands not executed (byte 1)");
    TEST_ASSERT(gb.rom_bank == 0x12, "ROM bank not restored to stacked bank (0x12)");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x12, "wCurrentBank not restored to stacked bank (0x12)");

    /* 2. func_A9B */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x01);
    tramp_called = 0;
    func_A9B(&gb, 0x07, hook_bank0f);
    TEST_ASSERT(tramp_called == 1, "func_A9B callback not called");
    TEST_ASSERT(gb.rom_bank == 0x07, "ROM bank not restored to stacked bank (0x07)");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == BANK_FontTiles, "wCurrentBank should remain at BANK(FontTiles)");

    /* 3. Spawn2x2RubbleEntities_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x01);
    tramp_called = 0;
    Spawn2x2RubbleEntities_trampoline(&gb, 0x04, hook_bank36_switch);
    TEST_ASSERT(tramp_called == 1, "Spawn2x2RubbleEntities_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x04, "ROM bank not restored to stacked bank (0x04)");
    TEST_ASSERT(gb_read(&gb, wCurrentBank) == 0x04, "wCurrentBank not restored to stacked bank (0x04)");

    /* 4. func_A5F */
    gb_init(&gb);
    tramp_called = 0;
    func_A5F(&gb, 0x08, hook_bank20);
    TEST_ASSERT(tramp_called == 1, "func_A5F callback not called");
    TEST_ASSERT(gb.rom_bank == 0x08, "func_A5F did not restore stacked bank (0x08)");

    /* 5. func_036_703E_trampoline */
    gb_init(&gb);
    tramp_called = 0;
    func_036_703E_trampoline(&gb, 0x09, hook_bank36_select);
    TEST_ASSERT(tramp_called == 1, "func_036_703E_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x09, "func_036_703E_trampoline did not restore stacked bank (0x09)");

    /* 6. cycleInstrumentItemColor_trampoline */
    gb_init(&gb);
    tramp_called = 0;
    cycleInstrumentItemColor_trampoline(&gb, 0x0A, hook_bank36_select);
    TEST_ASSERT(tramp_called == 1, "cycleInstrumentItemColor_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x0A, "cycleInstrumentItemColor_trampoline did not restore stacked bank (0x0A)");

    /* 7. func_036_4A77_trampoline */
    gb_init(&gb);
    gb_write(&gb, wCurrentBank, 0x02);
    tramp_called = 0;
    func_036_4A77_trampoline(&gb, 0x0B, hook_bank36_switch);
    TEST_ASSERT(tramp_called == 1, "func_036_4A77_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x0B, "func_036_4A77_trampoline did not restore stacked bank (0x0B)");

    /* 8. GetOwlStatueDialogId_trampoline */
    gb_init(&gb);
    tramp_called = 0;
    GetOwlStatueDialogId_trampoline(&gb, 0x0C, hook_bank36_select);
    TEST_ASSERT(tramp_called == 1, "GetOwlStatueDialogId_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x0C, "GetOwlStatueDialogId_trampoline did not restore stacked bank (0x0C)");

    /* 9. SpawnPhotographer_trampoline */
    gb_init(&gb);
    tramp_called = 0;
    SpawnPhotographer_trampoline(&gb, 0x0D, hook_bank36_select);
    TEST_ASSERT(tramp_called == 1, "SpawnPhotographer_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x0D, "SpawnPhotographer_trampoline did not restore stacked bank (0x0D)");

    /* 10. LoadPhotoBgMap_trampoline */
    gb_init(&gb);
    tramp_called = 0;
    LoadPhotoBgMap_trampoline(&gb, hook_bank3d);
    TEST_ASSERT(tramp_called == 1, "LoadPhotoBgMap_trampoline callback not called");
    TEST_ASSERT(gb.rom_bank == 0x3D, "LoadPhotoBgMap_trampoline ROM bank should be 0x3D");
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

    printf("[*] Running RestoreBankAndReturn tests...\n");
    test_restore_bank_and_return();

    printf("[*] Running LoadBank1AndReturn tests...\n");
    test_load_bank1_and_return();

    printf("[*] Running RestoreStackedBankAndReturn tests...\n");
    test_restore_stacked_bank_and_return();

    printf("[*] Running RestoreStackedBank tests...\n");
    test_restore_stacked_bank();

    printf("[*] Running Farcall tests...\n");
    test_farcall();

    printf("[*] Running BackupObjectInRAM2 tests...\n");
    test_backup_object_in_ram2();

    printf("[*] Running CopyObjectsAttributesToWRAM2 tests...\n");
    test_copy_objects_attributes_to_wram2();

    printf("[*] Running Bank Trampolines tests...\n");
    test_bank_trampolines();
    test_bank_trampolines_batch2();

    if (failures == 0) {
        printf("  [PASS] All bank.asm functions verified successfully!\n");
        return 0;
    } else {
        printf("  [FAIL] %d test failures in bank.asm tests.\n", failures);
        return 1;
    }
}
