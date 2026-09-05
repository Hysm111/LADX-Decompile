#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/vfx.h"
#include "constants/memory.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_add_transcient_vfx(void) {
    GBState gb;
    gb_init(&gb);

    /* 1. All slots free: should pick slot 15 ($0F) */
    gb_write(&gb, hMultiPurpose0, 0x38); /* X pos */
    gb_write(&gb, hMultiPurpose1, 0x42); /* Y pos */
    uint8_t slot1 = AddTranscientVfx(&gb, 0x05);

    TEST_ASSERT(slot1 == 0x0F, "Slot 15 not chosen when all slots free");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxTypeTable + 0x0F) == 0x05, "VFX type not set in slot 15");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosXTable + 0x0F) == 0x38, "VFX X not set in slot 15");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosYTable + 0x0F) == 0x42, "VFX Y not set in slot 15");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxCountdownTable + 0x0F) == 0x0F, "VFX countdown not set to $0F");

    /* 2. Slot 15 is busy: next call should pick slot 14 ($0E) */
    gb_write(&gb, hMultiPurpose0, 0x10);
    gb_write(&gb, hMultiPurpose1, 0x20);
    uint8_t slot2 = AddTranscientVfx(&gb, 0x09);

    TEST_ASSERT(slot2 == 0x0E, "Slot 14 not chosen when slot 15 busy");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxTypeTable + 0x0E) == 0x09, "VFX type not set in slot 14");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosXTable + 0x0E) == 0x10, "VFX X not set in slot 14");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosYTable + 0x0E) == 0x20, "VFX Y not set in slot 14");

    /* 3. Fill all slots 0 to 15 */
    for (int i = 0; i < 16; i++) {
        gb_write(&gb, wTranscientVfxTypeTable + i, 0x01);
    }
    /* Set wC5C0 = 4 */
    gb_write(&gb, wC5C0, 4);

    /* Should decrement wC5C0 to 3, and recycle slot 3 */
    gb_write(&gb, hMultiPurpose0, 0x99);
    gb_write(&gb, hMultiPurpose1, 0x88);
    uint8_t slot3 = AddTranscientVfx(&gb, 0x07);

    TEST_ASSERT(slot3 == 3, "Recycled slot should be 3");
    TEST_ASSERT(gb_read(&gb, wC5C0) == 3, "wC5C0 not decremented to 3");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxTypeTable + 3) == 0x07, "Recycled slot type mismatch");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosXTable + 3) == 0x99, "Recycled slot X mismatch");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxPosYTable + 3) == 0x88, "Recycled slot Y mismatch");

    /* 4. Test wrap-around of wC5C0 when it reaches 0 */
    gb_write(&gb, wC5C0, 0);
    uint8_t slot4 = AddTranscientVfx(&gb, 0x0A);

    TEST_ASSERT(slot4 == 0x0F, "wC5C0 did not wrap to 0x0F");
    TEST_ASSERT(gb_read(&gb, wC5C0) == 0x0F, "wC5C0 not stored as 0x0F");
    TEST_ASSERT(gb_read(&gb, wTranscientVfxTypeTable + 0x0F) == 0x0A, "Wrapped slot type mismatch");
}

void run_vfx_tests(void) {
    printf("[*] Running Transient VFX tests...\n");
    test_add_transcient_vfx();

    if (failures == 0) {
        printf("  [PASS] All vfx.asm functions verified successfully!\n\n");
    }
}
