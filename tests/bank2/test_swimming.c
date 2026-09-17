#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/falling.h"
#include "bank2/swimming.h"
#include "bank2/vfx.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_swimming(void) {
    /* Test 46: Data_002_4EF0-4F20 Swimming Velocity Tables */
    {
        assert(sizeof(Data_002_4EF0) == 16);
        assert(sizeof(Data_002_4F00) == 16);
        assert(sizeof(Data_002_4F10) == 16);
        assert(sizeof(Data_002_4F20) == 16);

        assert(Data_002_4EF0[1] == 0x08);
        assert(Data_002_4EF0[2] == -0x08);
        assert(Data_002_4F00[1] == 0x10);
        assert(Data_002_4F00[2] == -0x10);
        assert(Data_002_4F10[4] == -0x08);
        assert(Data_002_4F10[8] == 0x08);
        assert(Data_002_4F20[4] == -0x10);
        assert(Data_002_4F20[8] == 0x10);
    }

    /* Test 47: func_002_5928 Water Splash VFX */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionX, 0x44);

        func_002_5928(&gb, 0x30);

        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x30);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x44);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_WATER_SPLASH);
    }

    /* Test 48: LinkMotionSwimmingHandler Movement and Sunken Heart Piece */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Free movement mode resets state to default */
        gb_write(&gb, wFreeMovementMode, 1);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        LinkMotionSwimmingHandler(&gb, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_DEFAULT);

        /* 2. Dialog open or interactive motion blocked -> clears position increment */
        gb_init(&gb);
        gb_write(&gb, wFreeMovementMode, 0);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 1);
        LinkMotionSwimmingHandler(&gb, NULL, NULL, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        /* 3. B button toggle: start diving */
        gb_init(&gb);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        gb_write_hram(&gb, hJoypadState, J_B);
        gb_write_hram(&gb, hLinkPhysicsModifier, 0);
        gb_write_hram(&gb, hLinkPositionY, 0x20);
        LinkMotionSwimmingHandler(&gb, NULL, NULL, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkPhysicsModifier) == 1);
        assert(gb_read_hram(&gb, hLinkCountdown) == 0xA0);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_WATER_SPLASH);

        /* 4. A button stroke: triggers JINGLE_SWIM and sets wC183 = 0x20 */
        gb_init(&gb);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        gb_write_hram(&gb, hJoypadState, J_A);
        gb_write(&gb, wC183, 0);
        LinkMotionSwimmingHandler(&gb, NULL, NULL, NULL, NULL);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_SWIM);
        assert(gb_read(&gb, wC183) == 0x20);

        /* 5. Diving over sunken piece of heart in Overworld (ROOM_OW_KANALET_MOAT_HEARTPIECE) */
        gb_init(&gb);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        gb_write_hram(&gb, hLinkPhysicsModifier, 1);
        gb_write_hram(&gb, hLinkCountdown, 0x50);
        gb_write_hram(&gb, hMapId, 0);
        gb_write_hram(&gb, hMapRoom, ROOM_OW_KANALET_MOAT_HEARTPIECE);
        gb_write_hram(&gb, hLinkPositionY, 0x54); /* within [0x48, 0x57] */
        gb_write_hram(&gb, hLinkPositionX, 0x5C); /* within [0x50, 0x5F] */
        gb_write_hram(&gb, hRoomStatus, 0);

        LinkMotionSwimmingHandler(&gb, NULL, NULL, mock_spawn_entity, NULL);

        assert((gb_read_hram(&gb, hRoomStatus) & (1 << 5)) != 0);
        assert(gb_read(&gb, (uint16_t)(wEntitiesTypeTable + 15)) == ENTITY_HEART_PIECE);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosXTable + 15)) == 0x5C);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosYTable + 15)) == 0x54);
        assert(gb_read(&gb, (uint16_t)(wEntitiesPosZTable + 15)) == 0x03);

        /* 6. Diving in flooded grotto tunnel (MAP_CAVE_WATER, UNKNOWN_ROOM_8D) -> noise fade-out warp */
        gb_init(&gb);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_SWIMMING);
        gb_write_hram(&gb, hLinkPhysicsModifier, 1);
        gb_write_hram(&gb, hLinkCountdown, 0x50);
        gb_write_hram(&gb, hMapId, MAP_CAVE_WATER);
        gb_write_hram(&gb, hMapRoom, UNKNOWN_ROOM_8D);
        gb_write_hram(&gb, hLinkPositionY, 0x54);
        gb_write_hram(&gb, hLinkPositionX, 0x5C);

        LinkMotionSwimmingHandler(&gb, NULL, NULL, NULL, NULL);
        assert(gb_read(&gb, wWarp0DestinationX) == 0x5C);
        assert(gb_read(&gb, wD463) == LINK_MOTION_SWIMMING);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);
    }

    /* Test 49: LinkMotionUnknownHandler State 0x0F */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Low height, px != 0xE0 -> does not trigger fade out */
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        gb_write_hram(&gb, hLinkPositionZ, 0x20);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 0);

        LinkMotionUnknownHandler(&gb);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(gb_read(&gb, wLinkMotionState) != LINK_MOTION_MAP_FADE_OUT);

        /* 2. Height >= 0x78 -> triggers fade out and resets Z */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        gb_write_hram(&gb, hLinkPositionZ, 0x78);
        gb_write_hram(&gb, hLinkVelocityZ, 0x10);

        LinkMotionUnknownHandler(&gb);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT);
        assert(gb_read_hram(&gb, hLinkPositionZ) == 0);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0);
        assert(gb_read(&gb, wMapEntrancePositionZ) == 0x70);
    }

}
