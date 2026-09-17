#include "bank2/revolving_door.h"
#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "home/bank.h"
#include "home/gameplay.h"

/* Bank 2 Revolving Door & Tile/Palette Data Tables */
const uint8_t Data_002_4E1C[16] = {
    0xFF, 0x47, 0x00, 0x00, 0x0C, 0x39, 0x76, 0x5E,
    0xFF, 0x47, 0x00, 0x00, 0x67, 0x28, 0x76, 0x5E
};

const uint8_t LinkRevolvingDoorAnimation[7] = {
    LINK_ANIMATION_STATE_REVOLVING_DOOR_1,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_2,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_3,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_4,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_4,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_5,
    LINK_ANIMATION_STATE_REVOLVING_DOOR_3
};

void label_002_4D97(GBState *gb, void (*get_bg_attr_addr)(GBState *)) {
    if (!gb) return;

    uint8_t mp0 = gb_read_hram(gb, hMultiPurpose0);
    gb_write_hram(gb, hIntersectedObjectLeft, mp0);
    uint8_t e = (uint8_t)((mp0 >> 4) & 0x0F);

    uint8_t mp1 = gb_read_hram(gb, hMultiPurpose1);
    gb_write_hram(gb, hIntersectedObjectTop, mp1);
    uint8_t tile_idx = (uint8_t)((mp1 & 0xF0) | e);

    gb_write(gb, (uint16_t)(wRoomObjects + tile_idx), 0xAE);
    GetIntersectedObjectBGAddress(gb);

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_91D_jp_92E(gb, 0x02B8, 0x02, get_bg_attr_addr);
    }

    uint8_t draw_size = gb_read(gb, wDrawCommandsSize);
    uint16_t hl = (uint16_t)(wDrawCommand + draw_size);
    gb_write(gb, wDrawCommandsSize, (uint8_t)(draw_size + 0x0A));

    uint8_t bg_high = gb_read_hram(gb, hIntersectedObjectBGAddressHigh);
    uint8_t bg_low = gb_read_hram(gb, hIntersectedObjectBGAddressLow);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, bg_low);
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, 0x76);
    gb_write(gb, hl++, 0x77);

    gb_write(gb, hl++, bg_high);
    gb_write(gb, hl++, (uint8_t)(bg_low + 1));
    gb_write(gb, hl++, 0x81);
    gb_write(gb, hl++, 0x76);
    gb_write(gb, hl++, 0x77);

    gb_write(gb, hl, 0x00);
}

void func_002_4DFC(GBState *gb) {
    if (!gb) return;

    for (uint8_t c = 0; c < 8; c++) {
        uint8_t b = gb->wram[1][(wObjPal1 + c) - 0xD000];
        gb->wram[2][(wObjPal1 + c) - 0xD000] = b;
    }
    gb_write_hram(gb, rSVBK, 0);
}

void func_002_4E2C(GBState *gb, uint16_t de) {
    if (!gb) return;

    for (uint8_t c = 0; c < 8; c++) {
        uint8_t val = Data_002_4E1C[(de + c) & 0x0F];
        gb_write(gb, (uint16_t)(wObjPal8 + c), val);
    }
    gb_write(gb, wPaletteDataFlags, 0x02);
}

void func_002_4E48(GBState *gb) {
    if (!gb) return;

    for (uint8_t e = 0; e < 8; e++) {
        uint8_t b = gb->wram[2][(wObjPal8 + e) - 0xD000];
        gb->wram[1][(wObjPal8 + e) - 0xD000] = b;
    }
    gb_write_hram(gb, rSVBK, 0);
    gb_write(gb, wPaletteDataFlags, 0x02);
}

void func_002_4EDD(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wLinkAnimationFrame, 0);
    gb_write(gb, wC167, 0);
    gb_write(gb, wBGPaletteTransitionEffect, 0);
    gb_write(gb, wDDD7, 0);
    gb_write(gb, wLinkMotionState, LINK_MOTION_DEFAULT);
}

void LinkMotionRevolvingDoorHandler(GBState *gb) {
    if (!gb) return;

    gb_write_hram(gb, hLinkPositionY, 0x10);
    gb_write_hram(gb, hLinkPositionX, 0x50);
    gb_write(gb, wC167, 0x50);

    uint8_t frame = gb_read(gb, wLinkAnimationFrame);
    uint8_t de_offset = 0x08;

    if (frame == 0x18) {
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4E2C(gb, de_offset);
        }
    } else if (frame == 0x28) {
        de_offset = 0x00;
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4E2C(gb, de_offset);
        }
    } else if (frame == 0) {
        if (gb_read_hram(gb, hIsGBC) != 0) {
            func_002_4DFC(gb);
        }
    }

    uint8_t idx = (frame >> 3) & 0x07;
    gb_write_hram(gb, hLinkAnimationState, LinkRevolvingDoorAnimation[idx]);

    frame++;
    gb_write(gb, wLinkAnimationFrame, frame);

    if (frame >= 0x38) {
        gb_write_hram(gb, hLinkPositionY, 0xFB);
        gb_write_hram(gb, hLinkDirection, DIRECTION_UP);
        gb_write_hram(gb, hLinkAnimationState, LINK_ANIMATION_STATE_STANDING_UP);
    }

    if (frame != 0x48) {
        return;
    }

    if (gb_read_hram(gb, hIsGBC) != 0) {
        func_002_4E48(gb);
    }

    gb_write(gb, wRoomTransitionDirection, 0x02);
    gb_write(gb, wRoomTransitionState, 0x01);
    func_002_4EDD(gb);
}
