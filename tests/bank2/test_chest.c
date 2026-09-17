#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/room_events.h"
#include "constants/entities.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_chest(void) {
    /* Test 9: SpawnChestWithItem - Default Slot Allocation */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hIntersectedObjectLeft, 0x44);
        gb_write_hram(&gb, hIntersectedObjectTop, 0x62);
        gb_write_hram(&gb, hMultiPurpose8, 0x2A);

        /* All slots disabled except slot 15 */
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), 0);
        }

        bool ok = SpawnChestWithItem(&gb, NULL);
        assert(ok);
        /* Slot 15 should have been chosen (scanned 15 down to 0) */
        assert(gb_read(&gb, (uint16_t)(wEntitiesStatusTable + 15)) == ENTITY_STATUS_INIT);
        assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 15)) == ENTITY_CHEST_WITH_ITEM);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 15)) == 0x48);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 15)) == 0x70);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 15)) == 0x2A);
    }

    /* Test 10: SpawnChestWithItem - Entity Table Full */
    {
        GBState gb;
        gb_init(&gb);
        for (int i = 0; i < MAX_ENTITIES; i++) {
            gb_write(&gb, (uint16_t)(wEntitiesStatusTable + i), ENTITY_STATUS_ACTIVE);
        }
        bool ok = SpawnChestWithItem(&gb, NULL);
        assert(!ok);
    }

    /* Test 11: SpawnChestWithItem - Custom Callback */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hIntersectedObjectLeft, 0x20);
        gb_write_hram(&gb, hIntersectedObjectTop, 0x30);
        gb_write_hram(&gb, hMultiPurpose8, 0x05);

        bool ok = SpawnChestWithItem(&gb, mock_spawn_entity_slot3);
        assert(ok);
        assert(gb_read(&gb, (uint16_t)(wEntitiesStatusTable + 3)) == ENTITY_STATUS_ACTIVE - 1);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 3)) == 0x28);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 3)) == 0x40);
        assert(gb_read(&gb, (uint16_t)(wEntitiesSpriteVariantTable + 3)) == 0x05);

        /* Callback failure */
        ok = SpawnChestWithItem(&gb, mock_spawn_entity_fail);
        assert(!ok);
    }

}
