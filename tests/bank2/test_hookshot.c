#include "test_bank2.h"

#include "gb.h"
#include "bank2/items.h"
#include "constants/directions.h"
#include "constants/entities.h"
#include "constants/hardware.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

void test_bank2_hookshot(void) {
    /* Test 14: FireHookshot - Blocked when In Air */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wIsLinkInTheAir, 1);
        bool ok = FireHookshot(&gb);
        assert(!ok);
    }

    /* Test 15: FireHookshot - Successful Fire in All 4 Directions */
    {
        const uint8_t dirs[4] = { DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_UP, DIRECTION_DOWN };
        const int8_t exp_vx[4] = { 0x30, -0x30, 0x00, 0x00 };
        const int8_t exp_vy[4] = { 0x00, 0x00, -0x30, 0x30 };

        for (int d = 0; d < 4; d++) {
            GBState gb;
            gb_init(&gb);
            gb_write_hram(&gb, hLinkDirection, dirs[d]);

            bool ok = FireHookshot(&gb);
            assert(ok);

            /* Slot 0 was free, so projectile should be in slot 0 */
            assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 0)) == ENTITY_HOOKSHOT_CHAIN);
            assert(gb_read(&gb, (uint16_t)(wEntitiesTransitionCountdownTable + 0)) == 0x2A);
            assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 0)) == 0x00);
            assert((int8_t)gb_read(&gb, (uint16_t)(wEntitiesSpeedXTable + 0)) == exp_vx[d]);
            assert((int8_t)gb_read(&gb, (uint16_t)(wEntitiesSpeedYTable + 0)) == exp_vy[d]);
        }
    }

    /* Test 16: FireHookshot - Entity Table Full */
    {
        GBState gb;
        gb_init(&gb);
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), ENTITY_STATUS_ACTIVE);
        }
        bool ok = FireHookshot(&gb);
        assert(!ok);
    }

}
