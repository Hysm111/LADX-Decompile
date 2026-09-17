#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/shovel.h"
#include "constants/directions.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_shovel(void) {
    /* Test 38: LinkDirectionToLinkAnimationState2 and Adjacent Tile Tables */
    {
        assert(sizeof(LinkDirectionToLinkAnimationState2) == 8);
        assert(sizeof(LinkDirectionToAdjacentTileIndexX) == 4);
        assert(sizeof(LinkDirectionToAdjacentTileIndexY) == 4);

        assert(LinkDirectionToLinkAnimationState2[0] == 0x71);
        assert(LinkDirectionToLinkAnimationState2[1] == 0x72);
        assert(LinkDirectionToLinkAnimationState2[2] == 0x6F);
        assert(LinkDirectionToLinkAnimationState2[3] == 0x70);
        assert(LinkDirectionToLinkAnimationState2[4] == 0x73);
        assert(LinkDirectionToLinkAnimationState2[5] == 0x74);
        assert(LinkDirectionToLinkAnimationState2[6] == 0x6D);
        assert(LinkDirectionToLinkAnimationState2[7] == 0x6E);

        assert(LinkDirectionToAdjacentTileIndexX[0] == 0x14);
        assert(LinkDirectionToAdjacentTileIndexX[1] == (int8_t)0xFC);
        assert(LinkDirectionToAdjacentTileIndexX[2] == 0x08);
        assert(LinkDirectionToAdjacentTileIndexX[3] == 0x08);

        assert(LinkDirectionToAdjacentTileIndexY[0] == 0x0A);
        assert(LinkDirectionToAdjacentTileIndexY[1] == 0x0A);
        assert(LinkDirectionToAdjacentTileIndexY[2] == (int8_t)0xFC);
        assert(LinkDirectionToAdjacentTileIndexY[3] == 0x14);
    }

    /* Test 39: func_002_4BD4 and func_002_4C14 Shovel Draw Commands */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. func_002_4BD4 Outdoor */
        gb_write_hram(&gb, hIntersectedObjectBGAddressHigh, 0x98);
        gb_write_hram(&gb, hIntersectedObjectBGAddressLow, 0x40);
        gb_write(&gb, wIsIndoor, 0);

        func_002_4BD4(&gb, 0xD601);
        assert(gb_read(&gb, 0xD601) == 0x98);
        assert(gb_read(&gb, 0xD602) == 0x40);
        assert(gb_read(&gb, 0xD603) == 0x81);
        assert(gb_read(&gb, 0xD604) == 0x6A);
        assert(gb_read(&gb, 0xD605) == 0x7A);
        assert(gb_read(&gb, 0xD606) == 0x98);
        assert(gb_read(&gb, 0xD607) == 0x41);
        assert(gb_read(&gb, 0xD608) == 0x81);
        assert(gb_read(&gb, 0xD609) == 0x6B);
        assert(gb_read(&gb, 0xD60A) == 0x7B);
        assert(gb_read(&gb, 0xD60B) == 0x00);

        /* 2. func_002_4BD4 Indoor */
        gb_write(&gb, wIsIndoor, 1);
        func_002_4BD4(&gb, 0xD601);
        assert(gb_read(&gb, 0xD604) == 0x04);
        assert(gb_read(&gb, 0xD605) == 0x06);
        assert(gb_read(&gb, 0xD609) == 0x05);
        assert(gb_read(&gb, 0xD60A) == 0x07);

        /* 3. func_002_4C14 Outdoor on GBC */
        gb_init(&gb);
        gb_write_hram(&gb, hIntersectedObjectBGAddressHigh, 0x99);
        gb_write_hram(&gb, hIntersectedObjectBGAddressLow, 0x20);
        gb_write(&gb, wIsIndoor, 0);
        gb_write(&gb, wDrawCommandsVRAM1Size, 0);

        func_002_4C14(&gb, 0xD601);
        assert(gb_read(&gb, wDrawCommandsVRAM1Size) == 10);
        /* VRAM0 draw command */
        assert(gb_read(&gb, 0xD601) == 0x99);
        assert(gb_read(&gb, 0xD602) == 0x20);
        assert(gb_read(&gb, 0xD603) == 0x81);
        assert(gb_read(&gb, 0xD604) == 0x6A);
        assert(gb_read(&gb, 0xD605) == 0x7A);
        assert(gb_read(&gb, 0xD606) == 0x99);
        assert(gb_read(&gb, 0xD607) == 0x21);
        assert(gb_read(&gb, 0xD608) == 0x81);
        assert(gb_read(&gb, 0xD609) == 0x6B);
        assert(gb_read(&gb, 0xD60A) == 0x7B);
        assert(gb_read(&gb, 0xD60B) == 0x00);
        /* VRAM1 draw command */
        uint16_t vram1_base = wDrawCommandVRAM1;
        assert(gb_read(&gb, vram1_base + 0) == 0x99);
        assert(gb_read(&gb, vram1_base + 1) == 0x20);
        assert(gb_read(&gb, vram1_base + 2) == 0x81);
        assert(gb_read(&gb, vram1_base + 3) == 0x03);
        assert(gb_read(&gb, vram1_base + 4) == 0x03);
        assert(gb_read(&gb, vram1_base + 5) == 0x99);
        assert(gb_read(&gb, vram1_base + 6) == 0x21);
        assert(gb_read(&gb, vram1_base + 7) == 0x81);
        assert(gb_read(&gb, vram1_base + 8) == 0x03);
        assert(gb_read(&gb, vram1_base + 9) == 0x03);
        assert(gb_read(&gb, vram1_base + 10) == 0x00);
    }

    /* Test 40: func_002_4D20 Tile Diggability Check */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Carrying lifted object blocks digging */
        gb_write(&gb, wIsCarryingLiftedObject, 1);
        assert(!func_002_4D20(&gb));
        gb_write(&gb, wIsCarryingLiftedObject, 0);

        /* 2. In air blocks digging */
        gb_write_hram(&gb, hLinkPositionZ, 4);
        assert(!func_002_4D20(&gb));
        gb_write_hram(&gb, hLinkPositionZ, 0);

        /* 3. Link motion blocks digging */
        gb_write(&gb, wLinkMotionState, 1);
        assert(!func_002_4D20(&gb));
        gb_write(&gb, wLinkMotionState, 0);

        /* 4. Side scrolling blocks digging */
        gb_write_hram(&gb, hIsSideScrolling, 1);
        assert(!func_002_4D20(&gb));
        gb_write_hram(&gb, hIsSideScrolling, 0);

        /* 5. Outdoor digging: non-diggable object types (0x0C, 0x0D, 0xB9) */
        gb_write_hram(&gb, hLinkDirection, DIRECTION_DOWN);
        gb_write_hram(&gb, hLinkPositionX, 0x48);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write(&gb, wIsIndoor, 0);

        /* tile_idx = (Y + 0x14 - 16) & F0 | ((X + 8 - 8) & F0 >> 4) */
        /* Y = (0x40 + 0x14 - 0x10) & 0xF0 = 0x40; X = (0x48 + 0x08 - 0x08) & 0xF0 = 0x40; c = 4 */
        /* tile_idx = 0x44; room_obj = wRoomObjects + 0x44 */
        uint16_t room_obj_addr = (uint16_t)(wRoomObjects + 0x44);

        /* Object 0x0C (non-diggable) */
        gb_write(&gb, room_obj_addr, 0x0C);
        assert(!func_002_4D20(&gb));

        /* Object 0x0D (non-diggable) */
        gb_write(&gb, room_obj_addr, 0x0D);
        assert(!func_002_4D20(&gb));

        /* Object 0xB9 (non-diggable) */
        gb_write(&gb, room_obj_addr, 0xB9);
        assert(!func_002_4D20(&gb));

        /* Normal diggable grass/dirt object 0x04 */
        gb_write(&gb, room_obj_addr, 0x04);
        assert(func_002_4D20(&gb));
        assert(gb_read_hram(&gb, hIntersectedObjectTop) == 0x40);
        assert(gb_read_hram(&gb, hIntersectedObjectLeft) == 0x40);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x44);

        /* 6. Indoor digging: only object 0x05 is diggable */
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, room_obj_addr, 0x04);
        assert(!func_002_4D20(&gb));

        gb_write(&gb, room_obj_addr, 0x05);
        assert(func_002_4D20(&gb));
    }

    /* Test 41: label_002_4C92, func_002_4BC8, and func_002_4B49 */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. func_002_4BC8 calls func_002_4D20, sets wLinkUsingShovel = 2, creates hole */
        gb_write_hram(&gb, hLinkDirection, DIRECTION_DOWN);
        gb_write_hram(&gb, hLinkPositionX, 0x48);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write(&gb, wIsIndoor, 0);
        uint16_t room_obj_addr = (uint16_t)(wRoomObjects + 0x44);
        gb_write(&gb, room_obj_addr, 0x04);
        gb_write(&gb, wDrawCommandsSize, 0);

        bool dug = func_002_4BC8(&gb, mock_spawn_entity, mock_apply_vector);
        assert(dug);
        assert(gb_read(&gb, wLinkUsingShovel) == 0x02);
        assert(gb_read(&gb, room_obj_addr) == OBJECT_SHOVEL_HOLE);
        assert(gb_read(&gb, wDrawCommandsSize) == 10);

        /* 2. func_002_4B49: not using shovel -> returns early */
        gb_init(&gb);
        gb_write(&gb, wLinkUsingShovel, 0);
        func_002_4B49(&gb, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 0);

        /* 3. func_002_4B49: Wind Fish Egg Nightmare interaction */
        gb_init(&gb);
        gb_write(&gb, wLinkUsingShovel, 1);
        gb_write_hram(&gb, hMapId, MAP_WINDFISHS_EGG);
        gb_write(&gb, wFinalNightmareForm, 0x02);
        gb_write(&gb, wSwordCollisionEnabled, 0);
        g_mock_20_4b4a_calls = 0;

        func_002_4B49(&gb, mock_20_4b4a, NULL);
        assert(g_mock_20_4b4a_calls == 1);
        assert(gb_read(&gb, wSwordCollisionEnabled) == 1);

        /* 4. func_002_4B49: Timer progression and shovel digging trigger at 0x10 */
        gb_init(&gb);
        gb_write(&gb, wLinkUsingShovel, 1);
        gb_write(&gb, wLinkUsingShovelTimer, 0x0F);
        gb_write(&gb, wSwordAnimationState, 5);
        gb_write(&gb, wIsUsingSpinAttack, 3);
        gb_write(&gb, wSwordCharge, 10);
        g_mock_dig_action_calls = 0;

        func_002_4B49(&gb, NULL, mock_dig_action);
        assert(gb_read(&gb, wLinkUsingShovelTimer) == 0x10);
        assert(g_mock_dig_action_calls == 1);
        assert(gb_read(&gb, wSwordAnimationState) == 0);
        assert(gb_read(&gb, wIsUsingSpinAttack) == 0);
        assert(gb_read(&gb, wSwordCharge) == 0);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);

        /* 5. func_002_4B49: Timer 0x17 -> 0x18 finishes shovel use and triggers Dialog279 if Marin is following */
        gb_init(&gb);
        gb_write(&gb, wLinkUsingShovel, 2);
        gb_write(&gb, wLinkUsingShovelTimer, 0x17);
        gb_write(&gb, wIsMarinFollowingLink, 1);
        gb_write(&gb, wDialogState, 0);
        gb_write(&gb, wC1AC, 5);

        func_002_4B49(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkUsingShovelTimer) == 0x18);
        assert(gb_read(&gb, wLinkUsingShovel) == 0);
        assert(gb_read(&gb, wC1AC) == 0);
        assert(gb_read(&gb, wDialogIndex) == 0x79);
        assert(gb_read(&gb, wDialogIndexHi) == 0x02);

        /* 6. func_002_4B49: Animation frames mapping */
        gb_init(&gb);
        gb_write(&gb, wLinkUsingShovel, 1);
        gb_write(&gb, wLinkUsingShovelTimer, 0x00);
        gb_write_hram(&gb, hLinkDirection, DIRECTION_RIGHT);
        func_002_4B49(&gb, NULL, NULL);
        /* timer = 1 -> (timer >> 4) & 1 = 0 -> idx = (0 << 1) + 0 = 0 -> 0x71 */
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0x71);

        gb_write(&gb, wLinkUsingShovelTimer, 0x10);
        func_002_4B49(&gb, NULL, NULL);
        /* timer = 0x11 -> (timer >> 4) & 1 = 1 -> idx = (0 << 1) + 1 = 1 -> 0x72 */
        assert(gb_read_hram(&gb, hLinkAnimationState) == 0x72);
    }

}
