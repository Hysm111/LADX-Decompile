#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/room.h"
#include "constants/memory.h"
#include "constants/sfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_mark_trigger_as_resolved(void) {
    GBState gb;

    /* Case 1: wRoomEventEffectExecuted is already non-zero -> should do nothing */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 1);
    gb_write(&gb, wC1CF, 0x55);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wC1CF) == 0x55, "wC1CF modified when event already executed");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 0x00, "wC5A6 modified when event already executed");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_NONE, "hJingle modified when event already executed");

    /* Case 2: wRoomEventEffectExecuted is 0 and wC19D == 0 -> should resolve and play jingle */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0x77);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC1CF) == 0, "wC1CF not cleared to 0");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_PUZZLE_SOLVED, "hJingle not set to JINGLE_PUZZLE_SOLVED");

    /* Case 3: wRoomEventEffectExecuted is 0 but wC19D != 0 -> should resolve without playing jingle */
    gb_init(&gb);
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0x88);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x01);
    gb_write(&gb, hJingle, JINGLE_NONE);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC1CF) == 0, "wC1CF not cleared to 0");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_NONE, "hJingle triggered despite wC19D != 0");
}

void run_room_tests(void) {
    printf("[*] Running MarkTriggerAsResolved tests...\n");
    test_mark_trigger_as_resolved();

    if (failures == 0) {
        printf("  [PASS] All room.asm functions verified successfully!\n\n");
    }
}
