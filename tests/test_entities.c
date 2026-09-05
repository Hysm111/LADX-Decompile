#include "home/entities.h"
#include "constants/memory.h"
#include <stdio.h>
#include <assert.h>

void test_is_zero(void) {
    printf("[*] Running IsZero tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, 0xC500, 0x00);
    gb_write(&gb, 0xC505, 0x42);

    assert(IsZero(&gb, 0xC500, 0) == 0x00);
    assert(IsZero(&gb, 0xC500, 5) == 0x42);
}

void test_entity_countdowns(void) {
    printf("[*] Running Entity Countdown tests...\n");

    GBState gb;
    gb_init(&gb);

    /* Test GetEntitySlowTransitionCountdown */
    gb_write(&gb, wEntitiesSlowTransitionCountdownTable + 3, 0x12);
    gb_write(&gb, wEntitiesSlowTransitionCountdownTable + 7, 0x00);
    assert(GetEntitySlowTransitionCountdown(&gb, 3) == 0x12);
    assert(GetEntitySlowTransitionCountdown(&gb, 7) == 0x00);

    /* Test GetEntityPrivateCountdown1 */
    gb_write(&gb, wEntitiesPrivateCountdown1Table + 2, 0x34);
    gb_write(&gb, wEntitiesPrivateCountdown1Table + 5, 0x00);
    assert(GetEntityPrivateCountdown1(&gb, 2) == 0x34);
    assert(GetEntityPrivateCountdown1(&gb, 5) == 0x00);

    /* Test GetEntityTransitionCountdown */
    gb_write(&gb, wEntitiesTransitionCountdownTable + 0, 0x56);
    gb_write(&gb, wEntitiesTransitionCountdownTable + 9, 0x00);
    assert(GetEntityTransitionCountdown(&gb, 0) == 0x56);
    assert(GetEntityTransitionCountdown(&gb, 9) == 0x00);

    /* Test DecrementEntityIgnoreHitsCountdown */
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + 4, 5);
    DecrementEntityIgnoreHitsCountdown(&gb, 4);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 4) == 4);

    /* Test that 0 does not underflow */
    gb_write(&gb, wEntitiesIgnoreHitsCountdownTable + 8, 0);
    DecrementEntityIgnoreHitsCountdown(&gb, 8);
    assert(gb_read(&gb, wEntitiesIgnoreHitsCountdownTable + 8) == 0);
}

void run_entities_tests(void) {
    test_is_zero();
    test_entity_countdowns();
    printf("  [PASS] All entities.asm functions verified successfully!\n\n");
}
