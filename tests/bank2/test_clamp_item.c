#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/items.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdint.h>

void test_bank2_clamp_item_count(void) {
    /* Test: ClampItemCount (02:60D8-02:60DF) */

    /* 1. Current count less than max - no change */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x20);  /* max = 32 */
        gb_write(&gb, wMagicPowderCount, 0x10); /* current = 16 */

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0x10);
        assert(gb_read(&gb, wMaxMagicPowder) == 0x20);
    }

    /* 2. Current count equal to max - no change (but clamp logic runs) */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x20);
        gb_write(&gb, wMagicPowderCount, 0x20);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0x20);
    }

    /* 3. Current count greater than max - clamped to max */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x20);
        gb_write(&gb, wMagicPowderCount, 0x30);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0x20);
    }

    /* 4. Zero max - clamps to zero */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x00);
        gb_write(&gb, wMagicPowderCount, 0x10);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0x00);
    }

    /* 5. Max 0xFF, current 0xFE - no change */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0xFF);
        gb_write(&gb, wMagicPowderCount, 0xFE);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0xFE);
    }

    /* 6. Max 0xFF, current 0xFF - no change */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0xFF);
        gb_write(&gb, wMagicPowderCount, 0xFF);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);

        assert(gb_read(&gb, wMagicPowderCount) == 0xFF);
    }

    /* 7. NULL state - no-op */
    {
        ClampItemCount(NULL, wMaxMagicPowder, wMagicPowderCount);
        /* Should not crash */
    }

    /* 8. Test with bomb count address */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x3C); /* 60 */
        gb_write(&gb, wBombCount, 0x50);      /* 80 */

        ClampItemCount(&gb, wMaxMagicPowder, wBombCount);

        assert(gb_read(&gb, wBombCount) == 0x3C);
    }

    /* 9. Test with arrow count address */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x46); /* 70 */
        gb_write(&gb, wArrowCount, 0x46);     /* 70 */

        ClampItemCount(&gb, wMaxMagicPowder, wArrowCount);

        assert(gb_read(&gb, wArrowCount) == 0x46);
    }

    /* 10. Repeated calls with same addresses */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x20);
        gb_write(&gb, wMagicPowderCount, 0x30);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);
        assert(gb_read(&gb, wMagicPowderCount) == 0x20);

        ClampItemCount(&gb, wMaxMagicPowder, wMagicPowderCount);
        assert(gb_read(&gb, wMagicPowderCount) == 0x20);
    }
}