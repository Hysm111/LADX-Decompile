#include "test_bank2.h"

#include "gb.h"
#include "bank2/revolving_door.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_revolving_door(void) {
    /* Test 42: Data_002_4E1C and LinkRevolvingDoorAnimation Tables */
    {
        assert(sizeof(Data_002_4E1C) == 16);
        assert(sizeof(LinkRevolvingDoorAnimation) == 7);

        assert(Data_002_4E1C[0] == 0xFF);
        assert(Data_002_4E1C[1] == 0x47);
        assert(Data_002_4E1C[2] == 0x00);
        assert(Data_002_4E1C[3] == 0x00);
        assert(Data_002_4E1C[4] == 0x0C);
        assert(Data_002_4E1C[5] == 0x39);
        assert(Data_002_4E1C[6] == 0x76);
        assert(Data_002_4E1C[7] == 0x5E);
        assert(Data_002_4E1C[8] == 0xFF);
        assert(Data_002_4E1C[9] == 0x47);
        assert(Data_002_4E1C[10] == 0x00);
        assert(Data_002_4E1C[11] == 0x00);
        assert(Data_002_4E1C[12] == 0x67);
        assert(Data_002_4E1C[13] == 0x28);
        assert(Data_002_4E1C[14] == 0x76);
        assert(Data_002_4E1C[15] == 0x5E);

        assert(LinkRevolvingDoorAnimation[0] == LINK_ANIMATION_STATE_REVOLVING_DOOR_1);
        assert(LinkRevolvingDoorAnimation[1] == LINK_ANIMATION_STATE_REVOLVING_DOOR_2);
        assert(LinkRevolvingDoorAnimation[2] == LINK_ANIMATION_STATE_REVOLVING_DOOR_3);
        assert(LinkRevolvingDoorAnimation[3] == LINK_ANIMATION_STATE_REVOLVING_DOOR_4);
        assert(LinkRevolvingDoorAnimation[4] == LINK_ANIMATION_STATE_REVOLVING_DOOR_4);
        assert(LinkRevolvingDoorAnimation[5] == LINK_ANIMATION_STATE_REVOLVING_DOOR_5);
        assert(LinkRevolvingDoorAnimation[6] == LINK_ANIMATION_STATE_REVOLVING_DOOR_3);
    }

    /* Test 43: label_002_4D97 Room Tile Replacement & Draw Command */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. DMG mode: updates wRoomObjects, BG address, writes 10 bytes to wDrawCommand */
        gb_write_hram(&gb, hMultiPurpose0, 0x30); /* left = 0x30, e = 3 */
        gb_write_hram(&gb, hMultiPurpose1, 0x20); /* top = 0x20, tile_idx = 0x23 */
        gb_write_hram(&gb, hIsGBC, 0);
        gb_write(&gb, wDrawCommandsSize, 0);

        label_002_4D97(&gb, NULL);

        assert(gb_read_hram(&gb, hIntersectedObjectLeft) == 0x30);
        assert(gb_read_hram(&gb, hIntersectedObjectTop) == 0x20);
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x23)) == 0xAE);
        assert(gb_read(&gb, wDrawCommandsSize) == 10);

        uint16_t bg_high = gb_read_hram(&gb, hIntersectedObjectBGAddressHigh);
        uint16_t bg_low = gb_read_hram(&gb, hIntersectedObjectBGAddressLow);
        assert(gb_read(&gb, wDrawCommand + 0) == bg_high);
        assert(gb_read(&gb, wDrawCommand + 1) == bg_low);
        assert(gb_read(&gb, wDrawCommand + 2) == 0x81);
        assert(gb_read(&gb, wDrawCommand + 3) == 0x76);
        assert(gb_read(&gb, wDrawCommand + 4) == 0x77);
        assert(gb_read(&gb, wDrawCommand + 5) == bg_high);
        assert(gb_read(&gb, wDrawCommand + 6) == (uint8_t)(bg_low + 1));
        assert(gb_read(&gb, wDrawCommand + 7) == 0x81);
        assert(gb_read(&gb, wDrawCommand + 8) == 0x76);
        assert(gb_read(&gb, wDrawCommand + 9) == 0x77);
        assert(gb_read(&gb, wDrawCommand + 10) == 0x00);
    }

    /* Test 44: func_002_4DFC, func_002_4E2C, and func_002_4E48 Palette Handlers */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. func_002_4DFC: syncs wObjPal1 from WRAM bank 1 to bank 2 */
        for (uint8_t i = 0; i < 8; i++) {
            gb.wram[1][(wObjPal1 + i) - 0xD000] = (uint8_t)(0x10 + i);
            gb.wram[2][(wObjPal1 + i) - 0xD000] = 0x00;
        }
        func_002_4DFC(&gb);
        for (uint8_t i = 0; i < 8; i++) {
            assert(gb.wram[2][(wObjPal1 + i) - 0xD000] == (uint8_t)(0x10 + i));
        }

        /* 2. func_002_4E2C: copies Data_002_4E1C + de into wObjPal8 */
        gb_write(&gb, wPaletteDataFlags, 0);
        func_002_4E2C(&gb, 0x08);
        assert(gb_read(&gb, wPaletteDataFlags) == 0x02);
        for (uint8_t i = 0; i < 8; i++) {
            assert(gb_read(&gb, (uint16_t)(wObjPal8 + i)) == Data_002_4E1C[8 + i]);
        }

        /* 3. func_002_4E48: restores wObjPal8 from WRAM bank 2 to bank 1 */
        for (uint8_t i = 0; i < 8; i++) {
            gb.wram[2][(wObjPal8 + i) - 0xD000] = (uint8_t)(0x55 + i);
            gb.wram[1][(wObjPal8 + i) - 0xD000] = 0x00;
        }
        gb_write(&gb, wPaletteDataFlags, 0);
        func_002_4E48(&gb);
        assert(gb_read(&gb, wPaletteDataFlags) == 0x02);
        for (uint8_t i = 0; i < 8; i++) {
            assert(gb.wram[1][(wObjPal8 + i) - 0xD000] == (uint8_t)(0x55 + i));
        }
    }

    /* Test 45: LinkMotionRevolvingDoorHandler and func_002_4EDD */
    {
        GBState gb;
        gb_init(&gb);

        /* Frame 0: sets Link pos, wC167, animation state, advances frame */
        gb_write(&gb, wLinkAnimationFrame, 0);
        gb_write_hram(&gb, hIsGBC, 1);
        LinkMotionRevolvingDoorHandler(&gb);

        assert(gb_read_hram(&gb, hLinkPositionY) == 0x10);
        assert(gb_read_hram(&gb, hLinkPositionX) == 0x50);
        assert(gb_read(&gb, wC167) == 0x50);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LinkRevolvingDoorAnimation[0]);
        assert(gb_read(&gb, wLinkAnimationFrame) == 1);

        /* Frame 0x18: calls func_002_4E2C(gb, 8) */
        gb_write(&gb, wLinkAnimationFrame, 0x18);
        LinkMotionRevolvingDoorHandler(&gb);
        assert(gb_read(&gb, (uint16_t)(wObjPal8 + 4)) == Data_002_4E1C[8 + 4]);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0x19);

        /* Frame 0x28: calls func_002_4E2C(gb, 0) */
        gb_write(&gb, wLinkAnimationFrame, 0x28);
        LinkMotionRevolvingDoorHandler(&gb);
        assert(gb_read(&gb, (uint16_t)(wObjPal8 + 4)) == Data_002_4E1C[0 + 4]);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0x29);

        /* Frame >= 0x38: moves Link to exit top position (Y=0xFB, UP, STANDING_UP) */
        gb_write(&gb, wLinkAnimationFrame, 0x37);
        LinkMotionRevolvingDoorHandler(&gb);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0x38);
        assert(gb_read_hram(&gb, hLinkPositionY) == 0xFB);
        assert(gb_read_hram(&gb, hLinkDirection) == DIRECTION_UP);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_STANDING_UP);

        /* Frame 0x47 -> 0x48: triggers room transition and func_002_4EDD */
        gb_write(&gb, wLinkAnimationFrame, 0x47);
        LinkMotionRevolvingDoorHandler(&gb);
        assert(gb_read(&gb, wRoomTransitionDirection) == 0x02);
        assert(gb_read(&gb, wRoomTransitionState) == 0x01);
        assert(gb_read(&gb, wLinkAnimationFrame) == 0);
        assert(gb_read(&gb, wC167) == 0);
        assert(gb_read(&gb, wBGPaletteTransitionEffect) == 0);
        assert(gb_read(&gb, wDDD7) == 0);
        assert(gb_read(&gb, wLinkMotionState) == LINK_MOTION_DEFAULT);
    }

}
